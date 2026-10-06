import struct,sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_resource_registry as model
from trace_geometry import RETURN

class RegistryTraceGuards(unittest.TestCase):
    def trace(self,instructions=()):
        image=bytearray(max(b for a,b in model.RANGES+model.HELPERS)-0xFF000)
        for i,w in enumerate(instructions):struct.pack_into('<I',image,model.ENTRIES[0]-0xFF000+4*i,w)
        return model.RegistryTrace(bytes(image))

    def test_code_entry_and_complete_body_bounds(self):
        t=self.trace()
        for a,b in model.RANGES+model.HELPERS:
            self.assertIsInstance(t.fetch(a),int)
            with self.assertRaises(ValueError):t.fetch(a+1)
            if b not in model.ENTRIES+tuple(a for a,b in model.HELPERS):
                with self.assertRaises(ValueError):t.fetch(b)
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0]+4)
        with self.assertRaises(ValueError):t.fetch(0x100000)

    def test_memory_alignment_bounds_and_initialization(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.load(model.BUFFER,4)
        for i in range(16):t.memory[model.BUFFER+i]=0
        t.save(model.BUFFER,0x12345678,4);self.assertEqual(t.load(model.BUFFER,4),0x12345678)
        for address,size in ((model.BUFFER+1,4),(model.BUFFER-4,4),(model.END,4),(model.GLOBAL,8),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,0,size)

    def test_new_signed_comparison_or_xori_and_blez(self):
        t=self.trace();t.r[2],t.r[3]=0xFFFFFFFF,1
        t.execute((2<<21)|(3<<16)|(4<<11)|0x2A,0)
        t.execute((3<<21)|(2<<16)|(5<<11)|0x2A,4)
        self.assertEqual((t.r[4],t.r[5]),(1,0))
        t.execute((2<<21)|(3<<16)|(6<<11)|0x25,8);self.assertEqual(t.r[6],0xFFFFFFFF)
        t.execute((0xE<<26)|(2<<21)|(7<<16)|0xFFFF,12);self.assertEqual(t.r[7],0xFFFF0000)
        self.assertEqual(t.execute((6<<26)|(2<<21)|2,16),(28,False))
        self.assertEqual(t.execute((6<<26)|(3<<21)|2,20),(None,False))
        t.execute((0xE<<26)|(2<<21)|0xFFFF,24);self.assertEqual(t.r[0],0)

    def test_division_remainder_and_signed_mult_low32(self):
        t=self.trace();t.r[2],t.r[3]=0xFFFFFFFF,7
        with self.assertRaises(ValueError):t.execute((4<<11)|0x10,0)
        t.execute((2<<21)|(3<<16)|0x1B,4)
        t.execute((4<<11)|0x10,8);t.execute((5<<11)|0x12,12)
        self.assertEqual((t.r[4],t.r[5]),(0xFFFFFFFF%7,0xFFFFFFFF//7))
        t.r[3]=12;t.execute((2<<21)|(3<<16)|(6<<11)|0x18,16)
        self.assertEqual(t.r[6],0xFFFFFFF4)
        t.r[3]=0
        with self.assertRaisesRegex(ValueError,'zero division'):t.execute((2<<21)|(3<<16)|0x1B,20)

    def test_reserved_or_unsupported_operand_forms(self):
        t=self.trace()
        words=((2<<21)|(3<<16)|(4<<11)|(1<<6)|0x25,
               (2<<21)|(3<<16)|(4<<11)|(1<<6)|0x2A,
               (6<<26)|(2<<21)|(1<<16),
               (4<<21)|(31<<11)|9,
               (2<<21)|(3<<16)|(4<<11)|0x1B,
               (2<<21)|(4<<11)|0x10,
               (2<<21)|(3<<16)|(4<<11),
               (15<<26)|(1<<21),0x46000000,0x70000000)
        for w in words:
            with self.subTest(word=hex(w)),self.assertRaises(ValueError):t.execute(w,0)

    def test_untaken_delay_executes_and_likely_annuls(self):
        t=self.trace(((5<<26)|2,0x46000000,0x03E00008,0));t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(model.ENTRIES[0])
        t=self.trace(((21<<26)|2,0x46000000,0x03E00008,0));t.r[31]=RETURN
        t.run(model.ENTRIES[0]);self.assertEqual(t.instruction_count,3)
        t=self.trace(((4<<26)|2,(4<<26)|2,0,0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'transfer in delay'):t.run(model.ENTRIES[0])
        entry,end=model.RANGES[0];image=bytearray(self.trace().original)
        struct.pack_into('<I',image,entry-0xFF000,(4<<26)|((end-entry-8)//4))
        struct.pack_into('<I',image,end-4-0xFF000,0x03E00008)
        t=model.RegistryTrace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay outside'):t.run(entry)

    def test_actual_jr31_only_even_internal_selected_stop(self):
        entry=model.ENTRIES[0];stop=entry+8
        t=self.trace(((4<<26)|1,0,(9<<26)|(2<<16)|9,0x03E00008,0));t.r[31]=stop
        t.run(entry,stop);self.assertEqual(t.r[2],9)
        t=self.trace(((2<<21)|8,0));t.r[2]=stop
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(entry,stop)
        t=self.trace((0x03E00008,0));t.r[31]=stop
        with self.assertRaisesRegex(ValueError,'selected stop'):t.run(entry)
        t=self.trace(((3<<26)|(stop>>2),0));t.r[31]=stop
        with self.assertRaisesRegex(ValueError,'unknown registry call'):t.run(entry,stop)

    def test_actual_accessor_call_executes_and_unknown_calls_reject(self):
        helper=model.HELPERS[0][0]
        # Save/restore RA around a genuine nested two-instruction accessor.
        t=self.trace(((31<<21)|(16<<11)|0x2D,(3<<26)|(helper>>2),0,
                      (16<<21)|(31<<11)|0x2D,0x03E00008,0))
        image=bytearray(t.original);struct.pack_into('<II',image,helper-0xFF000,0x03E00008,0x8C820000)
        t=model.RegistryTrace(bytes(image));t.r[4],t.r[31]=model.BUFFER,RETURN
        for i in range(4):t.memory[model.BUFFER+i]=(0x12345678>>(8*i))&255
        t.run(model.ENTRIES[0]);self.assertEqual(t.r[2],0x12345678);self.assertEqual(t.instruction_count,8)
        t=self.trace(((3<<26)|(0x2A8230>>2),0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'unknown registry call'):t.run(model.ENTRIES[0])

    def test_unsupported_jump_and_owned_loop_bounds(self):
        t=self.trace(((4<<26)|0xFFFF,0))
        with self.assertRaisesRegex(ValueError,'bound'):t.run(model.ENTRIES[0])
        t=self.trace(((4<<26)|((model.RANGES[0][1]-model.RANGES[0][0]-4)//4),0))
        with self.assertRaisesRegex(ValueError,'outside owned'):t.run(model.ENTRIES[0])

if __name__=='__main__':unittest.main()
