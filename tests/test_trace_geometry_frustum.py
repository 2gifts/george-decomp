"""Strict six-face observer guards; synthetic tests need no private game file."""
from pathlib import Path
import math
import struct
import sys
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import trace_geometry_frustum as model
from trace_geometry import RETURN


class FrustumGuards(unittest.TestCase):
    def trace(self, instructions=(), helper=()):
        image=bytearray(model.END-0xFF000)
        for entry,words in ((model.ENTRY,instructions),(model.NORMALIZE,helper)):
            for i,w in enumerate(words):struct.pack_into('<I',image,entry-0xFF000+i*4,w)
        return model.FrustumTrace(bytes(image))

    def test_scope_and_initialized_aligned_memory(self):
        t=self.trace()
        for start,end in model.RANGES:
            self.assertEqual(t.fetch(start),0)
            for pc in (start-4,start+1,end):
                with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):model.FrustumTrace(b'').fetch(model.ENTRY)
        with self.assertRaises(ValueError):t.run(model.NORMALIZE)
        for a in range(model.BUFFER,model.BUFFER+16):t.memory[a]=0
        for size in (4,8,16):t.save(model.BUFFER,123,size);self.assertEqual(t.load(model.BUFFER,size),123)
        for address,size in ((model.BUFFER+1,4),(model.BUFFER+4,8),(model.BUFFER+16,4),
                             (model.BUFFER+model.WORDS*4,4),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,0,size)

    def test_reject_unknown_reserved_and_cop1_operand_forms(self):
        t=self.trace();cop=(17<<26)|(16<<21)
        for w in (2<<26,18<<26,1<<26,20<<26,(15<<26)|(1<<21),
                  0x21,(1<<6)|0x2D,(31<<21)|(1<<16)|8,(17<<26)|(4<<21)|1,
                  cop|(1<<16)|6,cop|(1<<16)|7,cop|(1<<11)|4,
                  cop|(1<<6)|0x32,cop|0x34,(17<<26)|(8<<21)|(4<<16)):
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,model.ENTRY)
        with self.assertRaises(ValueError):t.library_call(model.NORMALIZE)

    def test_scalar_finite_sqrt_source_and_division_domain(self):
        t=self.trace();cop=(17<<26)|(16<<21)
        t.f[2],t.f[3]=9,16
        t.execute(cop|(2<<16)|(3<<6)|4,0)
        self.assertEqual(t.f[3],3) # EE SQRT uses ft, including aliased destination.
        t.f[2]=-1
        with self.assertRaisesRegex(ValueError,'negative square'):t.execute(cop|(2<<16)|4,0)
        t.f[2],t.f[1]=0,1
        with self.assertRaisesRegex(ValueError,'zero denominator'):t.execute(cop|(2<<16)|(1<<11)|3,0)
        for value in (math.inf,math.nan,model.scalar(1)):
            with self.assertRaises(ValueError):model.normal_single(value)
        for a in range(model.BUFFER,model.BUFFER+4):t.memory[a]=0
        t.r[4]=model.BUFFER
        for w in (0x7F800000,0x7FC00001,1):
            t.save(model.BUFFER,w,4)
            with self.assertRaises(ValueError):t.execute((49<<26)|(4<<21),0)

    def test_saved_pointer_integer_word_load_and_store(self):
        t=self.trace()
        for a in range(model.BUFFER,model.BUFFER+16):t.memory[a]=0
        t.r[4],t.r[5]=model.BUFFER,0x12345678
        t.execute((0x2B<<26)|(4<<21)|(5<<16),0)
        t.execute((0x23<<26)|(4<<21)|(6<<16),0)
        self.assertEqual(t.r[6],0x12345678)
        t.r[5]=0xCAFEBABE12345678
        t.execute((5<<21)|(16<<11)|0x2D,0)
        self.assertEqual(t.r[16],t.r[5])
        t.execute((0x1F<<26)|(4<<21)|(16<<16),0)
        self.assertEqual(t.load(model.BUFFER,16),0xCAFEBABE12345678)

    def test_call_pointer_order_and_five_call_bound(self):
        t=self.trace();t.output=model.BUFFER
        for a in range(model.BUFFER,model.BUFFER+168):t.memory[a]=0
        for i in range(5):
            t.r[4]=model.BUFFER+i*28;t.record_call(model.NORMALIZE)
        self.assertEqual(t.calls,5);self.assertEqual(t.events[::4],[0,7,14,21,28])
        with self.assertRaises(ValueError):t.record_call(model.NORMALIZE)
        t=self.trace();t.r[4]=model.BUFFER+4
        with self.assertRaisesRegex(ValueError,'pointer/order'):t.record_call(model.NORMALIZE)
        with self.assertRaises(ValueError):t.record_call(model.NORMALIZE+4)

    def test_both_branch_outcomes_reject_encoded_delay_control(self):
        for outer in ((4<<26)|(4<<21)|(5<<16)|1,(17<<26)|(8<<21)|(1<<16)|1):
            for taken in (False,True):
                for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,(18<<26)|(8<<21)|1):
                    t=self.trace((outer,delay,0x03E00008,0));t.r[31]=RETURN
                    t.r[4],t.r[5],t.condition=1,1 if taken else 2,taken
                    with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRY)
                    self.assertEqual(t.instruction_count,1)
        t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,(9<<26)|(2<<16)|7,0x03E00008,0))
        t.r[31],t.r[4],t.r[5]=RETURN,1,2;t.run(model.ENTRY)
        self.assertEqual(t.r[2],7);self.assertEqual(t.instruction_count,4)

    def test_actual_jr_stop_transfer_delay_and_unknown_calls(self):
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRY)
        t=self.trace((0x03E00008,0));t.r[31]=model.ENTRY+8
        with self.assertRaisesRegex(ValueError,'selected stop'):t.run(model.ENTRY)
        t=self.trace(((4<<26)|((model.END-model.ENTRY-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(model.ENTRY)
        t=self.trace(((4<<26)|((model.END-model.ENTRY-8)//4),0));image=bytearray(t.original)
        struct.pack_into('<I',image,model.END-4-0xFF000,0x03E00008)
        t=model.FrustumTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(model.ENTRY)
        t=self.trace(((3<<26)|(0x1234>>2),0))
        with self.assertRaisesRegex(ValueError,'unreviewed frustum call'):t.run(model.ENTRY)

    def test_instruction_bound_and_fixture_contract_validation(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRY)
        for out,frame in ((-1,0),(87,0),(0,113),(True,0),(0,False),(0,1.5)):
            with self.assertRaises(ValueError):model.make_fixture(b'',out,frame,[0]*16,[0]*4)
        for frame,dimensions in (([0]*15,[0]*4),([0]*16,[0]*3)):
            with self.assertRaises(ValueError):model.make_fixture(b'',0,0,frame,dimensions)

    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'private original required for actual alias regression')
    def test_original_shifted_aliases_and_opposite_signed_zero(self):
        _,original=model.validated_elf(ROOT/'orig/SLUS_216.68')
        frame=(1,0,0,90,0,1,0,91,0,0,1,92,3,-2,5,93)
        for offset in (24,25,27,28,35,39,48,61,64):
            case=model.make_fixture(original,24,offset,frame,(1,4,2,3))
            self.assertEqual(case['events'][::4],[24,31,38,45,52])
            self.assertEqual(case['expected'][:24],case['initial'][:24])
            self.assertEqual(case['expected'][66:],case['initial'][66:])
            for i in range(3):self.assertEqual(case['expected'][24+35+i],case['expected'][24+28+i]^0x80000000)


if __name__=='__main__':unittest.main()
