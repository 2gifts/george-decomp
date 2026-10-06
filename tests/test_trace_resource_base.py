import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_resource_base import ResourceBaseTrace,BUFFER,ENTRIES,HELPERS,RANGES
from trace_geometry import RETURN


class ResourceBaseTraceGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(max(b for a,b in (*RANGES,*HELPERS))-0xFF000)
        for i,value in enumerate(instructions):struct.pack_into('<I',image,ENTRIES[0]-0xFF000+i*4,value)
        return ResourceBaseTrace(bytes(image))

    def test_code_and_entry_scope(self):
        t=self.trace()
        for address in (ENTRIES[0]+1,RANGES[0][1],0x100000):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.run(ENTRIES[0]+4)

    def test_memory_bounds_alignment_and_initialization(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.load(BUFFER,4)
        for i in range(16):t.memory[BUFFER+i]=0
        with self.assertRaises(ValueError):t.save(BUFFER+1,0,4)
        with self.assertRaises(ValueError):t.load(BUFFER-4,4)
        with self.assertRaises(ValueError):t.save(BUFFER,0,3)

    def test_reserved_and_unreviewed_opcodes(self):
        t=self.trace();pc=ENTRIES[0]
        for w in (0x46000000,0x70000000,(15<<26)|(1<<21),
                  (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x18,
                  (2<<21)|(3<<16)|(4<<11)|0x1B,
                  (2<<21)|(1<<11)|0x12,
                  (2<<21)|(1<<16)|(31<<11)|9):
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,pc)

    def test_division_producer_and_zero_divisor(self):
        t=self.trace();pc=ENTRIES[0]
        with self.assertRaises(ValueError):t.execute((4<<11)|0x12,pc)
        t.r[2],t.r[3]=0xFFFFFFFF,0
        div=(2<<21)|(3<<16)|0x1B
        with self.assertRaises(ValueError):t.execute(div,pc)
        t.r[3]=7;t.execute(div,pc);t.execute((4<<11)|0x12,pc);t.execute((5<<11)|0x10,pc)
        self.assertEqual((t.r[4],t.r[5]),(0xFFFFFFFF//7,0xFFFFFFFF%7))

    def test_untaken_branch_must_execute_delay(self):
        # BNE zero,zero falls through; its unsupported delay must be rejected.
        t=self.trace(((5<<26)|2,0x46000000,0x03E00008,0))
        t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unsupported resource opcode'):t.run(ENTRIES[0])

    def test_annulled_likely_delay_and_control_in_delay(self):
        t=self.trace(((21<<26)|2,0x46000000,0x03E00008,0));t.r[31]=RETURN
        t.run(ENTRIES[0])
        self.assertEqual(t.instruction_count,3)
        t=self.trace(((4<<26)|2,(4<<26)|2,0,0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(ENTRIES[0])

    def test_actual_jr31_required_even_for_internal_stop(self):
        entry=ENTRIES[0];stop=entry+8
        t=self.trace(((4<<26)|1,0,(9<<26)|(2<<16)|9,0x03E00008,0));t.r[31]=stop
        t.run(entry,stop);self.assertEqual(t.r[2],9)
        t=self.trace(((2<<21)|8,0));t.r[2]=stop
        with self.assertRaisesRegex(ValueError,'unsupported resource return'):t.run(entry,stop)
        t=self.trace(((3<<26)|((stop>>2)&0x3FFFFFF),0));t.r[31]=stop
        with self.assertRaisesRegex(ValueError,'unknown resource controlled call'):t.run(entry,stop)

    def test_unknown_call_and_observed_allocation_lanes(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.library_call(0x123456)
        t.r[4],t.r[5],t.r[6]=0x22500,12,0
        with self.assertRaisesRegex(ValueError,'allocation lanes'):t.library_call(0x2AD700)


if __name__=='__main__':unittest.main()
