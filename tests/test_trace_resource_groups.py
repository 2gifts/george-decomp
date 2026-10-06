import hashlib
import struct
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_resource_groups import GroupTrace,BUFFER,END,GROUP,HOLDER,CHILDREN,PAYLOADS,RANGES,ENTRIES,CALLS,RETURN,ORIGINAL_TABLE,TABLE_END,FALLBACK_GLOBAL,VIRTUAL


class ResourceGroupGuardTests(unittest.TestCase):
    def trace(self):
        t=GroupTrace(b'')
        for p in range(BUFFER,END):t.memory[p]=0x55
        return t

    def test_only_complete_group_and_real_helper_bodies(self):
        t=self.trace()
        for pc in (RANGES[0][0],RANGES[-1][1],0x20F9B0,0x226D78,RANGES[0][0]+1):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)
        entry,end=RANGES[5];t=GroupTrace(bytearray(end-0xFF000))
        self.assertEqual(t.fetch(entry),0);self.assertEqual(t.fetch(end-4),0)
        for entry in ENTRIES[5:]:
            with self.assertRaises(ValueError):t.library_call(entry)

    def test_all_authored_memory_is_bounded_aligned_and_initialized(self):
        t=self.trace()
        for address,size in ((BUFFER-4,4),(END,4),(END-1,2),(BUFFER+1,4),(BUFFER,3),(0x4455A0,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,1,size)
        with self.assertRaises((ValueError,KeyError)):t.load(0x7F000,4)
        with self.assertRaises(ValueError):t.save(ORIGINAL_TABLE,1,4)

    def test_complete_readonly_table_hash_and_global_permission(self):
        authored=bytes(range(16));raw=bytearray(TABLE_END-0xFF000)
        raw[ORIGINAL_TABLE-0xFF000:TABLE_END-0xFF000]=authored
        t=GroupTrace(raw)
        with self.assertRaises(ValueError):t.load(ORIGINAL_TABLE+8,2)
        with patch('trace_resource_groups.TABLE_SHA256',hashlib.sha256(authored).hexdigest()):
            self.assertEqual(t.load(ORIGINAL_TABLE+8,2),0x0908)
            self.assertEqual(t.load(ORIGINAL_TABLE+12,4),0x0F0E0D0C)
            raw[ORIGINAL_TABLE-0xFF000]^=1
            with self.assertRaises(ValueError):t.load(ORIGINAL_TABLE+12,4)
        for address,size in ((ORIGINAL_TABLE+1,2),(ORIGINAL_TABLE+14,4),(ORIGINAL_TABLE,1),(TABLE_END,4)):
            with self.assertRaises(ValueError):t.load(address,size)
        self.assertEqual(t.load(FALLBACK_GLOBAL,4),PAYLOADS[3])
        with self.assertRaises(ValueError):t.load(FALLBACK_GLOBAL,2)
        with self.assertRaises(ValueError):t.load(FALLBACK_GLOBAL+4,4)

    def test_precise_controlled_calls_and_fixed_event_geometry(self):
        t=self.trace();t.r[4]=HOLDER
        with self.assertRaises(ValueError):t.library_call(0x393B74)
        with self.assertRaises(ValueError):t.library_call(CALLS[0])
        self.assertEqual(t.events,[]);self.assertEqual(t.calls,[0]*13)
        t.r[4]=CHILDREN[0];t.r[5]=0;t.library_call(CALLS[2])
        self.assertEqual(len(t.events),11)
        self.assertEqual(t.events[:6],[2,CHILDREN[0],0,0,0,0])
        self.assertEqual(t.calls[2],1)

    def test_signed_virtual_adjustments_and_captured_mutation(self):
        t=self.trace();t.mutation=6;t.r[4]=CHILDREN[0]-4;t.r[5]=0
        t.library_call(VIRTUAL)
        self.assertEqual(t.load(GROUP+0x28,4),CHILDREN[3])
        self.assertEqual(t.load(GROUP+0x68,4),CHILDREN[3])
        self.assertEqual(t.load(GROUP+0x24,2),1)
        t.r[4]=CHILDREN[0]
        with self.assertRaises(ValueError):t.library_call(VIRTUAL)
        t.adjustment=12;t.r[4]=CHILDREN[0]+12;t.library_call(VIRTUAL)
        t.r[5]=1
        with self.assertRaises(ValueError):t.library_call(VIRTUAL)

    def test_only_actual_jr31_returns_with_owned_delay(self):
        entry,end=RANGES[5];raw=bytearray(end-0xFF000)
        for insn in (0x00800008,(2<<26)|(RANGES[6][0]>>2),(3<<26)|0x3FFFFFF):
            struct.pack_into('<II',raw,entry-0xFF000,insn,0)
            t=GroupTrace(raw);t.r[4],t.r[31]=RETURN,RETURN
            with self.assertRaises(ValueError):t.run(entry)
        struct.pack_into('<II',raw,entry-0xFF000,0x03E00008,0)
        t=GroupTrace(raw);t.r[31]=entry+8
        with self.assertRaises(ValueError):t.run(entry)
        raw=bytearray(end-0xFF000);struct.pack_into('<II',raw,end-8-0xFF000,0x03E00008,0)
        t=GroupTrace(raw);t.r[31]=entry+8;t.run(entry,entry+8)
        self.assertEqual(t.instruction_count,(end-entry)//4)

    def test_untaken_ordinary_delay_and_terminal_bounds(self):
        entry,end=RANGES[5];raw=bytearray(end-0xFF000)
        struct.pack_into('<II',raw,entry-0xFF000,0x14000000,0x03E00008)
        t=GroupTrace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        raw=bytearray(end-0xFF000)
        struct.pack_into('<I',raw,entry-0xFF000,(4<<26)|((end-entry-8)//4))
        struct.pack_into('<I',raw,end-4-0xFF000,0x03E00008)
        t=GroupTrace(raw);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

    def test_reserved_operands_and_unowned_vu_forms_rejected(self):
        t=self.trace()
        for insn in (0x00810809,0x00A1F809,0x00A0F849,0x0081086D,0x46010006,0x44850801,0x4BE000D3):
            with self.assertRaises(ValueError):t.execute(insn,0)
        t.r[4]=BUFFER+1
        with self.assertRaises(ValueError):t.execute((0x21<<26)|(4<<21)|(2<<16),0)


if __name__=='__main__':unittest.main()
