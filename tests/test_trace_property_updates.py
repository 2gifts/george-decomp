import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_updates import UpdateTrace,BUFFER,END,NODES,RANGES,ALL_RANGES,TRANSFORM,CALLBACKS,RETURN


class UpdateGuardTests(unittest.TestCase):
    def trace(self):
        t=UpdateTrace(b'')
        for address in range(BUFFER,END):t.memory[address]=0x55
        return t

    def test_only_complete_scalar_update_and_ownership_code(self):
        t=self.trace()
        for pc in (RANGES[0][0],RANGES[-1][1],0x2B9440,0x2AD9A8,TRANSFORM,RANGES[0][0]+1):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)
        entry,end=RANGES[0]
        original=bytearray(end-0xFF000)
        t=UpdateTrace(original)
        self.assertEqual(t.fetch(entry),0)
        self.assertEqual(t.fetch(end-4),0)

    def test_authored_memory_is_strict(self):
        t=self.trace()
        for address in (BUFFER-1,END,0x4455A0,0x3D48E8):
            with self.assertRaises(ValueError):t.load(address,4)
            with self.assertRaises(ValueError):t.save(address,1,4)
        with self.assertRaises(ValueError):t.save(END-1,1,2)
        with self.assertRaises((KeyError,ValueError)):t.load(0x7F000,4)

    def test_vu_contract_requires_exact_identity_and_pointer_abi(self):
        t=self.trace();node=NODES[0]
        t.r[4],t.r[5],t.r[6]=node+0x70,node+0x38,node+0x2C
        with self.assertRaises(ValueError):t.library_call(TRANSFORM)
        self.assertEqual(t.events,[])
        for i in range(4):t.save(node+0x38+i*4,0x3F800000 if i==3 else 0,4)
        t.r[5]+=4
        with self.assertRaises(ValueError):t.library_call(TRANSFORM)
        self.assertEqual(t.events,[])
        t.r[5]-=4;t.library_call(TRANSFORM)
        self.assertEqual(t.calls[6],1)
        self.assertEqual(t.load(node+0xAC,4),0x3F800000)
        t.save(node+0x38,0x80000000,4)
        with self.assertRaises(ValueError):t.library_call(TRANSFORM)
        self.assertEqual(t.calls[6],1)

    def test_one_node_callback_has_no_float_input_contract(self):
        t=self.trace();node=NODES[0]
        t.r[4]=node;t.f[12]=-4.5
        t.save(node+0x18,0x3FA00000,4);t.save(node+0x1C,0xC010,2)
        t.save(node+0x20,CALLBACKS[0],4);t.save(node+0x60,node+0x64,4)
        t.library_call(CALLBACKS[0])
        self.assertEqual(t.events,[7,node,0x3FA00000,0xC010,CALLBACKS[0],node+0x64])
        self.assertEqual(t.f[12],123.0)
        t.r[4]=BUFFER
        with self.assertRaises(ValueError):t.library_call(CALLBACKS[0])
        with self.assertRaises(ValueError):t.library_call(0x393B74)
        with self.assertRaises(ValueError):t.controlled_call(0x2B9440,node)

    def test_actual_return_and_owned_delays(self):
        entry,end=RANGES[0];original=bytearray(end-0xFF000)
        struct.pack_into('<II',original,entry-0xFF000,0x00800008,0)
        t=UpdateTrace(original);t.r[4]=RETURN;t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        struct.pack_into('<II',original,entry-0xFF000,0x0FFFFFFF,0)
        t=UpdateTrace(original);t.r[31]=0x0FFFFFFC
        with self.assertRaises(ValueError):t.run(entry,0x0FFFFFFC)
        # A direct jump into another reviewed function is not a local branch.
        struct.pack_into('<II',original,entry-0xFF000,(2<<26)|(RANGES[1][0]>>2),0)
        t=UpdateTrace(original)
        with self.assertRaises(ValueError):t.run(entry)
        # Branch to last owned word: its JR has no owned delay instruction.
        struct.pack_into('<I',original,entry-0xFF000,(4<<26)|10)
        struct.pack_into('<I',original,end-4-0xFF000,0x03E00008)
        t=UpdateTrace(original);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

    def test_untaken_ordinary_delays_and_internal_jr_are_strict(self):
        entry,end=RANGES[0];original=bytearray(end-0xFF000)
        # Untaken ordinary BNE still executes its owned delay exactly once,
        # rejecting a control transfer inside that delay.
        struct.pack_into('<II',original,entry-0xFF000,0x1400000A,0x03E00008)
        t=UpdateTrace(original);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)
        # JR31 must target this invocation's stop, even within the same body.
        struct.pack_into('<II',original,entry-0xFF000,0x03E00008,0)
        t=UpdateTrace(original);t.r[31]=entry+8
        with self.assertRaises(ValueError):t.run(entry)
        # An untaken ordinary branch at the last word still needs its delay.
        struct.pack_into('<I',original,entry-0xFF000,(4<<26)|11)
        struct.pack_into('<I',original,end-4-0xFF000,0x14000000)
        t=UpdateTrace(original);t.r[31]=RETURN
        with self.assertRaises(ValueError):t.run(entry)

        # A recursive caller can select a return address inside this same body.
        # Merely reaching that PC by fallthrough must not end the invocation.
        original=bytearray(end-0xFF000)
        struct.pack_into('<II',original,end-8-0xFF000,0x03E00008,0)
        t=UpdateTrace(original);t.r[31]=entry+8
        t.run(entry,entry+8)
        self.assertEqual(t.instruction_count,(end-entry)//4)

    def test_inherited_reserved_operands_and_vu_instructions_rejected(self):
        t=self.trace()
        for instruction in (0x00810809,0x00A1F809,0x00A0F849,0x0081086D,0x46010006,0x44850801,0x4BE000D3):
            with self.assertRaises(ValueError):t.execute(instruction,0)


if __name__=='__main__':unittest.main()
