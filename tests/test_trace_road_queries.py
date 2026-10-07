"""Strict decoder boundaries and actual road-query alias/width invariants."""
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_road_queries as model
from analyze import validated_elf
from trace_geometry import RETURN,word

LOCAL_ORIGINAL=(model.ROOT/'orig/SLUS_216.68').is_file()

class RoadGuards(unittest.TestCase):
    def trace(self,code=()):
        raw=bytearray(model.INTERVALS[-3][1]-0xFF000)
        for i,w in enumerate(code):
            struct.pack_into('<I',raw,model.ENTRIES[0]-0xFF000+i*4,w)
        return model.RoadTrace(bytes(raw))

    def test_complete_entries_and_excluded_negate(self):
        t=self.trace()
        for p in (model.ENTRIES[0]-4,model.ENTRIES[0]+1,0x1CC7B4,0x3747E0,0x37AFD4):
            with self.assertRaises(ValueError):t.fetch(p)
        with self.assertRaises(ValueError):t.run(model.ENTRIES[1]+4)
        with self.assertRaises(ValueError):model.RoadTrace(b'').fetch(model.ENTRIES[0])
        t.frames=[0]*16
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0])
        t.instruction_count=150000
        with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,model.ENTRIES[0])

    def test_memory_alignment_and_unwritten_output(self):
        t=self.trace()
        for a,n in ((model.BUFFER+1,4),(model.END,4),(model.BUFFER,3),(model.FAR_RECORD+0x34,1)):
            with self.assertRaises(ValueError):t.load(a,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(0x7FF10,4)
        t.save(0x7FF10,0xABCD0000,4)
        self.assertEqual(t.load(0x7FF10,4),0xABCD0000)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(0x7FF14,1)

    def test_control_delay_rejection_independent_of_branch_outcome(self):
        for delay in ((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(4<<21)|(1<<16)|1,
                      (17<<26)|(8<<21)|1,3<<26,0x03E00008):
            t=self.trace((0x03E00008,delay));t.r[4],t.r[5],t.r[31]=1,2,RETURN
            with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[0])
        t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,0x03E00008));t.r[4],t.r[5]=1,2
        with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[0])
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[0])

    def test_actual_signed_and_unsigned_division_traps(self):
        t=self.trace();pc=model.ENTRIES[1]
        for fn in (0x1A,0x1B):
            t.r[4],t.r[5]=1,0
            with self.assertRaises(ValueError):t.execute((4<<21)|(5<<16)|fn,pc)
        t.r[4],t.r[5]=0x80000000,0xFFFFFFFF
        with self.assertRaises(ValueError):t.execute((4<<21)|(5<<16)|0x1A,pc)
        for w in (0x000001CD,0x70000000,(4<<21)|(5<<16)|(1<<6)|6):
            with self.assertRaises(ValueError):t.execute(w,pc)
        with self.assertRaisesRegex(ValueError,'complete original'):t.library_call(0x374748)

    def test_regimm_annul_and_raw_mfc1_width(self):
        t=self.trace();pc=model.ENTRIES[0]
        t.r[4]=0xFFFFFFFF
        self.assertEqual(t.execute((1<<26)|(4<<21)|(3<<16)|1,pc),(None,True))
        t.r[4]=0
        self.assertEqual(t.execute((1<<26)|(4<<21)|(3<<16)|1,pc),(pc+8,False))
        t.f[12]=-0.0
        t.execute((17<<26)|(2<<16)|(12<<11),pc)
        self.assertEqual(t.r[2],0x80000000)
        with self.assertRaises(ValueError):t.execute((17<<26)|(2<<16)|(12<<11)|1,pc)

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_no_candidate_outputs_and_optional_mode(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        # Execute both complete original fptoui/unpack bodies, not a host cast.
        # Nonfinite inputs remain rejected by the shared observer's FP stores.
        for raw,result in ((0x80000000,0),(0xC2200000,0),(0x00000001,0),
                           (0x80000001,0),(0x421F999A,39),(0x4EFFFFFF,0x7FFFFF80),
                           (0x4F000000,0x80000000),(0x4F7FFFFF,0xFFFFFF00),
                           (0x4F800000,0xFFFFFFFF)):
            t=model.RoadTrace(original);t.r[29],t.r[31]=0x80000,RETURN
            t.f[12]=model.scalar(raw);t.run(0x374748)
            self.assertEqual(t.r[2]&0xFFFFFFFF,result)
        for raw in (0x7FC12345,0x7F800000,0xFF800000):
            t=model.RoadTrace(original);t.r[29],t.r[31]=0x80000,RETURN
            t.f[12]=model.scalar(raw)
            with self.assertRaisesRegex(ValueError,'nonfinite'):t.run(0x374748)
        t=model.scene(original,routine=1,count=65537,run=False)
        for a in range(model.OUTPUT+4,model.OUTPUT+12):t.memory.pop(a)
        t.run(model.ENTRIES[1])
        self.assertTrue(all(a not in t.memory for a in range(model.OUTPUT+4,model.OUTPUT+12)))
        c=model.scene(original,routine=1,mode_output=0)
        self.assertEqual(c['expected'][(model.OUTPUT+8-model.BUFFER)//4],0x12345678)
        self.assertNotIn(0x1CE868,c['visited'])
        self.assertEqual(c['result'],word(-8.0))
        for threshold in (-1.0,0.0,1.0):
            c=model.scene(original,routine=1,threshold=threshold,shape=3)
            self.assertEqual(c['result'],word(min(threshold,0.0)))

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_full_word_mode_precedes_byte_alias(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        for shift in range(4):
            c=model.scene(original,routine=1,vertex_output=model.OUTPUT+8+shift)
            self.assertEqual(c['expected'][(model.OUTPUT+8-model.BUFFER)//4],1&~(255<<(8*shift)))
            self.assertEqual(c['stores'][-6:],[model.OUTPUT+8,4,1,model.OUTPUT+8+shift,1,0])

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_unsigned_pointer_signed_subquery_and_byte_offset(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        c=model.scene(original,numbers=(65535,))
        self.assertEqual(c['result'],model.FAR_RECORD)
        self.assertEqual(c['expected'][(model.OUTPUT+8-model.BUFFER)//4],1)
        for lazy in (False,True):
            c=model.scene(original,point=(4,0,0),cell_count=2,numbers=(0,1),lazy=lazy,shift_records=True)
            self.assertEqual(c['result'],model.RECORDS+0x34)
            self.assertEqual(c['invocations'][1],2)

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_empty_cell_publication_and_cached_null_resolver(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        c=model.scene(original,cell_count=0)
        self.assertEqual(c['result'],0)
        self.assertEqual(c['expected'][(model.OUTPUT-model.BUFFER)//4],model.ROADS)
        self.assertEqual(c['expected'][(model.OUTPUT+8-model.BUFFER)//4],0x12345678)
        c=model.scene(original,registry=False,neighbors=(0x50000,0x50001,0x60000))
        self.assertEqual(c['invocations'][3],2)
        self.assertEqual(c['result'],0)
        self.assertEqual(c['expected'][(model.OUTPUT-model.BUFFER)//4],0xCCCCCCCC)

if __name__=='__main__':unittest.main()
