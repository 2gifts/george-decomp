"""Asset-free rejection and observation tests for the bounded scalar tracer."""
import hashlib
import struct
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_resource_bounds import (BoundsTrace,BUFFER,END,GROUP,ALLOCATED,HOLDER,
    OTHER_HOLDER,CHILDREN,PAYLOADS,KEYS,RANGES,HELPER_RANGES,ENTRIES,CALLS,
    TABLES,POINT,FRAME,RETURN)


class ResourceBoundsGuardTests(unittest.TestCase):
    def trace(self,original=b''):
        t=BoundsTrace(original)
        for a,b in ((BUFFER,END),(0x7F000,0x81000)):
            for p in range(a,b):t.memory[p]=0x55
        return t

    def body(self,index=0):
        entry,end=RANGES[index]
        return entry,end,bytearray(end-0xFF000)

    def test_only_complete_bounds_and_owned_helpers(self):
        t=self.trace()
        for entry,end in (*RANGES,*HELPER_RANGES):
            raw=bytearray(end-0xFF000);owned=self.trace(raw)
            self.assertEqual(owned.fetch(entry),0)
            self.assertEqual(owned.fetch(end-4),0)
        for pc in (RANGES[0][0]+1,RANGES[3][1],RANGES[-1][1],0x226BC0):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.fetch(RANGES[0][0])
        with self.assertRaises(ValueError):t.run(RANGES[1][0]+4)
        for entry in ENTRIES:
            with self.assertRaises(ValueError):t.library_call(entry)

    def test_aligned_initialized_memory_and_no_other_subsystem_permissions(self):
        t=self.trace()
        for address,size in ((BUFFER-4,4),(END,4),(END-1,2),(BUFFER+1,4),
                             (BUFFER,3),(0x4682DC,4),(0x4455D0,4),(0x4455A0,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,1,size)
        del t.memory[BUFFER+3]
        with self.assertRaises(KeyError):t.load(BUFFER,4)
        # Scalar writes are expressly permitted in authored windows.
        t.save(BUFFER,0xAABBCCDD,4)
        self.assertEqual(t.load(BUFFER,4),0xAABBCCDD)

    def test_both_complete_readonly_prefix_hashes(self):
        authored=bytes(range(16));raw=bytearray(max(TABLES)+16-0xFF000)
        for base in TABLES:raw[base-0xFF000:base+16-0xFF000]=authored
        t=self.trace(raw)
        with patch('trace_resource_bounds.TABLES',{base:hashlib.sha256(authored).hexdigest() for base in TABLES}):
            for base in TABLES:
                self.assertEqual(t.load(base+8,2),0x0908)
                self.assertEqual(t.load(base+12,4),0x0F0E0D0C)
                for address,size in ((base,1),(base+1,2),(base+14,4),(base+16,4)):
                    with self.assertRaises(ValueError):t.load(address,size)
                with self.assertRaises(ValueError):t.save(base,0,4)
            # Tampering with an unobserved header word still invalidates the
            # whole checked prefix; an observed pair alone is insufficient.
            for base in TABLES:
                raw[base-0xFF000]^=1
                with self.assertRaisesRegex(ValueError,'prefix'):t.load(base+12,4)

    def test_halfword_loop_slt_domain_and_reserved_operand(self):
        t=self.trace();instruction=(4<<21)|(5<<16)|(2<<11)|0x2A
        for left,right in ((0,0),(0,65535),(65535,65536),(65536,65535)):
            t.r[4],t.r[5]=left,right;t.execute(instruction,0)
            self.assertEqual(t.r[2],int(left<right))
        for value in (-1,65537,0xFFFFFFFF,0x100000000):
            t.r[4],t.r[5]=value,1
            with self.assertRaisesRegex(ValueError,'domain'):t.execute(instruction,0)
        t.r[4],t.r[5]=0,1
        with self.assertRaisesRegex(ValueError,'reserved'):t.execute(instruction|(1<<6),0)
        t.r[4]=BUFFER;t.save(BUFFER+2,0xFFFC,2)
        t.execute((0x21<<26)|(4<<21)|(2<<16)|2,0)
        self.assertEqual(t.r[2],0xFFFFFFFC)
        with self.assertRaises(ValueError):t.execute((0x21<<26)|(4<<21)|(2<<16)|1,0)

    def test_calls_reject_before_publishing_and_capture_before_mutation(self):
        t=self.trace();t.r[4:9]=[KEYS[0],2,0xDEAD,0xBEEF,123]
        for target in (0x393B74,CALLS[0],CALLS[2]):
            with self.assertRaises(ValueError):t.library_call(target)
        self.assertEqual(t.calls,[0]*11);self.assertEqual(t.events,[])
        t.save(GROUP+2,4,1);t.save(GROUP+0x10,HOLDER,4)
        t.mutation=7;t.r[4:9]=[GROUP,3,0xDEAD,0xBEEF,123]
        t.library_call(CALLS[8])
        self.assertEqual(t.events[:6],[8,GROUP,3,0,0,0])
        self.assertEqual(t.events[8],HOLDER)
        self.assertEqual(t.load(GROUP+0x10,4),OTHER_HOLDER)
        self.assertEqual(t.r[2],0x87654321)

    def test_virtual_adjustments_and_real_constructor_output_contract(self):
        t=self.trace();t.r[4:6]=[CHILDREN[0]-4,0]
        t.library_call(CALLS[6]);self.assertEqual(t.calls[6],1)
        t.r[4]=CHILDREN[0]
        with self.assertRaises(ValueError):t.library_call(CALLS[6])
        t.adjustment=12;t.r[4:6]=[PAYLOADS[0]+12,3]
        t.library_call(CALLS[10]);self.assertEqual(t.calls[10],1)
        t.r[5]=0
        with self.assertRaises(ValueError):t.library_call(CALLS[10])
        t.r[4:9]=[ALLOCATED,KEYS[0],6,0xFEDCBA98,0x1234ABCD]
        t.library_call(CALLS[2])
        self.assertEqual(t.load(ALLOCATED,2),1)
        self.assertEqual(t.load(ALLOCATED+0x24,4),PAYLOADS[2])

    def test_projection_writes_only_point_and_first_twelve_frame_words(self):
        t=self.trace();point,frame=0x80000,0x80020
        t.r[4:9]=[HOLDER,point,frame,0xDEAD,0xBEEF]
        t.library_call(CALLS[9])
        self.assertEqual(t.events[:6],[9,HOLDER,0xF00000E0,0xF00000F0,0,0])
        self.assertEqual([t.load(point+i*4,4) for i in range(4)],list(POINT))
        self.assertEqual([t.load(frame+i*4,4) for i in range(12)],list(FRAME))
        for p in (point-4,point+16,frame-4,frame+48,frame+60):
            self.assertEqual(t.load(p,4),0x55555555)
        self.assertEqual(t.r[2],PAYLOADS[0])
        for a,b,c in ((HOLDER,point+4,frame),(HOLDER,point,point),
                      (HOLDER,point,point+0x10-16),(HOLDER,0x81000,frame),
                      (HOLDER,point,0x80FE0),(GROUP,point,frame)):
            t.r[4:7]=[a,b,c]
            with self.assertRaisesRegex(ValueError,'projection'):t.library_call(CALLS[9])

    def test_return_delay_side_effect_and_only_actual_jr31(self):
        entry,end,raw=self.body()
        struct.pack_into('<II',raw,entry-0xFF000,0x03E00008,0x24020001)
        t=self.trace(raw);t.r[31]=RETURN;t.run(entry)
        self.assertEqual(t.r[2],1);self.assertEqual(t.instruction_count,2)
        t=self.trace(raw);t.r[31]=entry+4
        with self.assertRaisesRegex(ValueError,'return'):t.run(entry)
        struct.pack_into('<I',raw,entry-0xFF000,0x00800008)
        t=self.trace(raw);t.r[4]=t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'return'):t.run(entry)
        struct.pack_into('<I',raw,entry-0xFF000,0x03E00008)
        struct.pack_into('<I',raw,entry+4-0xFF000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay'):t.run(entry)

    def test_likely_annul_ordinary_delay_and_loop_bound(self):
        entry,end,raw=self.body(1)
        # Equal operands make BNEZL annul its otherwise unsupported delay.
        struct.pack_into('<IIII',raw,entry-0xFF000,0x54000001,0xFFFFFFFF,0x03E00008,0x24020001)
        t=self.trace(raw);t.r[31]=RETURN;t.run(entry)
        self.assertEqual(t.r[2],1);self.assertEqual(t.instruction_count,3)
        struct.pack_into('<II',raw,entry-0xFF000,0x14000000,0x03E00008)
        t=self.trace(raw);t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'delay'):t.run(entry)
        entry,end,raw=self.body()
        struct.pack_into('<II',raw,entry-0xFF000,0x1000FFFF,0)
        t=self.trace(raw)
        with self.assertRaisesRegex(ValueError,'bound'):t.run(entry)

    def test_unowned_cop_and_reserved_forms_fail(self):
        t=self.trace()
        for instruction in (0x00810809,0x00A1F809,0x00A0F849,0x0081086D,
                            0x46010006,0x44850801,0x4BE000D3,0xFFFFFFFF):
            with self.assertRaises(ValueError):t.execute(instruction,0)


if __name__=='__main__':unittest.main()
