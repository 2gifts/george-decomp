"""Strict operand, dispatch, delay, memory and genuine-route alias regressions."""
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_actor_route_init as m
from analyze import validated_elf
from trace_geometry import RETURN
LOCAL=(m.ROOT/'orig/SLUS_216.68').is_file()

class InitGuards(unittest.TestCase):
    def trace(self,code=()):
        raw=bytearray(m.TABLE_ADDRESS+m.TABLE_SIZE-0xFF000)
        for i,w in enumerate(code):struct.pack_into('<I',raw,m.ENTRIES[0]-0xFF000+i*4,w)
        return m.InitTrace(bytes(raw))
    def test_owned_entries_padding_and_limits(self):
        t=self.trace()
        for a in (m.ENTRIES[0]-4,m.ENTRIES[0]+1,0x1CD724,0x1DB3DC,0x20D1CC):
            with self.assertRaises(ValueError):t.fetch(a)
        with self.assertRaises(ValueError):t.run(m.ENTRIES[0]+4)
        with self.assertRaises(ValueError):m.InitTrace(b'').fetch(m.ENTRIES[0])
        t.frames=[0]*20
        with self.assertRaises(ValueError):t.run(m.ENTRIES[0])
        t.instruction_count=150000
        with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,m.ENTRIES[0])
    def test_memory_and_readonly_dispatch(self):
        t=self.trace()
        for a,n in ((m.BUFFER+1,4),(m.END,4),(m.BUFFER,3),(m.TABLE_ADDRESS,4)):
            with self.assertRaises(ValueError):t.save(a,0,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(0x7FFFF,1)
        t.save(0x7FFFF,2,1);self.assertEqual(t.load(0x7FFFF,1),2)
        with self.assertRaises(ValueError):t.load(0x7FFFE,2)
        for a,n in ((m.TABLE_ADDRESS+1,4),(m.TABLE_ADDRESS,8)):
            with self.assertRaisesRegex(ValueError,'geometry'):t.load(a,n)
        with self.assertRaisesRegex(ValueError,'target'):t.load(m.TABLE_ADDRESS,4)
    def test_delay_control_both_outcomes(self):
        delays=((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(4<<21)|(1<<16)|1,
                (17<<26)|(8<<21)|1,3<<26,0x03E00008)
        for equal in (False,True):
            for delay in delays:
                t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay))
                t.r[4],t.r[5]=1,1 if equal else 2
                with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(m.ENTRIES[0])
    def test_actual_return_and_computed_jump(self):
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'computed jump'):t.run(m.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=m.ENTRIES[0]+8
        with self.assertRaisesRegex(ValueError,'JR31 stop'):t.run(m.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=m.ENTRIES[0]+8
        t.run(m.ENTRIES[0],m.ENTRIES[0]+8)
        self.assertEqual(t.instruction_count,2)
    def test_strict_operands_and_ee_sqrt_ft(self):
        t=self.trace();pc=m.ENTRIES[0]
        for w in (0x4BE0003C,0x70000000,(15<<26)|(1<<21),
                  (17<<26)|(4<<21)|1,(17<<26)|(16<<21)|(1<<6)|0x34,
                  (4<<21)|(2<<16)|(31<<11)|(1<<6)|9,
                  (17<<26)|(16<<21)|(1<<11)|4,0x46010844):
            with self.assertRaises(ValueError):t.execute(w,pc)
        t.f[0]=100;t.f[1]=9
        t.execute(0x46010044,pc);self.assertEqual(t.f[1],3)
        t.f[1]=-1
        with self.assertRaises(ValueError):t.execute(0x46010044,pc)
    def test_64bit_flags_and_movement_subset(self):
        t=self.trace();pc=m.ENTRIES[0]
        t.execute((9<<26)|(5<<16)|0xEFFF,pc)
        self.assertEqual(t.r[5],0xFFFFFFFFFFFFEFFF)
        t.r[3]=0x80001000
        t.execute((3<<21)|(5<<16)|(3<<11)|36,pc)
        self.assertEqual(t.r[3],0x80000000)
        t.r[3]=0x1020304050607080
        t.execute((3<<21)|(5<<16)|(3<<11)|36,pc)
        self.assertEqual(t.r[3],0x1020304050606080)
        t.save(m.ENTITY+0x438,0,4);t.save(m.ENTITY+0xC,31,4);t.r[4]=m.ENTITY
        with self.assertRaisesRegex(ValueError,'movement path'):t.run(0x177E48)
    def test_controlled_contracts_fail_closed(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.library_call(0x12345678)
        t.r[4]=m.VOBJECT+4
        with self.assertRaisesRegex(ValueError,'one-GPR'):t.library_call(m.POSITION_CALL)
        with self.assertRaisesRegex(ValueError,'seven-GPR'):t.run(0x20BEA8)
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_query_publication_ties_and_threshold(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        t,args,_=m.scene(raw,alias=1);t.run(m.ENTRIES[0])
        self.assertEqual(t.r[2],0)
        self.assertEqual(t.load(m.HEADER,4),m.RECORDS+32)
        self.assertEqual(t.load(m.OUTPUT,4),(m.RECORDS+32)|1)
        t,args,_=m.scene(raw,centers=((3,0,0),(-3,0,0),(3,0,0)));t.run(m.ENTRIES[0])
        self.assertEqual(t.load(m.OUTPUT+4,4),m.RECORDS)
        t,args,_=m.scene(raw,centers=((1e9,0,0),)*3);t.run(m.ENTRIES[0])
        self.assertEqual(t.load(m.OUTPUT+4,4),0x5A5A5A5A)
        self.assertEqual(t.load(m.OUTPUT,4),0xFFFFFFFF)
        t,args,_=m.scene(raw,alias=4,kind=0,plane_count=1,inside=True);t.run(m.ENTRIES[0])
        self.assertEqual(t.r[2],1)
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_reused_movement_fresh_setup_and_queue(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        t,args,_=m.scene(raw,routine=1,flag_bits=0x80001000);t.run(m.ENTRIES[1])
        self.assertEqual(t.load(m.ENTITY+0x190,8),0x80000000)
        self.assertEqual(t.invocations[7:10],[1,1,1])
        t,args,_=m.scene(raw,routine=1,setup_alias=True);t.run(m.ENTRIES[1])
        self.assertEqual(t.load(m.STATE+0x10,4),m.STATE+0x14)
        self.assertEqual(t.load(m.MANAGER+0x6C,4),m.STATE+0x14)
        t,args,_=m.scene(raw,routine=1,owner_alias=2);t.run(m.ENTRIES[1])
        self.assertEqual(t.events[4],m.word(4.0))
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_completion_output_aliases(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        for alias in (0,1,2):
            t,args,_=m.scene(raw,routine=2,flags=4,step=7,output_alias=alias)
            t.run(m.ENTRIES[2])
            self.assertEqual(t.r[2],0 if alias==1 else 1)
            self.assertEqual(t.load(args[1],1),255 if alias else 7)
        t,args,_=m.scene(raw,routine=0,count=0);t.run(m.ENTRIES[0])
        self.assertEqual(t.load(m.OUTPUT+4,4),0x5A5A5A5A)

if __name__=='__main__':unittest.main()
