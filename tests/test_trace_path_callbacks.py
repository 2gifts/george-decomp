import struct,sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_path_callbacks as model
from trace_geometry import RETURN,scalar,word

class CallbackGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(max(b for a,b in model.RANGES+model.HELPERS)-0xFF000)
        for i,w in enumerate(instructions):struct.pack_into('<I',image,model.ENTRIES[0]-0xFF000+4*i,w)
        return model.CallbackTrace(bytes(image))

    def test_scoped_code_aligned_initialized_memory(self):
        t=self.trace()
        for a,b in model.RANGES+model.HELPERS:
            self.assertIsInstance(t.fetch(a),int)
            with self.assertRaises(ValueError):t.fetch(a+1)
        for address in (0x2C03EC,0x2A2784):
            with self.assertRaises(ValueError):t.run(address)
        with self.assertRaises(ValueError):t.fetch(0x2C19F0)
        with self.assertRaises(ValueError):t.load(model.BUFFER,4)
        for i in range(16):t.memory[model.BUFFER+i]=0
        t.save(model.BUFFER,123,4);self.assertEqual(t.load(model.BUFFER,4),123)
        for a,n in ((model.BUFFER+1,4),(model.END,4),(model.BUFFER-4,4),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(a,n)
            with self.assertRaises(ValueError):t.save(a,0,n)

    def test_signed_slt_conditional_moves_blez(self):
        t=self.trace();t.r[4]=0xFFFFFFFF;t.r[5]=0
        t.execute((4<<21)|(5<<16)|(2<<11)|42,0);self.assertEqual(t.r[2],1)
        for fn,condition,taken in ((10,0,True),(10,1,False),(11,0,False),(11,1,True)):
            t.r[2]=99;t.r[4]=123;t.r[5]=condition
            t.execute((4<<21)|(5<<16)|(2<<11)|fn,0);self.assertEqual(t.r[2],123 if taken else 99)
        t.r[4]=0x80000000
        self.assertEqual(t.execute((6<<26)|(4<<21)|2,0),(12,False))
        t.r[4]=1;self.assertEqual(t.execute((6<<26)|(4<<21)|2,0),(None,False))
        for w in ((4<<21)|(2<<11)|(1<<6)|42,(6<<26)|(1<<16)):
            with self.assertRaises(ValueError):t.execute(w,0)

    def test_word_to_float_and_finite_scalar_operand_guards(self):
        t=self.trace();t.f[1]=scalar(0x80000000)
        w=(17<<26)|(20<<21)|(1<<11)|(2<<6)|32
        t.execute(w,0);self.assertEqual(word(t.f[2]),word(-2147483648.0))
        for w in (w|(1<<16),w^1,(17<<26)|(16<<21)|(1<<16)|6,(17<<26)|(4<<21)|1,(17<<26)|(8<<21)|(4<<16)):
            with self.assertRaises(ValueError):t.execute(w,0)
        t.f[1]=1;t.f[2]=0
        with self.assertRaisesRegex(ValueError,'zero denominator'):t.execute((17<<26)|(16<<21)|(2<<16)|(1<<11)|3,0)
        t.f[1]=float('inf')
        with self.assertRaisesRegex(ValueError,'finite'):t.execute((17<<26)|(16<<21)|(1<<11)|6,0)

    def test_ordinary_likely_delays_and_transfer_guards(self):
        branch=(17<<26)|(8<<21)|2
        t=self.trace((branch,0x70000000,0x03E00008,0));t.r[31]=RETURN;t.condition=True
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0])
        t=self.trace((branch|(2<<16),0x70000000,0x03E00008,0));t.r[31]=RETURN;t.condition=True
        t.run(model.ENTRIES[0]);self.assertEqual(t.instruction_count,3)
        t=self.trace(((4<<26)|2,(4<<26)|2,0,0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(model.ENTRIES[0])
        # Reject by encoding even when the inner ordinary branch is untaken.
        # Executing it first would otherwise return (None, False), hiding the
        # forbidden delay-slot control transfer in the old runner.
        untaken=((4<<26)|(4<<21)|(5<<16)|2,
                 (1<<26)|(6<<21)|(1<<16)|2,
                 (17<<26)|(8<<21)|2)
        for condition in (True,False):
            for delay in untaken:
                t=self.trace((branch,delay,0x03E00008,0,0x03E00008,0))
                t.r[4],t.r[5],t.r[6]=0,1,0xFFFFFFFF
                t.r[31],t.condition=RETURN,condition
                with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(model.ENTRIES[0])
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[0])

    def test_unknown_calls_and_retained_call_link(self):
        t=self.trace(((3<<26)|(0x123450>>2),0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unknown controlled'):t.run(model.ENTRIES[0])
        t=self.trace(((4<<21)|(31<<11)|9,0));t.r[4]=0xF000FFFF;t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unknown controlled'):t.run(model.ENTRIES[0])
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'unknown controlled'):t.library_call(0x37B238)

    def test_cross_body_delays_and_instruction_bound(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        a,b=model.RANGES[0]
        t=self.trace(((4<<26)|((b-a-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(a)
        image=bytearray(t.original);struct.pack_into('<I',image,a-0xFF000,(4<<26)|((b-a-8)//4));struct.pack_into('<I',image,b-4-0xFF000,0x03E00008)
        t=model.CallbackTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(a)

    def test_signed_packed_loads_shifts_or_and_regimm(self):
        t=self.trace()
        t.memory[model.FIRST]=128;t.r[4]=model.FIRST
        t.execute((0x20<<26)|(4<<21)|(2<<16),0);self.assertEqual(t.r[2],0xFFFFFF80)
        t.execute((2<<16)|(3<<11)|(4<<6)|3,0);self.assertEqual(t.r[3],0xFFFFFFF8)
        t.r[4]=0x1234;t.r[5]=0xFFFF0000
        t.execute((4<<21)|(5<<16)|(2<<11)|0x25,0);self.assertEqual(t.r[2],0xFFFF1234)
        for value,target in ((0,12),(1,12),(0x7FFFFFFF,12),(0x80000000,None),(0xFFFFFFFF,None)):
            t.r[4]=value;self.assertEqual(t.execute((1<<26)|(4<<21)|(1<<16)|2,0),(target,False))
        for w in ((1<<26)|(4<<21)|(2<<16),(4<<21)|(5<<16)|(2<<11)|(1<<6)|0x25,(1<<21)|(2<<16)|(3<<11)|3):
            with self.assertRaises(ValueError):t.execute(w,0)

    def test_minimum_bits_and_integer_transfers(self):
        t=self.trace();w=(17<<26)|(16<<21)|(2<<16)|(1<<11)|(3<<6)|0x29
        for a,b,expected in ((0x80000000,0,0x80000000),(0,0x80000000,0x80000000),(0xBF800000,0xC0000000,0xC0000000),(0x3F800000,0x40000000,0x3F800000)):
            t.f[1]=scalar(a);t.f[2]=scalar(b);t.execute(w,0);self.assertEqual(word(t.f[3]),expected)
        t.f[1]=float('nan')
        with self.assertRaisesRegex(ValueError,'finite'):t.execute(w,0)
        t.r[4]=0xFFFFFF80
        t.execute((17<<26)|(4<<21)|(4<<16)|(1<<11),0)
        with self.assertRaisesRegex(ValueError,'finite'):t.execute((17<<26)|(16<<21)|(1<<11)|6,0)
        t.execute((17<<26)|(20<<21)|(1<<11)|(2<<6)|32,0)
        self.assertEqual(word(t.f[2]),word(-128.0))
        with self.assertRaises(ValueError):t.execute((17<<26)|(4<<21)|(4<<16)|(1<<11)|1,0)

if __name__=='__main__':unittest.main()
