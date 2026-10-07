import unittest,sys,struct
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_path_callbacks2 as model
from trace_geometry import RETURN,scalar,word

class AccumulatorGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(max(b for a,b in model.RANGES+model.HELPERS)-0xFF000)
        for i,w in enumerate(instructions):struct.pack_into('<I',image,model.ENTRIES[0]-0xFF000+4*i,w)
        return model.AccTrace(bytes(image))
    @staticmethod
    def acc(fn,fs=1,ft=2,fd=0):return (17<<26)|(16<<21)|(ft<<16)|(fs<<11)|(fd<<6)|fn
    def test_accumulator_order_and_final_destination(self):
        t=self.trace();t.f[1],t.f[2]=2.0,3.0
        t.execute(self.acc(26),0);self.assertEqual(t.accumulator,6.0)
        t.f[1],t.f[2]=0.25,0.5
        t.execute(self.acc(30),0);self.assertEqual(t.accumulator,6.125)
        t.execute(self.acc(28,fd=3),0)
        self.assertEqual(t.f[3],6.25);self.assertEqual(t.accumulator,6.125)
        # Rounding the product before addition gives zero; evaluating this
        # cancellation as a fused/double expression would leave 2**-46.
        t.f[1],t.f[2]=scalar(0xBF800002),1.0
        t.execute(self.acc(26),0)
        t.f[1]=t.f[2]=scalar(0x3F800001)
        t.execute(self.acc(28,fd=3),0)
        self.assertEqual(word(t.f[3]),0)
    def test_reserved_and_uninitialized_accumulator(self):
        for fn in (28,30):
            t=self.trace();t.f[1]=t.f[2]=1.0
            with self.assertRaisesRegex(ValueError,'uninitialized accumulator'):t.execute(self.acc(fn),0)
        for fn in (26,30):
            t=self.trace();t.f[1]=t.f[2]=1.0
            with self.assertRaisesRegex(ValueError,'reserved accumulator'):t.execute(self.acc(fn,fd=1),0)
        with self.assertRaises(ValueError):self.trace().execute(self.acc(27),0)
    def test_finite_normal_zero_domain(self):
        for value in (float('inf'),float('nan'),scalar(1),scalar(0x807FFFFF)):
            t=self.trace();t.f[1],t.f[2]=value,1.0
            with self.assertRaises(ValueError):t.execute(self.acc(26),0)
        t=self.trace();t.f[1],t.f[2]=scalar(0x00800000),0.5
        with self.assertRaises(ValueError):t.execute(self.acc(26),0)
        t=self.trace();t.f[1],t.f[2]=-0.0,1.0
        t.execute(self.acc(26),0);self.assertEqual(word(t.accumulator),0x80000000)
        t.f[1]=0.0;t.execute(self.acc(30),0);self.assertEqual(word(t.accumulator),0)
    def test_scoped_code_and_aligned_initialized_memory(self):
        t=self.trace()
        for a,b in model.RANGES+model.HELPERS:
            t.fetch(a)
            with self.assertRaises(ValueError):t.fetch(a+1)
        for pc in (0x2C1AD0,0x2C2258,0x2C23C0):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0]+4)
        with self.assertRaises(ValueError):t.load(model.parent.FIRST,4)
        for i in range(4):t.memory[model.parent.FIRST+i]=0
        self.assertEqual(t.load(model.parent.FIRST,4),0)
        for a,n in ((model.parent.FIRST+1,4),(model.parent.END,4),(model.parent.FIRST,3)):
            with self.assertRaises(ValueError):t.load(a,n)
    def test_all_encoded_delay_controls_are_rejected(self):
        outer=(4<<26)|(4<<21)|(5<<16)|2
        delay=((4<<26)|(4<<21)|(5<<16)|2,(1<<26)|(6<<21)|(1<<16)|2,
               (17<<26)|(8<<21)|2,0x03E00008,(3<<26)|(0x2A2780>>2))
        for equal in (True,False):
            for w in delay:
                t=self.trace((outer,w,0x03E00008,0,0x03E00008,0))
                t.r[4],t.r[5],t.r[6]=0,0 if equal else 1,0xFFFFFFFF;t.r[31]=RETURN;t.condition=True
                with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(model.ENTRIES[0])
    def test_unknown_calls_cross_body_and_bound(self):
        t=self.trace();t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unknown accumulator'):t.library_call(0x29CF28)
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        a,b=model.RANGES[0];t=self.trace(((4<<26)|((b-a-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(a)

if __name__=='__main__':unittest.main()
