"""Strict route entry/ABI/alias guards; game-dependent tests skip absent orig."""
from pathlib import Path
import struct
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_actor_route as m
from analyze import validated_elf
from trace_geometry import RETURN
LOCAL=(m.ROOT/'orig/SLUS_216.68').is_file()

class RouteGuards(unittest.TestCase):
    def trace(self,code=()):
        raw=bytearray(0x37AFD4-0xFF000)
        for i,w in enumerate(code):struct.pack_into('<I',raw,m.ENTRIES[0]-0xFF000+i*4,w)
        return m.RouteTrace(bytes(raw))
    def test_whole_entries_bound_and_padding(self):
        t=self.trace()
        for a in (m.ENTRIES[0]-4,m.ENTRIES[0]+1,0x1CC68C,0x3747E0,0x37AFD4):
            with self.assertRaises(ValueError):t.fetch(a)
        with self.assertRaises(ValueError):t.run(m.ENTRIES[0]+4)
        with self.assertRaises(ValueError):m.RouteTrace(b'').fetch(m.ENTRIES[0])
        t.frames=[0]*20
        with self.assertRaises(ValueError):t.run(m.ENTRIES[0])
        t.instruction_count=150000
        with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,m.ENTRIES[0])
    def test_memory_unknown_and_alignment(self):
        t=self.trace()
        for a,n in ((m.BUFFER+1,4),(m.END,4),(m.BUFFER,3)):
            with self.assertRaises(ValueError):t.save(a,0,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(0x7FFFF,1)
        t.save(0x7FFFF,2,1);self.assertEqual(t.load(0x7FFFF,1),2)
        with self.assertRaises(ValueError):t.load(0x7FFFE,2)
    def test_delay_control_all_outcomes(self):
        delays=((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(4<<21)|(1<<16)|1,
                (17<<26)|(8<<21)|1,3<<26,0x03E00008)
        for equal in (False,True):
            for delay in delays:
                t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay))
                t.r[4],t.r[5]=1,1 if equal else 2
                with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(m.ENTRIES[0])
        t=self.trace((0x03E00008,delays[0]));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(m.ENTRIES[0])
    def test_only_actual_jr31_to_selected_stop(self):
        t=self.trace(((2<<21)|8,0));t.r[2]=RETURN
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(m.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=m.ENTRIES[0]+8
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(m.ENTRIES[0])
        t=self.trace((0x03E00008,0));t.r[31]=m.ENTRIES[0]+8
        t.run(m.ENTRIES[0],m.ENTRIES[0]+8)
        self.assertEqual(t.instruction_count,2)
    def test_reserved_forms_and_no_vu_expansion(self):
        t=self.trace();pc=m.ENTRIES[0]
        for w in (0x4BE0003C,0x70000000,(15<<26)|(1<<21),
                  (17<<26)|(4<<21)|1,(17<<26)|(16<<21)|(1<<6)|0x34,
                  (4<<21)|(2<<16)|(31<<11)|(1<<6)|9):
            with self.assertRaises(ValueError):t.execute(w,pc)
    def test_controlled_one_gpr_receiver(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.library_call(0x12345678)
        t.r[4]=m.VOBJECT+4
        with self.assertRaisesRegex(ValueError,'one-GPR'):t.library_call(m.FRAME_CALL)
        with self.assertRaisesRegex(ValueError,'numeric position'):t.library_call(0x120B68)
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_tag_snapshots_vs_fresh_setup(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        t,args=m.make_scene(raw,routine=1,tag_alias=1);t.run(m.ENTRIES[1])
        self.assertEqual(t.load(m.STATE+0x48,4),0)
        self.assertEqual(t.load(m.STATE+0x4C,4),1)
        self.assertEqual(t.events,[3,m.SETUP,m.STATE+0x14,5,1,1,m.POINT,m.STATE+0x114])
        t,args=m.make_scene(raw,routine=1,tag_alias=2);t.run(m.ENTRIES[1])
        self.assertEqual(t.events[4:6],[0,0])
        for which in (1,2):
            t,args=m.make_scene(raw,routine=1,query_failure=which);t.run(m.ENTRIES[1])
            self.assertEqual(t.load(m.STATE+0x3E,1),2)
            self.assertEqual(t.events,[])
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_callback_captured_normal_and_global(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        t,args=m.make_scene(raw,mutation=2,adjustment=-4);t.run(m.ENTRIES[0])
        self.assertEqual(t.calls,[1,0,0]);self.assertEqual(t.load(m.ROADS+0x48,4),m.POSITIONS+12)
        self.assertEqual(t.load(m.ACTOR+0x55,1),1)
        t,args=m.make_scene(raw,mutation=5);t.run(m.ENTRIES[0])
        self.assertEqual(t.load(m.GLOBAL,4),m.OTHER_MANAGER)
    @unittest.skipUnless(LOCAL,'requires locally supplied original ELF')
    def test_original_queue_count_before_marker_and_fresh_count(self):
        _,raw=validated_elf(m.ROOT/'orig/SLUS_216.68')
        t,args=m.make_scene(raw,routine=2,count=30,marker=1);t.run(m.ENTRIES[2])
        self.assertEqual(t.r[2],0);self.assertEqual(t.load(m.MANAGER+1,1),30)
        t,args=m.make_scene(raw,routine=2,count=0,queue_alias=True);t.run(m.ENTRIES[2])
        self.assertEqual(t.r[2],1);self.assertEqual(t.load(m.MANAGER+1,1),2)
        self.assertEqual(t.load(m.MANAGER+0x70,4),m.MANAGER-2)
if __name__=='__main__':unittest.main()
