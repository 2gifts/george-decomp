import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_lifecycle import LifecycleTrace,BUFFER,END,RANGES,HELPER_RANGES,CALLS,RETURN


class LifecycleGuardTests(unittest.TestCase):
    def trace(self):
        t=LifecycleTrace(b'')
        for address in range(BUFFER,BUFFER+16):t.memory[address]=0x55
        return t

    def test_observed_byte_and_half_stores(self):
        t=self.trace();t.r[4]=BUFFER;t.r[5]=0x1234ABCD
        self.assertEqual(t.execute((0x28<<26)|(4<<21)|(5<<16)|7,0),(None,False))
        self.assertEqual(t.load(BUFFER+6,2),0xCD55)
        t.execute((0x29<<26)|(4<<21)|(5<<16)|2,0)
        self.assertEqual(t.load(BUFFER,4),0xABCD5555)
        self.assertEqual(t.load(BUFFER+4,2),0x5555)
        with self.assertRaises(ValueError):t.execute((0x29<<26)|(4<<21)|(5<<16)|1,0)

    def test_only_complete_reviewed_code(self):
        t=self.trace()
        for pc in (RANGES[0][0],HELPER_RANGES[0][0]):
            with self.assertRaises(ValueError):t.fetch(pc)
        for pc in (RANGES[0][1],RANGES[0][0]+1,0x2A1C30):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)

    def test_unknown_memory_and_readonly_permissions(self):
        t=self.trace()
        for p in (BUFFER-1,END,0x4455D0,0x4455A0):
            with self.assertRaises(ValueError):t.load(p,4)
            with self.assertRaises(ValueError):t.save(p,1,4)
        with self.assertRaises(ValueError):t.save(END-1,1,2)
        with self.assertRaises((KeyError,ValueError)):t.load(BUFFER+32,4)

    def test_controlled_callee_arguments(self):
        t=self.trace()
        for target in (*CALLS,0x393B74):
            with self.assertRaises(ValueError):t.library_call(target)
        with self.assertRaises(ValueError):t.execute(0x4BE000D3,0)

    def test_actual_link_register_return_and_owned_delay(self):
        entry,end=HELPER_RANGES[0]
        original=bytearray(end-0xFF000)
        # JR a0 must not be accepted as a normal function return.
        struct.pack_into('<II',original,entry-0xFF000,0x00800008,0)
        t=LifecycleTrace(original);t.r[4]=RETURN;t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        # A JAL whose target is the stop is still a call, never a return.
        struct.pack_into('<II',original,entry-0xFF000,0x0FFFFFFF,0)
        t=LifecycleTrace(original);t.r[31]=0x0FFFFFFC
        with self.assertRaises(ValueError):t.run(entry,0x0FFFFFFC)
        # Last instruction cannot use a delay from outside this owned body.
        struct.pack_into('<I',original,entry-0xFF000,(4<<26)|3)
        struct.pack_into('<I',original,end-4-0xFF000,0x03E00008)
        t=LifecycleTrace(original);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

    def test_inherited_reserved_operands(self):
        t=self.trace()
        for instruction in (0x00810809,0x00A1F809,0x00A0F849,0x0081086D,0x46010006,0x44850801):
            with self.assertRaises(ValueError):t.execute(instruction,0)


if __name__=='__main__':unittest.main()
