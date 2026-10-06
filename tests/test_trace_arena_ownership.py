import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_arena_ownership import OwnershipTrace,BUFFER,END,RECORD,OWNER,CAPACITY,ARENA,RANGES,HELPERS,MEMORY_RANGES,ALLOCATE,FREE,RETURN


class OwnershipGuardTests(unittest.TestCase):
    def trace(self,original=b'',mutation=0):
        t=OwnershipTrace(original,mutation=mutation)
        for a,b in MEMORY_RANGES:
            for p in range(a,b):t.memory[p]=0x55
        return t

    def test_fetch_only_selected_and_two_helpers(self):
        t=self.trace()
        for pc in (RANGES[0][0],RANGES[0][1],RANGES[4][1],RANGES[-1][1],0x2B8E88,RANGES[0][0]+1):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)
        entry,end=RANGES[0];t=self.trace(bytearray(end-0xFF000))
        self.assertEqual(t.fetch(entry),0);self.assertEqual(t.fetch(end-4),0)

    def test_memory_windows_alignment_and_initialized_bytes(self):
        t=self.trace()
        for address,size in ((BUFFER-1,4),(END,4),(ARENA+24,4),(ARENA+16,16),(BUFFER+1,4),(BUFFER,2),(OWNER-4,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,1,size)
        del t.memory[BUFFER]
        with self.assertRaises(ValueError):t.save(BUFFER,1,4)
        with self.assertRaises(ValueError):t.load(BUFFER,4)

    def test_sllv_mult_mflo_signed_and_unsigned_edges(self):
        t=self.trace();t.r[2],t.r[3]=1,63
        t.execute((3<<21)|(2<<16)|(4<<11)|4,0)
        self.assertEqual(t.r[4],0x80000000)
        t.r[2],t.r[3]=0xFFFFFFFF,12
        t.execute((2<<21)|(3<<16)|0x18,4)
        t.execute((5<<11)|0x12,8)
        self.assertEqual(t.r[5],0xFFFFFFF4);self.assertEqual(t.r[0],0)
        t.execute((2<<21)|(3<<16)|(6<<11)|0x2A,12)
        t.execute((2<<21)|(3<<16)|(7<<11)|0x2B,16)
        self.assertEqual((t.r[6],t.r[7]),(1,0))
        t.execute((3<<21)|(2<<16)|(8<<11)|0x23,20)
        self.assertEqual(t.r[8],13)
        t.r[2]=0x80000000;t.r[3]=4
        t.execute((2<<21)|(3<<16)|(2<<11)|0x18,24)
        self.assertEqual(t.r[2],0);self.assertEqual(t.lo,0)

    def test_reserved_operands_uninitialized_lo_and_unowned_opcodes_fail(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.execute((2<<11)|0x12,0)
        for word in ((2<<21)|(3<<16)|(4<<11)|(1<<6)|4,
                     (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x18,
                     (2<<21)|(4<<11)|0x12,(3<<16)|(4<<11)|0x12,
                     (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x23,
                     (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x2A,
                     (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x2B,
                     (2<<21)|(3<<16)|(4<<11),0x0081086D,0x3C220001,
                     0x46000000,0x4BE000D3,(2<<21)|(31<<11)|9):
            with self.assertRaises(ValueError):t.execute(word,0)

    def test_precise_controlled_calls_events_mutation_clobber(self):
        t=self.trace(mutation=1)
        with self.assertRaises(ValueError):t.library_call(0x2AEC2C)
        t.allocation=END-44;t.r[4]=48
        with self.assertRaises(ValueError):t.library_call(ALLOCATE)
        self.assertEqual(t.events,[])
        t.allocation=RECORD;t.library_call(ALLOCATE)
        self.assertEqual(len(t.events),10);self.assertEqual(t.events[:2],[0,48])
        self.assertEqual(t.load(CAPACITY,4),65)
        self.assertEqual(t.r[2],RECORD);self.assertEqual(t.r[4:16],[0xDEADBEEF]*12)
        self.assertEqual(t.fp[:20],[0x3F400000]*20)
        self.assertEqual(t.fp[20],0x3E800014)
        t.r[4]=RECORD;t.library_call(FREE)
        self.assertEqual(t.counts,[1,1]);self.assertEqual(t.events[10:12],[1,RECORD])
        self.assertEqual(t.load(CAPACITY,4),99)

    def test_finite_raw_cop1_moves_aliases_and_reserved_forms(self):
        t=self.trace();t.r[2]=0x80000000
        t.execute((0x11<<26)|(4<<21)|(2<<16)|(12<<11),0)
        t.execute((0x11<<26)|(16<<21)|(12<<11)|(12<<6)|6,4)
        self.assertEqual(t.fp[12],0x80000000)
        t.r[3]=BUFFER
        t.execute((0x39<<26)|(3<<21)|(12<<16),8)
        t.execute((0x31<<26)|(3<<21)|(13<<16),12)
        self.assertEqual(t.fp[13],0x80000000)
        t.r[2]=0x7F800000
        for w in ((0x11<<26)|(4<<21)|(2<<16)|(12<<11),
                  (0x11<<26)|(4<<21)|(2<<16)|(12<<11)|1,
                  (0x11<<26)|(16<<21)|(1<<16)|(12<<11)|(13<<6)|6,
                  (0x11<<26)|(16<<21)|(12<<11)|(13<<6),
                  (0x11<<26)|(16<<21)|(12<<11)|(13<<6)|7):
            with self.assertRaises(ValueError):t.execute(w,16)
        t.save(BUFFER,0x7FC00000,4)
        with self.assertRaises(ValueError):t.execute((0x31<<26)|(3<<21)|(13<<16),20)
        t.fp[12]=0xFF800000
        with self.assertRaises(ValueError):t.execute((0x39<<26)|(3<<21)|(12<<16),24)
        t.r[2]=0x12340000;t.execute((0xD<<26)|(2<<21)|(3<<16)|0x5678,28)
        self.assertEqual(t.r[3],0x12345678)

    def test_actual_jr31_only_and_internal_selected_return_not_early_stop(self):
        entry,end=RANGES[0];raw=bytearray(end-0xFF000)
        for word in (0x00800008,(2<<26)|(RANGES[1][0]>>2),(3<<26)|0x3FFFFFF):
            struct.pack_into('<II',raw,entry-0xFF000,word,0)
            t=self.trace(raw);t.r[4]=t.r[31]=RETURN
            with self.assertRaises(ValueError):t.run(entry)
        raw=bytearray(end-0xFF000);struct.pack_into('<II',raw,end-8-0xFF000,0x03E00008,0)
        t=self.trace(raw);t.r[31]=entry+8;t.run(entry,entry+8)
        self.assertEqual(t.instruction_count,(end-entry)//4)
        t=self.trace(raw);t.r[31]=entry+8
        with self.assertRaises(ValueError):t.run(entry)

    def test_ordinary_delay_likely_annul_and_owned_terminal_bounds(self):
        entry,end=RANGES[0];raw=bytearray(end-0xFF000)
        struct.pack_into('<II',raw,entry-0xFF000,0x14000000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        # Untaken BNEL skips its forbidden delay and reaches the real return.
        struct.pack_into('<II',raw,entry-0xFF000,0x54000000,0xFFFFFFFF)
        struct.pack_into('<II',raw,entry+8-0xFF000,0x03E00008,0)
        t=self.trace(raw);t.r[31]=RETURN;t.run(entry)
        self.assertEqual(t.instruction_count,3)
        raw=bytearray(end-0xFF000)
        struct.pack_into('<I',raw,entry-0xFF000,(4<<26)|((end-entry-8)//4))
        struct.pack_into('<I',raw,end-4-0xFF000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

    def test_owned_loop_still_has_instruction_bound(self):
        entry,end=RANGES[0];raw=bytearray(end-0xFF000)
        struct.pack_into('<II',raw,entry-0xFF000,0x1000FFFF,0)
        t=self.trace(raw)
        with self.assertRaisesRegex(ValueError,'bound'):t.run(entry)


if __name__=='__main__':unittest.main()
