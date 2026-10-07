"""Strict Count/memory/control guards; no original payload in public tests."""
import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from trace_timer_registry import TimerTrace,fixture,BASE,RATE,RATEGLOBAL,TIMER,SELECTED,MASK
from trace_geometry import RETURN
from trace_string_registry import sx32

def parameters():
    return dict(routine=1,lazy=0,count=2,spare=2,current=0x80000001,previous=17,reset=0,divider=17,mutation=0,timer_alias=0,cache=0,exit_count=0,exit_failure=0)

class TimerGuards(unittest.TestCase):
    def test_memory_ownership_alignment_and_width(self):
        t=TimerTrace(b'',parameters())
        for a,n in ((0,4),(BASE+1,4),(BASE,3),(RATEGLOBAL,8)):
            with self.assertRaises(ValueError):t.save(a,0,n)
    def test_initialized_reads_only(self):
        t=TimerTrace(b'',parameters())
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
        t.save(BASE,0x12345678,4);self.assertEqual(t.load(BASE,4),0x12345678)
    def test_reviewed_code_only(self):
        t=TimerTrace(b'',parameters())
        for pc in (0,SELECTED[0][0]+1,SELECTED[0][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
    def test_exact_count_encoding_and_location(self):
        t=TimerTrace(b'',parameters())
        for w,pc in ((0x40034800,0x2BD720),(0x40025000,0x2BD720),(0x40024801,0x2BD720),(0x40024800,0x2BD724)):
            with self.assertRaises(ValueError):t.execute(w,pc)
        self.assertEqual(t.counter_reads,0)
    def test_count_once_signed_low64_and_zero_register(self):
        t=TimerTrace(b'',parameters());t.r[0]=77
        self.assertEqual(t.execute(0x40024800,0x2BD720),(None,False))
        self.assertEqual(t.r[2],sx32(0x80000001));self.assertEqual(t.r[0],0)
        self.assertEqual(t.counter_reads,1);self.assertIn(0x2BD720,t.visited)
        with self.assertRaisesRegex(ValueError,'Count count'):t.execute(0x40024800,0x2BD720)
        self.assertEqual(len(t.events),1)
    def test_budget_precedes_count_callbacks_and_mutations(self):
        p=parameters();p['mutation']=14;t=TimerTrace(b'',p);t.instruction_count=30000
        t.save(TIMER,17,4);t.save(RATEGLOBAL,RATE,4);t.save(RATE+8,17,4)
        before=(dict(t.memory),list(t.r),list(t.events),set(t.visited),t.counter_reads)
        with self.assertRaisesRegex(ValueError,'bound before Count'):t.execute(0x40024800,0x2BD720)
        self.assertEqual(before,(t.memory,t.r,t.events,t.visited,t.counter_reads))
    def test_original_division_zero_and_break_fail_closed(self):
        t=TimerTrace(b'',parameters());t.r[2]=10;t.r[3]=0
        with self.assertRaisesRegex(ValueError,'division by zero'):t.execute(0x0043001B,0x2BDA48)
        with self.assertRaisesRegex(ValueError,'BREAK'):t.execute(0x000001CD,0x2BDA50)
    def test_delay_control_rejected_even_if_untaken(self):
        class Synthetic(TimerTrace):
            def fetch(self,pc):
                entry=SELECTED[0][0]
                return {entry:0x10800001,entry+4:self.delay,entry+8:0x03E00008,entry+12:0}[pc]
        for delay in (0x14A00001,0x04A10001,0x45000001,0x08000000,0x03E00008):
            t=Synthetic(b'',parameters());t.delay=delay;t.r[4]=1;t.r[5]=0;t.r[31]=RETURN
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(SELECTED[0][0])
    def test_native_domain_rejects_zero_divider_and_bad_counts(self):
        for kwargs in (dict(divider=0),dict(count=13),dict(mutation=16),dict(routine=2,timer_alias=1,reset=1,current=0)):
            with self.assertRaises(ValueError):fixture(b'',**kwargs)

if __name__=='__main__':unittest.main()
