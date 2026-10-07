"""Owned code, actual helper returns, 64-bit thresholds and delay guards."""
from pathlib import Path
import math
import struct
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_segment_intersection as model
from trace_geometry import RETURN, word


class IntersectionGuards(unittest.TestCase):
    def trace(self,instructions=(),routine=0,helper=()):
        image=bytearray(model.NORMALIZE_END-0xFF000)
        for base,words in ((model.ENTRIES[routine],instructions),(model.NORMALIZE,helper)):
            for i,w in enumerate(words):struct.pack_into('<I',image,base-0xFF000+i*4,w)
        return model.IntersectionTrace(bytes(image))

    def test_owned_entry_full_code_and_initialized_aligned_memory(self):
        t=self.trace()
        for a in (*model.ENTRIES,model.NORMALIZE):self.assertEqual(t.fetch(a),0)
        for a in (model.ENTRIES[0]-4,model.ENTRIES[0]+1,model.RANGES[1][1],model.NORMALIZE_END):
            with self.assertRaises(ValueError):t.fetch(a)
        with self.assertRaises(ValueError):t.run(model.NORMALIZE)
        with self.assertRaises(ValueError):model.IntersectionTrace(b'').fetch(model.ENTRIES[0])
        for a in range(model.BUFFER,model.BUFFER+16):t.memory[a]=0
        for size in (4,8,16):t.save(model.BUFFER,123,size);self.assertEqual(t.load(model.BUFFER,size),123)
        for a,size in ((model.BUFFER+1,4),(model.BUFFER_END,4),(model.BUFFER,3),(model.BUFFER+4,8),(model.BUFFER+16,4)):
            with self.assertRaises(ValueError):t.load(a,size)
            with self.assertRaises(ValueError):t.save(a,0,size)

    def test_unknown_opcodes_calls_reserved_and_cop1_operand_forms(self):
        t=self.trace();cop=(17<<26)|(16<<21)
        words=[2<<26,18<<26,35<<26,(1<<26)|(3<<16),(15<<26)|(1<<21),
               (1<<6)|0x2D,(1<<21)|0x38,(31<<21)|(1<<16)|8,
               (17<<26)|(4<<21)|1,cop|(1<<16)|6,cop|(1<<16)|7,
               cop|(1<<6)|0x36,cop|(1<<11)|4,(17<<26)|(8<<21)|(4<<16)]
        for w in words:
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,model.ENTRIES[0])
        with self.assertRaises(ValueError):t.library_call(model.NORMALIZE)
        with self.assertRaises(ValueError):t.record_call(0x374850)

    def test_daddu_dsll_full64_threshold_and_signed_regimm(self):
        t=self.trace();t.r[2]=0xBFF0000000000000
        t.execute((2<<21)|(16<<11)|0x2D,0);self.assertEqual(t.r[16],t.r[2])
        t.r[5]=0xFB93
        t.execute((5<<16)|(5<<11)|(16<<6)|0x38,0)
        t.execute((13<<26)|(5<<21)|(5<<16)|0xE2D6,0)
        t.execute((5<<16)|(5<<11)|(30<<6)|0x38,0)
        self.assertEqual(t.r[5],0x3EE4F8B580000000)
        for kind in (0,1,2):
            for value in (model.MASK64,0,1):
                t.r[2]=value;w=(1<<26)|(2<<21)|(kind<<16)|1
                taken=value!=model.MASK64 if kind==1 else value==model.MASK64
                self.assertEqual(t.execute(w,model.ENTRIES[0]),(model.ENTRIES[0]+8,False) if taken else (None,kind==2))

    def test_soft_normal_zero_domain_and_signed_zero(self):
        t=self.trace();t.f[12]=-0.0;t.library_call(0x374848)
        self.assertEqual(t.r[2],0x8000000000000000)
        t.r[4],t.r[5]=t.r[2],0;t.library_call(0x373250);self.assertEqual(t.r[2],0)
        t.r[4],t.r[5]=0,model.double_bits(-1.5);t.library_call(0x372CC0)
        self.assertEqual(t.r[2],model.double_bits(1.5))
        for value in (math.inf,math.nan,struct.unpack('<f',struct.pack('<I',1))[0]):
            with self.assertRaises(ValueError):model.normal_single(value)
        for value in (0x7FF0000000000000,1):
            with self.assertRaises(ValueError):model.double_value(value)

    def test_scalar_division_sqrt_and_load_domains(self):
        t=self.trace();cop=(17<<26)|(16<<21);t.f[1],t.f[2]=1,0
        with self.assertRaisesRegex(ValueError,'zero denominator'):t.execute(cop|(2<<16)|(1<<11)|3,0)
        t.f[2]=-1
        with self.assertRaisesRegex(ValueError,'negative square root'):t.execute(cop|(2<<16)|4,0)
        for value in (math.inf,math.nan):
            t.f[1]=value
            with self.assertRaises(ValueError):t.execute(cop|(1<<11)|6,0)
        for a in range(model.BUFFER,model.BUFFER+4):t.memory[a]=0
        t.r[4]=model.BUFFER
        for b in (0x7F800000,1):
            t.save(model.BUFFER,b,4)
            with self.assertRaises(ValueError):t.execute((49<<26)|(4<<21),0)

    def test_ordinary_delays_reject_encoded_control_on_both_outcomes(self):
        outer=(4<<26)|(4<<21)|(5<<16)|1
        for equal in (False,True):
            for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,(18<<26)|(8<<21)|1):
                t=self.trace((outer,delay,0x03E00008,0));t.r[4],t.r[5],t.r[31]=1,1 if equal else 2,RETURN
                with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[0])
                self.assertEqual(t.instruction_count,1)
        t=self.trace((outer,(9<<26)|(2<<16)|7,0x03E00008,0));t.r[4],t.r[5],t.r[31]=1,2,RETURN
        t.run(model.ENTRIES[0]);self.assertEqual(t.r[2],7);self.assertEqual(t.instruction_count,4)

    def test_beql_and_bltzl_annul_unsupported_delay_on_untaken_branch(self):
        for outer in ((20<<26)|(4<<21)|1,(1<<26)|(2<<21)|(2<<16)|1):
            t=self.trace((outer,(4<<26)|1,0x03E00008,0));t.r[4],t.r[2],t.r[31]=1,0,RETURN
            t.run(model.ENTRIES[0]);self.assertEqual(t.instruction_count,3)
            t=self.trace((outer,(4<<26)|1,0x03E00008,0));t.r[4],t.r[2],t.r[31]=0,model.MASK64,RETURN
            with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(model.ENTRIES[0])

    def test_actual_helper_must_return_to_captured_invocation(self):
        # A real nested helper invocation executes owned instructions and must
        # return by JR31 to the exact call continuation, not directly outside.
        jal=(3<<26)|(model.NORMALIZE>>2)
        save=(31<<21)|(16<<11)|0x2D;restore=(16<<21)|(31<<11)|0x2D
        t=self.trace((save,jal,0,restore,0x03E00008,0),helper=(0x03E00008,0))
        for a in range(model.BUFFER,model.BUFFER+12):t.memory[a]=0
        t.r[4],t.r[31]=model.BUFFER,RETURN;t.run(model.ENTRIES[0]);self.assertEqual(t.calls[3],1)
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=model.ENTRIES[0]+8
        with self.assertRaisesRegex(ValueError,'selected stop'):t.run(model.ENTRIES[0])

    def test_instruction_branch_and_delay_boundaries(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        end=model.RANGES[0][1]
        t=self.trace(((4<<26)|((end-model.ENTRIES[0]-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(model.ENTRIES[0])
        t=self.trace(((4<<26)|((end-model.ENTRIES[0]-8)//4),0));image=bytearray(t.original)
        struct.pack_into('<I',image,end-4-0xFF000,0x03E00008);t=model.IntersectionTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(model.ENTRIES[0])


if __name__=='__main__':unittest.main()
