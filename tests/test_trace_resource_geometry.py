"""Scope/return/operand guards for the bounded resource instruction model."""
import hashlib
from pathlib import Path
import struct
import sys
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from trace_resource_geometry import ResourceTrace, RANGES, TABLE, TABLE_END, BUFFER, END, ENTRIES, TABLE_SHA256
from trace_geometry import RETURN


def image(words):
    result=bytearray(TABLE_END-0xFF000)
    for address,value in words.items():struct.pack_into('<I',result,address-0xFF000,value)
    return result


class ResourceGuards(unittest.TestCase):
    def trace(self,words=None):
        t=ResourceTrace(image(words or {}));t.r[31]=RETURN
        for address in range(BUFFER,END):t.memory[address]=0
        return t

    def test_entry_instruction_bounds(self):
        t=self.trace()
        for a,b in RANGES:
            self.assertEqual(t.fetch(a),0)
            with self.assertRaises(ValueError):t.fetch(a+1)
            if not any(c<=b<d for c,d in RANGES):
                with self.assertRaises(ValueError):t.fetch(b)
        with self.assertRaises(ValueError):t.run(ENTRIES[0]+4)
        with self.assertRaises(ValueError):ResourceTrace(b'').fetch(ENTRIES[0])

    def test_authored_memory_and_readonly_guard(self):
        t=self.trace();t.save(BUFFER,0x12345678,4);self.assertEqual(t.load(BUFFER,4),0x12345678)
        for address,size in ((END-1,4),(BUFFER-1,1),(0x4455D0,4),(0x7EFFF,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,0,size)
        with self.assertRaises(ValueError):t.save(TABLE,0,4)
        with self.assertRaises(ValueError):t.load(TABLE,4)  # wrong full-table SHA
        with self.assertRaises(ValueError):t.load(TABLE+1,2)
        with self.assertRaises(ValueError):t.load(TABLE_END-2,4)
        del t.memory[BUFFER]
        with self.assertRaises(KeyError):t.load(BUFFER,1)

    def test_jr_requires_actual_ra_and_selected_stop(self):
        a=ENTRIES[3]
        for instruction,register,target in ((0x02000008,16,RETURN),(0x03E00008,31,ENTRIES[2]),(0x03E00008,31,a+8)):
            t=self.trace({a:instruction,a+4:0});t.r[register]=target
            with self.assertRaisesRegex(ValueError,'return'):t.run(a)
        t=self.trace({a:0x03E00008,a+4:0});t.run(a)

    def test_transfer_in_taken_untaken_and_return_delays(self):
        a=ENTRIES[3]
        # BEQ zero,zero; BEQ zero,v0 untaken; JR ra.
        for instruction in (0x10000001,0x10020001,0x03E00008):
            t=self.trace({a:instruction,a+4:0x03E00008});t.r[2]=1
            with self.assertRaisesRegex(ValueError,'delay'):t.run(a)

    def test_branch_cannot_enter_another_body(self):
        a=ENTRIES[3];offset=(ENTRIES[4]-a-4)//4
        t=self.trace({a:0x10000000|offset,a+4:0})
        with self.assertRaisesRegex(ValueError,'transfer'):t.run(a)

    def test_jal_unknown_and_jalr_reserved_operands(self):
        a=ENTRIES[3]
        t=self.trace({a:0x0C000000|(0x123456>>2),a+4:0})
        with self.assertRaisesRegex(ValueError,'unknown'):t.run(a)
        for instruction in (0x0060F849,0x00610009,0x00600809):
            with self.assertRaisesRegex(ValueError,'JALR'):self.trace().execute(instruction,a)

    def test_narrow_byte_halfword_and_signed_gate(self):
        t=self.trace();a=ENTRIES[0];t.r[4]=BUFFER;t.r[2]=0x1234FFFF
        t.execute(0xA0820000,a);self.assertEqual(t.load(BUFFER,1),255)
        t.execute(0xA4820002,a);self.assertEqual(t.load(BUFFER+2,2),65535)
        t.execute(0x84830002,a);self.assertEqual(t.r[3],0xFFFFFFFF)
        t.execute(0x28620003,a);self.assertEqual(t.r[2],1)
        t.r[3]=0x80000000;t.execute(0x28620003,a);self.assertEqual(t.r[2],1)
        t.r[3]=3;t.execute(0x28620003,a);self.assertEqual(t.r[2],0)

    def test_observed_calls_reject_wrong_abi(self):
        for target in (0x225E80,0x2AEE60,0x226BC0,0x226E38,0x20F070,0x2A1C60,0x29FEF8,0x2A48F0,0x2A4808):
            with self.assertRaises(ValueError):self.trace().library_call(target)

    def test_instruction_bound(self):
        a=ENTRIES[3];t=self.trace({a:0x1000FFFF,a+4:0})
        with self.assertRaisesRegex(ValueError,'bound'):t.run(a)


if __name__=='__main__':unittest.main()
