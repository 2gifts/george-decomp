import hashlib
import struct
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_resource_manager as model
from trace_geometry import RETURN

class ManagerGuardTests(unittest.TestCase):
    def trace(self,raw=None,**kwargs):
        # Independently authored equal destinations support isolated decoder
        # guards; the real switch bytes are never copied into this test.
        if raw is None:raw=bytearray(model.TABLE-0xFF000+28)
        data=struct.pack('<7I',*[model.RANGES[0][0]+8]*7)
        raw[model.TABLE-0xFF000:model.TABLE-0xFF000+28]=data
        with patch.object(model,'TABLE_HASH',hashlib.sha256(data).hexdigest()):
            t=model.ManagerTrace(raw,**kwargs)
        for a,b in model.MEMORY_RANGES:
            for p in range(a,b):t.memory[p]=0
        t.save(model.GLOBAL,model.MANAGERS[0],4)
        return t

    def test_whole_table_identity_required_even_before_execution(self):
        with self.assertRaisesRegex(ValueError,'switch identity'):model.ManagerTrace(b'')
        raw=bytearray(model.TABLE-0xFF000+28)
        with self.assertRaisesRegex(ValueError,'switch identity'):model.ManagerTrace(raw)

    def test_fetch_and_entry_scope(self):
        t=self.trace()
        self.assertEqual(t.fetch(model.RANGES[0][0]),0)
        for pc in (model.RANGES[0][0]+1,model.RANGES[0][1],0x225D38,model.TABLE,0x2AD748):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(model.RANGES[0][0]+4)
        t.original=b''
        with self.assertRaises(ValueError):t.fetch(model.RANGES[0][0])

    def test_memory_bounds_alignment_readonly_and_unknown_bytes(self):
        t=self.trace()
        for a,n in ((model.BUFFER-1,4),(model.END,4),(model.BUFFER+1,4),(model.GLOBAL,8),(model.TABLE+28,4),(model.BUFFER,3)):
            with self.assertRaises(ValueError):t.load(a,n)
            with self.assertRaises(ValueError):t.save(a,1,n)
        self.assertEqual(t.load(model.TABLE,4),model.RANGES[0][0]+8)
        with self.assertRaisesRegex(ValueError,'readonly'):t.save(model.TABLE,1,4)
        del t.memory[model.BUFFER]
        with self.assertRaises(ValueError):t.load(model.BUFFER,4)
        with self.assertRaises(ValueError):t.save(model.BUFFER,0,4)

    def test_signed_immediate_comparisons_and_xori_low32(self):
        t=self.trace();t.r[2]=0xFFFFFFFF
        t.execute((0xA<<26)|(2<<21)|(3<<16)|0x3E8,0)
        t.execute((0xB<<26)|(2<<21)|(4<<16)|0xFFFF,4)
        self.assertEqual((t.r[3],t.r[4]),(1,0))
        t.r[2]=0xFFFFFFFE;t.execute((0xB<<26)|(2<<21)|(4<<16)|0xFFFF,8)
        self.assertEqual(t.r[4],1)
        t.r[2]=0x80000000;t.execute((0xE<<26)|(2<<21)|(5<<16)|0xFFFF,12)
        self.assertEqual(t.r[5],0x8000FFFF)
        t.execute((0xE<<26)|(2<<21)|0xFFFF,16);self.assertEqual(t.r[0],0)

    def test_inherited_special_forms_and_pool_halfwords(self):
        t=self.trace();t.r[2],t.r[3]=0xFFFFFFFF,32
        t.execute((2<<21)|(3<<16)|(4<<11)|0x18,0)
        self.assertEqual(t.r[4],0xFFFFFFE0)
        t.execute((5<<11)|0x12,4);self.assertEqual(t.r[5],0xFFFFFFE0)
        t.r[6]=model.BUFFER;t.save(model.BUFFER,0xFFFF,2)
        t.execute((0x21<<26)|(6<<21)|(7<<16),8)
        self.assertEqual(t.r[7],0xFFFFFFFF)
        t.execute((0x25<<26)|(6<<21)|(8<<16),12);self.assertEqual(t.r[8],0xFFFF)
        for w in ((2<<21)|(3<<16)|(4<<11)|(1<<6)|0x18,
                  (2<<21)|(4<<11)|0x12,(3<<16)|(4<<11)|0x10,
                  (2<<21)|(3<<16)|(4<<11),0x3C220001,
                  0x46000000,0x4BE000D3,(4<<21)|(31<<11)|9):
            with self.assertRaises(ValueError):t.execute(w,16)

    def test_controlled_call_validation_nine_args_and_events(self):
        t=self.trace();t.r[29]=0x80000
        with self.assertRaises(ValueError):t.library_call(0x20E59C)
        t.r[4:9]=[model.OBJECTS[0],1,model.KEY,0x80014,model.RECORD+12]
        with self.assertRaises(ValueError):t.library_call(model.CALLS[1])
        self.assertEqual(t.events,[])
        t.r[4:12]=[model.QUEUE,7,model.KEY,0x1234,129,model.PAYLOAD,model.COMPLETION,model.RECORD]
        t.save(0x80000,1,4);t.library_call(model.CALLS[5])
        self.assertEqual(len(t.events),20)
        self.assertEqual(t.events[:10],[5,model.QUEUE,7,model.KEY,0x1234,129,model.PAYLOAD,model.COMPLETION,model.RECORD,1])
        self.assertEqual(t.r[3:16],[0xDEADBEEF]*13)
        t.r[4:12]=[model.QUEUE,7,model.KEY,0x1234,129,model.PAYLOAD,model.COMPLETION,model.RECORD]
        t.save(0x80000,2,4)
        with self.assertRaises(ValueError):t.library_call(model.CALLS[5])

    def test_actual_jr31_internal_selected_stop_and_only_proven_switch_jr4(self):
        entry,end=model.RANGES[1];raw=bytearray(model.TABLE-0xFF000+28)
        struct.pack_into('<II',raw,end-8-0xFF000,0x03E00008,0)
        t=self.trace(raw);t.r[31]=entry+8;t.run(entry,entry+8)
        self.assertEqual(t.instruction_count,(end-entry)//4)
        t=self.trace(raw);t.r[31]=entry+8
        with self.assertRaises(ValueError):t.run(entry)
        struct.pack_into('<II',raw,entry-0xFF000,0x00800008,0)
        t=self.trace(raw);t.r[4]=model.RANGES[0][0]+8
        with self.assertRaisesRegex(ValueError,'indirect'):t.run(entry)
        entry=model.RANGES[0][0]
        raw=bytearray(model.TABLE-0xFF000+28)
        struct.pack_into('<II',raw,entry-0xFF000,(4<<26)|((0x224B70-entry-4)//4),0)
        struct.pack_into('<II',raw,0x224B70-0xFF000,0x00800008,0)
        struct.pack_into('<II',raw,entry+8-0xFF000,0x03E00008,0)
        t=self.trace(raw);t.r[4],t.r[31]=entry+8,RETURN;t.run(entry)
        self.assertEqual(t.instruction_count,6)
        t=self.trace(raw);t.r[4],t.r[31]=entry+12,RETURN
        with self.assertRaisesRegex(ValueError,'indirect'):t.run(entry)

    def test_untaken_ordinary_delay_and_likely_annul_bounds(self):
        entry,end=model.RANGES[1];raw=bytearray(model.TABLE-0xFF000+28)
        struct.pack_into('<II',raw,entry-0xFF000,0x14000000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        struct.pack_into('<II',raw,entry-0xFF000,0x54000000,0xFFFFFFFF)
        struct.pack_into('<II',raw,entry+8-0xFF000,0x03E00008,0)
        t=self.trace(raw);t.r[31]=RETURN;t.run(entry)
        self.assertEqual(t.instruction_count,3)
        raw=bytearray(model.TABLE-0xFF000+28)
        struct.pack_into('<I',raw,entry-0xFF000,(4<<26)|((end-entry-8)//4))
        struct.pack_into('<I',raw,end-4-0xFF000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

    def test_owned_loop_remains_bounded(self):
        entry=model.RANGES[1][0];raw=bytearray(model.TABLE-0xFF000+28)
        struct.pack_into('<II',raw,entry-0xFF000,0x1000FFFF,0)
        t=self.trace(raw)
        with self.assertRaisesRegex(ValueError,'bound'):t.run(entry)

if __name__=='__main__':unittest.main()
