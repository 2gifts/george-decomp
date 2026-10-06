import struct,sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_path_sampling as model
from trace_geometry import RETURN,scalar,word

class PathTraceGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(model.HELPER[1]-0xFF000)
        for i,w in enumerate(instructions):struct.pack_into('<I',image,model.ENTRIES[0]-0xFF000+4*i,w)
        return model.PathTrace(bytes(image))

    def test_complete_entry_code_and_memory_guards(self):
        t=self.trace()
        for a,b in model.RANGES+(model.HELPER,):
            self.assertIsInstance(t.fetch(a),int)
            with self.assertRaises(ValueError):t.fetch(a+1)
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0]+4)
        with self.assertRaises(ValueError):t.fetch(0x136080)
        with self.assertRaises(ValueError):t.load(model.BUFFER,4)
        for i in range(16):t.memory[model.BUFFER+i]=0
        t.save(model.BUFFER,0x12345678,4)
        self.assertEqual(t.load(model.BUFFER,4),0x12345678)
        for a,n in ((model.BUFFER+1,4),(model.BUFFER-4,4),(model.END,4),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(a,n)
            with self.assertRaises(ValueError):t.save(a,0,n)

    def test_cvt_word_signed_bits_and_operand_guards(self):
        t=self.trace();t.f[1]=scalar(255)
        instruction=(17<<26)|(20<<21)|(1<<11)|(2<<6)|32
        t.execute(instruction,0);self.assertEqual(word(t.f[2]),word(255.0))
        t.f[1]=scalar(0x80000000);t.execute(instruction,4)
        self.assertEqual(word(t.f[2]),word(-2147483648.0))
        for w in (instruction|(1<<16),instruction^1,(17<<26)|(21<<21)|(1<<11)|32):
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,0)

    def test_finite_cop1_and_reserved_forms(self):
        t=self.trace();t.f[1]=1.0;t.f[2]=0.0
        with self.assertRaisesRegex(ValueError,'zero denominator'):
            t.execute((17<<26)|(16<<21)|(2<<16)|(1<<11)|3,0)
        for w in ((17<<26)|(16<<21)|4,(17<<26)|(8<<21)|(4<<16),
                  (17<<26)|(4<<21)|1,(17<<26)|(16<<21)|(1<<16)|6,
                  (17<<26)|(16<<21)|(1<<6)|0x32,0x70000000,(4<<21)|(31<<11)|9):
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,0)
        t.f[1]=float('inf')
        with self.assertRaisesRegex(ValueError,'finite'):
            t.execute((17<<26)|(16<<21)|(1<<11)|6,0)

    def test_float_ordinary_delay_executes_and_likely_annuls(self):
        branch=(17<<26)|(8<<21)|2
        t=self.trace((branch,0x70000000,0x03E00008,0));t.r[31]=RETURN;t.condition=True
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0])
        t=self.trace((branch|(2<<16),0x70000000,0x03E00008,0));t.r[31]=RETURN;t.condition=True
        t.run(model.ENTRIES[0]);self.assertEqual(t.instruction_count,3)
        t=self.trace(((4<<26)|2,(4<<26)|2,0,0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(model.ENTRIES[0])

    def test_actual_jr31_only_and_unknown_calls(self):
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(model.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=model.ENTRIES[0]+8
        with self.assertRaisesRegex(ValueError,'selected stop'):t.run(model.ENTRIES[0])
        t=self.trace(((3<<26)|(0x123450>>2),0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unknown controlled'):t.run(model.ENTRIES[0])
        with self.assertRaisesRegex(ValueError,'unknown controlled'):t.library_call(0x123450)

    def test_cross_body_delay_and_loop_bounds(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        a,b=model.RANGES[0];t=self.trace(((4<<26)|((b-a-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(a)
        image=bytearray(t.original);struct.pack_into('<I',image,a-0xFF000,(4<<26)|((b-a-8)//4));struct.pack_into('<I',image,b-4-0xFF000,0x03E00008)
        t=model.PathTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(a)

if __name__=='__main__':unittest.main()
