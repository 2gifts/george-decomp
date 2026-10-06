import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_management import ManagementTrace,BUFFER,END,NODE,RECORD,LIST,RANGES,ALL_RANGES,CALLS,RETURN


class ManagementGuardTests(unittest.TestCase):
    def trace(self):
        t=ManagementTrace(b'')
        for p in range(BUFFER,END):t.memory[p]=0x55
        return t

    def test_only_complete_reviewed_code_is_fetched(self):
        t=self.trace()
        for pc in (RANGES[0][0],RANGES[-1][1],0x2B9660,0x2A1C30,RANGES[0][0]+1):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)
        entry,end=RANGES[0];t=ManagementTrace(bytearray(end-0xFF000))
        self.assertEqual(t.fetch(entry),0);self.assertEqual(t.fetch(end-4),0)

    def test_authored_memory_is_bounded_and_initialized(self):
        t=self.trace()
        for address in (BUFFER-1,END,0x4455A0,0x3D48E8):
            with self.assertRaises(ValueError):t.load(address,4)
            with self.assertRaises(ValueError):t.save(address,1,4)
        with self.assertRaises(ValueError):t.save(END-1,1,2)
        with self.assertRaises((KeyError,ValueError)):t.load(0x7F000,4)

    def test_local_lb_xor_and_inherited_reserved_forms(self):
        t=self.trace();t.r[4]=BUFFER;t.save(BUFFER,0x80,1)
        t.execute((0x20<<26)|(4<<21)|(2<<16),0)
        self.assertEqual(t.r[2],0xFFFFFF80)
        t.execute((2<<21)|(3<<11)|0x2D,4)
        self.assertEqual(t.r[3],0xFFFFFF80)
        t.r[5]=t.r[3]
        self.assertEqual(t.execute((4<<26)|(2<<21)|(5<<16)|1,8),(16,False))
        t.execute((2<<21)|(3<<16)|(6<<11)|0x26,12)
        self.assertEqual(t.r[6],0)
        for insn in ((2<<21)|(3<<16)|(6<<11)|(1<<6)|0x26,0x0081086D,0x46010006,0x44850801,0x4BE000D3):
            with self.assertRaises(ValueError):t.execute(insn,0)

    def test_unknown_calls_and_precise_allocator_contracts_fail(self):
        t=self.trace();t.r[4],t.r[5]=0xC0,5
        with self.assertRaises(ValueError):t.library_call(CALLS[0])
        self.assertEqual(t.events,[])
        with self.assertRaises(ValueError):t.library_call(0x2B9660)
        t.r[5]=4;t.f[12]=1.25;t.library_call(CALLS[0])
        self.assertEqual(t.r[2],NODE);self.assertEqual(t.r[4],0xDEADBEEF)
        self.assertEqual(t.f[12],77.0)
        t=ManagementTrace(b'',routine=3)
        for p in range(BUFFER,END):t.memory[p]=0
        t.r[4]=13
        with self.assertRaises(ValueError):t.library_call(CALLS[1])
        t=self.trace();t.r[4]=0x30000
        with self.assertRaises(ValueError):t.library_call(CALLS[4])

    def test_aligned_strcpy_vu_and_helper_contracts_are_not_emulated(self):
        t=self.trace();t.r[4],t.r[5]=BUFFER,BUFFER+32
        with self.assertRaises(ValueError):t.library_call(CALLS[6])
        self.assertEqual(t.events,[])
        t.r[4]=NODE+0x74
        with self.assertRaises(ValueError):t.library_call(CALLS[3])
        t.r[4]=NODE
        with self.assertRaises(ValueError):t.library_call(CALLS[7])
        t.r[4],t.r[5]=LIST,LIST
        with self.assertRaises(ValueError):t.library_call(CALLS[8])

    def test_only_actual_jr31_can_end_owned_body(self):
        entry,end=RANGES[0];raw=bytearray(end-0xFF000)
        for insn in (0x00800008,(2<<26)|(RANGES[1][0]>>2),(3<<26)|0x3FFFFFF):
            struct.pack_into('<II',raw,entry-0xFF000,insn,0)
            t=ManagementTrace(raw);t.r[4],t.r[31]=RETURN,RETURN
            with self.assertRaises(ValueError):t.run(entry)
        struct.pack_into('<II',raw,entry-0xFF000,0x03E00008,0)
        t=ManagementTrace(raw);t.r[31]=entry+8
        with self.assertRaises(ValueError):t.run(entry)
        # Merely passing the selected internal return PC does not end execution.
        raw=bytearray(end-0xFF000);struct.pack_into('<II',raw,end-8-0xFF000,0x03E00008,0)
        t=ManagementTrace(raw);t.r[31]=entry+8;t.run(entry,entry+8)
        self.assertEqual(t.instruction_count,(end-entry)//4)

    def test_untaken_delays_and_terminal_delay_bounds(self):
        entry,end=RANGES[0];raw=bytearray(end-0xFF000)
        struct.pack_into('<II',raw,entry-0xFF000,0x14000000,0x03E00008)
        t=ManagementTrace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        raw=bytearray(end-0xFF000)
        struct.pack_into('<I',raw,entry-0xFF000,(4<<26)|((end-entry-8)//4))
        struct.pack_into('<I',raw,end-4-0xFF000,0x03E00008)
        t=ManagementTrace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)


if __name__=='__main__':unittest.main()
