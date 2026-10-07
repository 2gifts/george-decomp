"""Synthetic scope, operand, finite soft ABI and delay guards; no game required."""
from pathlib import Path
import math
import struct
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_plane_intersection as model
from trace_geometry import RETURN


class PlaneGuards(unittest.TestCase):
    def trace(self,instructions=(),routine=0):
        image=bytearray(model.RANGES[-1][1]-0xFF000)
        for i,w in enumerate(instructions):
            struct.pack_into('<I',image,model.ENTRIES[routine]-0xFF000+i*4,w)
        return model.PlaneTrace(bytes(image))

    def test_owned_code_and_initialized_aligned_memory(self):
        t=self.trace()
        for a in model.ENTRIES:self.assertEqual(t.fetch(a),0)
        for a in (model.ENTRIES[0]-4,model.ENTRIES[0]+1,*[end for _,end in model.RANGES]):
            with self.assertRaises(ValueError):t.fetch(a)
        with self.assertRaises(ValueError):t.run(model.CALLS[0])
        with self.assertRaises(ValueError):model.PlaneTrace(b'').fetch(model.ENTRIES[0])
        for a in range(model.BUFFER,model.BUFFER+16):t.memory[a]=0
        for size in (4,8,16):t.save(model.BUFFER,123,size);self.assertEqual(t.load(model.BUFFER,size),123)
        for a,size in ((model.BUFFER+1,4),(model.BUFFER_END,4),(model.BUFFER,3),(model.BUFFER+4,8),(model.BUFFER+16,4)):
            with self.assertRaises(ValueError):t.load(a,size)
            with self.assertRaises(ValueError):t.save(a,0,size)

    def test_unknown_calls_reserved_and_cop1_operands(self):
        t=self.trace();cop=(17<<26)|(16<<21)
        words=[2<<26,18<<26,35<<26,20<<26,(1<<26)|(2<<16),(15<<26)|(1<<21),
               (1<<6)|0x2D,(1<<21)|0x38,(31<<21)|(1<<16)|8,
               (17<<26)|(4<<21)|1,cop|(1<<16)|6,cop|(1<<16)|7,
               cop|(1<<6)|0x36,cop|4,(17<<26)|(8<<21)|(2<<16)]
        for w in words:
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,model.ENTRIES[0])
        with self.assertRaises(ValueError):t.library_call(0x3936A4)
        with self.assertRaises(ValueError):t.record_call(0x374850)

    def test_full64_moves_threshold_signed_branches_and_slti(self):
        t=self.trace();t.r[2]=0xBFF0000000000000
        t.execute((2<<21)|(16<<11)|0x2D,0);self.assertEqual(t.r[16],t.r[2])
        t.r[5]=0xFC68
        for shift,low in ((16,0xDB8B),(16,0x8000),(14,None)):
            t.execute((5<<16)|(5<<11)|(shift<<6)|0x38,0)
            if low is not None:t.execute((13<<26)|(5<<21)|(5<<16)|low,0)
        self.assertEqual(t.r[5],0x3F1A36E2E0000000)
        for kind in (0,1):
            for value in (model.MASK64,0,1):
                t.r[2]=value;w=(1<<26)|(2<<21)|(kind<<16)|1
                taken=value!=model.MASK64 if kind==1 else value==model.MASK64
                self.assertEqual(t.execute(w,model.ENTRIES[0]),(model.ENTRIES[0]+8,False) if taken else (None,False))
        for value,imm,expected in ((model.MASK64,0,1),(2,3,1),(3,3,0),(0,0xFFFF,0)):
            t.r[3]=value;t.execute((10<<26)|(3<<21)|(2<<16)|imm,0);self.assertEqual(t.r[2],expected)

    def test_zero_fill_exact_domain_and_effects(self):
        t=self.trace()
        for a in range(0x80000,0x80010):t.memory[a]=0xAA
        t.r[4],t.r[5],t.r[6]=0x80000,0,12;t.library_call(0x3936A0)
        self.assertEqual(t.r[2],0x80000);self.assertEqual(t.load(0x80000,8),0)
        self.assertEqual(t.load(0x80008,4),0);self.assertEqual(t.load(0x8000C,4),0xAAAAAAAA)
        self.assertEqual(t.events,[3,0,12,0,0,0,0,0,0,0,0,0])
        for address,fill,count in ((model.BUFFER,0,12),(0x80001,0,12),(0x80000,1,12),(0x80000,0,8)):
            t.r[4],t.r[5],t.r[6]=address,fill,count
            with self.assertRaises(ValueError):t.library_call(0x3936A0)

    def test_soft_signed_zero_and_finite_domains(self):
        t=self.trace();t.f[12]=-0.0;t.library_call(0x374848)
        self.assertEqual(t.r[2],0x8000000000000000)
        t.r[4],t.r[5]=t.r[2],0;t.library_call(0x373250);self.assertEqual(t.r[2],0)
        t.r[4],t.r[5]=0,model.double_bits(-1.5);t.library_call(0x372CC0)
        self.assertEqual(t.r[2],model.double_bits(1.5))
        for value in (math.inf,math.nan,struct.unpack('<f',struct.pack('<I',1))[0]):
            with self.assertRaises(ValueError):model.normal_single(value)
        for value in (0x7FF0000000000000,1):
            with self.assertRaises(ValueError):model.double_value(value)

    def test_division_and_scalar_load_domains(self):
        t=self.trace();cop=(17<<26)|(16<<21);t.f[1],t.f[2]=1,0
        with self.assertRaisesRegex(ValueError,'zero denominator'):t.execute(cop|(2<<16)|(1<<11)|3,0)
        for value in (math.inf,math.nan):
            t.f[1]=value
            with self.assertRaises(ValueError):t.execute(cop|(1<<11)|6,0)
        for a in range(model.BUFFER,model.BUFFER+4):t.memory[a]=0
        t.r[4]=model.BUFFER
        for bits in (0x7F800000,1):
            t.save(model.BUFFER,bits,4)
            with self.assertRaises(ValueError):t.execute((49<<26)|(4<<21),0)

    def test_both_ordinary_delay_outcomes_reject_encoded_control(self):
        branches=((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(2<<21)|(1<<16)|1,
                  (17<<26)|(8<<21)|(1<<16)|1)
        for outer in branches:
            for taken in (False,True):
                for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,(18<<26)|(8<<21)|1):
                    t=self.trace((outer,delay,0x03E00008,0));t.r[4],t.r[5],t.r[31]=1,1 if taken else 2,RETURN
                    t.r[2]=0 if taken else model.MASK64;t.condition=taken
                    with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[0])
                    self.assertEqual(t.instruction_count,1)
        t=self.trace((branches[0],(9<<26)|(2<<16)|7,0x03E00008,0));t.r[4],t.r[5],t.r[31]=1,2,RETURN
        t.run(model.ENTRIES[0]);self.assertEqual(t.r[2],7);self.assertEqual(t.instruction_count,4)

    def test_return_requires_actual_jr31_and_code_bounds(self):
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=model.ENTRIES[0]+8
        with self.assertRaisesRegex(ValueError,'selected stop'):t.run(model.ENTRIES[0])
        end=model.RANGES[0][1]
        t=self.trace(((4<<26)|((end-model.ENTRIES[0]-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(model.ENTRIES[0])
        t=self.trace(((4<<26)|((end-model.ENTRIES[0]-8)//4),0));image=bytearray(t.original)
        struct.pack_into('<I',image,end-4-0xFF000,0x03E00008);t=model.PlaneTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(model.ENTRIES[0])

    def test_instruction_bound_and_fixture_layouts(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        for routine,offsets in ((2,(4,12,36,44)),(0,(4,12,36)),(0,(4,12,36,61)),(0,(-1,12,36,44))):
            with self.assertRaises(ValueError):model.make_fixture(b'',routine,offsets,())
        for values in ((),((1,2,3),),((1,2,3),(1,2,3)),((1,2,3,4),(1,2,3,4),(1,2,3,4))):
            with self.assertRaises(ValueError):model.make_fixture(b'',0,(4,12,36,44),values)


if __name__=='__main__':unittest.main()
