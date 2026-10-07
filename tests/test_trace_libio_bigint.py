"""Guards for the bounded initialized original Bigint observer."""
import copy
import importlib.util
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import trace_libio_bigint as model


class Guards(unittest.TestCase):
    def dummy(self):
        return model.LibioBigintTrace(bytes(0x300000), model.fixtures()[0])

    def test_unowned_memory(self):
        with self.assertRaisesRegex(ValueError, 'unowned'):
            self.dummy().load(0x1000, 4)

    def test_uninitialized_memory(self):
        with self.assertRaisesRegex(ValueError, 'uninitialized'):
            self.dummy().load(model.BASE, 4)

    def test_unaligned_memory(self):
        with self.assertRaisesRegex(ValueError, 'unaligned'):
            self.dummy().save(model.BASE + 1, 2, 4)

    def test_unreviewed_pc(self):
        with self.assertRaisesRegex(ValueError, 'unreviewed'):
            self.dummy().fetch(0x35ea54)

    def test_budget_rejects_before_mutation(self):
        trace = self.dummy()
        trace.instruction_count = 20000
        before = copy.deepcopy((trace.r, trace.memory, trace.instruction_count, trace.visited))
        with self.assertRaisesRegex(ValueError, 'budget before mutation'):
            trace.execute(0, 0x35e9e0)
        self.assertEqual(before, (trace.r, trace.memory, trace.instruction_count, trace.visited))

    @unittest.skipUnless((ROOT / 'orig/SLUS_216.68').is_file(), 'original absent')
    def test_changed_instruction_rejected(self):
        trace = model.LibioBigintTrace((ROOT / 'orig/SLUS_216.68').read_bytes(), model.fixtures()[0])
        with self.assertRaisesRegex(ValueError, 'differs'):
            trace.execute(0, 0x35e9e0)

    @unittest.skipUnless((ROOT / 'orig/SLUS_216.68').is_file(), 'original absent')
    def test_wrong_selected_return(self):
        trace = model.LibioBigintTrace((ROOT / 'orig/SLUS_216.68').read_bytes(), model.fixtures()[0])
        trace.initialize()
        trace.r[4], trace.r[5] = model.A, 0
        trace.r[31] = model.RETURN + 4
        with self.assertRaisesRegex(ValueError, 'actual JR31'):
            trace.run(model.SELECTED[0][0])

    @unittest.skipUnless((ROOT / 'orig/SLUS_216.68').is_file(), 'original absent')
    def test_invalid_initialized_capacity(self):
        parameters = model.fixtures()[0].copy()
        parameters['a_k'] = 2
        trace = model.LibioBigintTrace((ROOT / 'orig/SLUS_216.68').read_bytes(), parameters)
        with self.assertRaisesRegex(ValueError, 'capacity'):
            trace.initialize()

    @unittest.skipUnless((ROOT / 'orig/SLUS_216.68').is_file(), 'original absent')
    def test_captured_wds_after_free_mutation(self):
        parameters = next(p for p in model.fixtures() if p['mutation'])
        observation = model.LibioBigintTrace((ROOT / 'orig/SLUS_216.68').read_bytes(), parameters).observe()
        offset = observation['return_value'] // 4
        self.assertEqual(observation['memory'][offset + 4], 9)
        self.assertEqual(observation['memory'][offset + 5 + 8], 9)
        self.assertEqual(observation['events'], [[1, 84, model.NEW - model.BASE], [2, model.A - model.BASE, 0]])


if __name__ == '__main__':
    unittest.main()
