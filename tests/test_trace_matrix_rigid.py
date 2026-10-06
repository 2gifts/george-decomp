import math
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from trace_matrix_rigid import MatrixRigidTrace, ENTRY, END, BUFFER, RETURN


class MatrixRigidGuards(unittest.TestCase):
    def test_scope_and_unknown_memory(self):
        trace = MatrixRigidTrace(b'')
        for pc in (ENTRY-4, ENTRY+1, END, 0x299D68):
            with self.assertRaises(ValueError):
                trace.fetch(pc)
        with self.assertRaises(ValueError):
            trace.fetch(ENTRY)
        with self.assertRaises(KeyError):
            trace.load(BUFFER, 4)
        with self.assertRaises(ValueError):
            trace.save(BUFFER, 0, 4)
        with self.assertRaises(ValueError):
            trace.library_call(0x2A3538)

    def test_reused_reserved_and_finite_guards(self):
        trace = MatrixRigidTrace(b'')
        # NEG.S must leave its unused ft field zero.
        reserved = (17 << 26) | (16 << 21) | (1 << 16) | 7
        with self.assertRaises(ValueError):
            trace.execute(reserved, ENTRY)
        trace.f[0] = math.inf
        multiply = (17 << 26) | (16 << 21) | 2
        with self.assertRaises(ValueError):
            trace.execute(multiply, ENTRY)
        with self.assertRaises(ValueError):
            trace.single(BUFFER, math.nan)

    def test_control_transfer_and_delay_scope(self):
        class TransferTrace(MatrixRigidTrace):
            def fetch(self, pc):
                return (3 << 26) | (0x20000 >> 2)
        with self.assertRaises(ValueError):
            TransferTrace(b'').run()

        class FakeReturnTrace(MatrixRigidTrace):
            def fetch(self, pc):
                return (3 << 26) | (RETURN >> 2)
        with self.assertRaises(ValueError):
            FakeReturnTrace(b'').run()

        class DelayTrace(MatrixRigidTrace):
            def fetch(self, pc):
                return (31 << 21) | 8
        trace = DelayTrace(b'')
        trace.r[31] = RETURN
        with self.assertRaises(ValueError):
            trace.run()


if __name__ == '__main__':
    unittest.main()
