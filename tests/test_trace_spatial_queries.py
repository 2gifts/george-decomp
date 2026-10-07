"""Strict scope, real helpers, scratch initialization and signed edge regressions."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_spatial_queries as model
from analyze import validated_elf
from trace_geometry import RETURN, word

LOCAL_ORIGINAL=(model.ROOT/'orig/SLUS_216.68').is_file()


class SpatialGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(model.HELPERS[-1][1]-0xFF000)
        for i,w in enumerate(instructions):
            struct.pack_into('<I',image,model.ENTRIES[1]-0xFF000+i*4,w)
        return model.SpatialTrace(bytes(image))

    def test_complete_entry_memory_alignment_and_execution_bounds(self):
        t=self.trace()
        for pc in (model.ENTRIES[0]-4,model.ENTRIES[0]+1,model.RANGES[1][1],model.HELPERS[-1][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):model.SpatialTrace(b'').fetch(model.ENTRIES[0])
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0]+4)
        for a,n in ((model.BUFFER+1,4),(model.END,4),(model.GLOBAL+4,4),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(a,n)
        t.instruction_count=3000
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])

    def test_unknown_scratch_reads_require_actual_preceding_store(self):
        t=self.trace()
        for a in (model.BUFFER,0x7FF34):
            with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(a,4)
            t.save(a,word(3),4)
            self.assertEqual(t.load(a,4),word(3))
            with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(a+4,4)
        t.save(0x7FF30,123,16)
        self.assertEqual(t.load(0x7FF30,16),123)

    def test_signed_division_compare_blez_and_actual_break_guards(self):
        t=self.trace()
        divide=(4<<21)|(5<<16)|0x1A
        t.r[4],t.r[5]=-7 & 0xFFFFFFFF,3
        t.execute(divide,model.ENTRIES[0])
        self.assertEqual((model.signed(t.lo),model.signed(t.hi)),(-2,-1))
        t.execute((4<<21)|(5<<16)|(2<<11)|0x2A,model.ENTRIES[0])
        self.assertEqual(t.r[2],1)
        for a,b in ((1,0),(0x80000000,0xFFFFFFFF)):
            t.r[4],t.r[5]=a,b
            with self.assertRaisesRegex(ValueError,'division'):t.execute(divide,model.ENTRIES[0])
        for w in (divide|(1<<11),divide|64,(6<<26)|(4<<21)|(1<<16),0x000001CD):
            with self.assertRaises(ValueError):t.execute(w,model.ENTRIES[0])
        t.r[4]=0
        self.assertEqual(t.execute((6<<26)|(4<<21)|1,model.ENTRIES[0]),(model.ENTRIES[0]+8,False))
        t.r[4]=1
        self.assertEqual(t.execute((6<<26)|(4<<21)|1,model.ENTRIES[0]),(None,False))

    def test_delay_control_encoding_and_actual_jr31_only(self):
        for delay in ((4<<26)|(4<<21)|(5<<16)|1,
                      (1<<26)|(4<<21)|(1<<16)|1,
                      (17<<26)|(8<<21)|1,(3<<26),0x03E00008):
            t=self.trace((0x03E00008,delay))
            t.r[4],t.r[5],t.r[31]=1,2,RETURN
            with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[1])
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[1])
        with self.assertRaises(ValueError):t.library_call(model.HELPERS[0][0])

    def test_finite_vu_lanes_and_reserved_instruction_forms(self):
        t=self.trace()
        t.vf[4],t.vf[5]=[1,2,3,math.inf],[1,0,0,1]
        with self.assertRaises(ValueError):t.execute(0x4BE521BC,model.HELPERS[1][0])
        for w in (0x4BC521BC,0x48A22801,0x70000000,0x0022183E):
            with self.assertRaises(ValueError):t.execute(w,model.HELPERS[1][0])

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_first_zero_rejects_but_later_zero_reuses_initialized_y(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        for routine in (2,3):
            t=model.prepare(original,routine=routine,polygon_count=0)
            with self.assertRaisesRegex(ValueError,'uninitialized'):t.run(model.ENTRIES[routine])
        for y,expected in ((0,model.RECORDS+0x60),(1.9999,model.RECORDS+0x60),
                           (2,0),(-2,0),(-1.9999,model.RECORDS+0x60)):
            t=model.prepare(original,routine=2,record_count=2,later_zero=True,point=(3,y,0))
            t.run(model.ENTRIES[2])
            self.assertEqual(t.r[2],expected)
            self.assertEqual(t.invocations[-1],4)
        for y,expected in ((2.9999,model.RECORDS+0x60),(-0.9999,model.RECORDS+0x60),
                           (3,0),(-1,0)):
            t=model.prepare(original,routine=2,record_count=2,later_zero=True,
                            shape=3,point=(3,y,0))
            t.run(model.ENTRIES[2]);self.assertEqual(t.r[2],expected)
            self.assertEqual(t.invocations[-1],4)
        # No fabricated initialization: Y was actually written by four real
        # transform invocations on the earlier rejected record.

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_sixth_vertex_rejects_saved_stack_overlap(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        for routine in (2,3):
            t=model.prepare(original,routine=routine,polygon_count=6)
            with self.assertRaisesRegex(ValueError,'saved-stack overlap'):t.run(model.ENTRIES[routine])
            self.assertEqual(t.invocations[-1],5)

    @unittest.skipUnless(LOCAL_ORIGINAL,'requires locally supplied original ELF')
    def test_original_tiny_edge_retains_side_and_exact_threshold_is_not_skipped(self):
        _,original=validated_elf(model.ROOT/'orig/SLUS_216.68')
        class Observed(model.SpatialTrace):
            def execute(self,w,pc):
                result=super().execute(w,pc)
                if pc==0x1CF6D0 and result[0] is not None:
                    self.skipped_side.append(word(self.f[6]))
                return result
        t=model.prepare(original,routine=3,polygon_count=5)
        t.__class__=Observed;t.skipped_side=[]
        vertices=((-1,0,-1),(1,0,-1),(1,0,1),(0.975,0,1.025),(-1,0,1))
        for i,vertex in enumerate(vertices):
            for j,value in enumerate(vertex):t.save(model.BLOBS+8+i*12+j*4,word(value),4)
        t.run(model.ENTRIES[3])
        self.assertEqual(t.r[2],1)
        self.assertEqual(t.skipped_side,[word(-2.0)])
        for x,expected in ((0.09999999403953552,1),(0.10000000149011612,0)):
            t=model.prepare(original,routine=3,polygon_count=2,point=(0,0,1))
            for j,value in enumerate((0,0,0,x,0,0)):t.save(model.BLOBS+8+j*4,word(value),4)
            t.run(model.ENTRIES[3]);self.assertEqual(t.r[2],expected)


if __name__=='__main__':
    unittest.main()
