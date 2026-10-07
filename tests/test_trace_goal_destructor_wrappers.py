"""Guard carriers; execution remains gated until actual prelink acceptance."""
import copy
import importlib.util
from pathlib import Path
import unittest
R=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('goal_forward_trace',R/'tools/trace_goal_destructor_wrappers.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
ForwardTrace,ENTRIES,ARENA,SP,MASK,RETURN=(getattr(module,k)for k in ('ForwardTrace','ENTRIES','ARENA','SP','MASK','RETURN'))
from analyze import validated_elf

ORIGINAL=Path.cwd()/'orig/SLUS_216.68'

@unittest.skipUnless(ORIGINAL.is_file(),'requires local hash-validated original')
class GoalForwardGuards(unittest.TestCase):
    @classmethod
    def setUpClass(cls): _,cls.original=validated_elf(ORIGINAL)

    def trace(self,budget=7):
        t=ForwardTrace(self.original,ENTRIES[0],budget)
        for a in range(ARENA,ARENA+128,4):t.save(a,0,4)
        for a in range(SP-16,SP,8):t.save(a,0,8)
        t.r[4],t.r[5],t.r[29],t.r[31]=ARENA,2,SP,RETURN
        t.stores.clear();return t

    def state(self,t):return copy.deepcopy((t.r,t.memory,t.events,t.stores,t.instruction_count,t.visited))
    def test_bad_entry(self):
        with self.assertRaises(ValueError):ForwardTrace(self.original,ENTRIES[0]+4)
    def test_changed_body(self):
        b=bytearray(self.original);b[ENTRIES[0]-0xFF000]^=1
        with self.assertRaises(ValueError):ForwardTrace(bytes(b),ENTRIES[0])
    def test_changed_word_prestate(self):
        t=self.trace();before=self.state(t)
        with self.assertRaises(ValueError):t.execute(t.expected[t.entry]^1,t.entry)
        self.assertEqual(before,self.state(t))
    def test_budget_prestate(self):
        t=self.trace(0);before=self.state(t)
        with self.assertRaises(ValueError):t.execute(t.expected[t.entry],t.entry)
        self.assertEqual(before,self.state(t))
    def test_helper_budget_prestate(self):
        t=self.trace(4);t.instruction_count=4;before=self.state(t)
        with self.assertRaises(ValueError):t.controlled_helper(0x20D2C0)
        self.assertEqual(before,self.state(t))
    def test_wrong_helper_prestate(self):
        t=self.trace();before=self.state(t)
        with self.assertRaises(ValueError):t.controlled_helper(0x20D2C4)
        self.assertEqual(before,self.state(t))
    def test_uninitialized_stack_prestate(self):
        t=self.trace();del t.memory[SP-16];t.r[29]=SP-16;before=self.state(t)
        with self.assertRaises(ValueError):t.execute(t.expected[t.entry+4],t.entry+4)
        self.assertEqual(before,self.state(t))
    def test_delay_control_prestate(self):
        t=self.trace();pc=t.entry+8;t.expected[pc+4]=0x10000000
        b=bytearray(t.original);b[pc+4-0xFF000:pc+8-0xFF000]=(0x10000000).to_bytes(4,'little');t.original=bytes(b)
        before=self.state(t)
        with self.assertRaises(ValueError):t.execute(t.expected[pc],pc)
        self.assertEqual(before,self.state(t))
    def test_owned_state_and_stack_prestate(self):
        t=self.trace();t.r[4]=ARENA+4;before=self.state(t)
        with self.assertRaises(ValueError):t.controlled_helper(0x20D2C0)
        self.assertEqual(before,self.state(t))

if __name__=='__main__':unittest.main()
