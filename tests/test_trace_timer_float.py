"""Bounded exact COP1 guards; proprietary originals are optional public tests."""
import sys,unittest
from pathlib import Path
from fractions import Fraction
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from trace_timer_float import (FloatTimerTrace,fixture,SELECTED,COP1,encode_exact,
 decode_exact,BASE,RATE,RATEGLOBAL,TIMER,RETURN,sx32)

def parameters():
    return dict(routine=0,lazy=0,count=2,spare=2,current=4096,previous=64,reset=0,
        divider=256,mutation=0,timer_alias=0,cache=0,exit_count=0,exit_failure=0)
def snapshot(t):
    return dict(memory=dict(t.memory),r=list(t.r),fraw=list(t.fraw),steps=list(t.cop1_steps),
        events=list(t.events),visited=set(t.visited),count=t.instruction_count,reads=t.counter_reads)

class FloatTimerGuards(unittest.TestCase):
    def test_raw_encoding_and_pc_fail_before_mutation(self):
        for pc,w in ((0x2BD898,0x44820000),(0x2BD894,0x44820800),(0x2BD900,0x46010000),(0x2BD900,0x44420000)):
            t=FloatTimerTrace(b'',parameters());t.r[2]=1024;t.r[0]=77;before=snapshot(t)
            with self.assertRaisesRegex(ValueError,'unapproved'):t.execute(w,pc)
            self.assertEqual(before,snapshot(t))
    def test_exact_values_and_raw_integer_then_float_state(self):
        t=FloatTimerTrace(b'',parameters());t.r[2]=1024;t.r[0]=77
        t.execute(COP1[0x2BD894],0x2BD894);self.assertEqual(t.fraw[0],1024)
        t.execute(COP1[0x2BD89C],0x2BD89C);self.assertEqual(t.fraw[0],0x44800000)
        t.r[4]=256;t.execute(COP1[0x2BD8CC],0x2BD8CC);t.execute(COP1[0x2BD8D4],0x2BD8D4)
        t.execute(COP1[0x2BD900],0x2BD900)
        self.assertEqual(t.fraw[0],0x40800000);self.assertEqual(t.r[0],0)
        self.assertEqual(t.instruction_count,5);self.assertEqual(len(t.visited),5)
    def test_unsupported_inputs_and_uninitialized_fpr_preserve_state(self):
        for raw in (0x1000001,0x7fffffff,0x80000000,0xffffffff):
            t=FloatTimerTrace(b'',parameters());t.r[2]=raw;before=snapshot(t)
            with self.assertRaises(ValueError):t.execute(COP1[0x2BD894],0x2BD894)
            self.assertEqual(before,snapshot(t))
        for raw in (0,3,0x1193ff10):
            t=FloatTimerTrace(b'',parameters());t.r[4]=raw;before=snapshot(t)
            with self.assertRaises(ValueError):t.execute(COP1[0x2BD8CC],0x2BD8CC)
            self.assertEqual(before,snapshot(t))
        t=FloatTimerTrace(b'',parameters());before=snapshot(t)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.execute(COP1[0x2BD900],0x2BD900)
        self.assertEqual(before,snapshot(t))
    def test_budget_precedes_cop1_and_count_callbacks(self):
        t=FloatTimerTrace(b'',parameters());t.r[2]=1024;t.instruction_count=30000;before=snapshot(t)
        with self.assertRaisesRegex(ValueError,'budget before COP1'):t.execute(COP1[0x2BD894],0x2BD894)
        self.assertEqual(before,snapshot(t))
        t.p['mutation']=14;t.save(TIMER,64,4);t.save(RATEGLOBAL,RATE,4);t.save(RATE+4,256,4);before=snapshot(t)
        with self.assertRaisesRegex(ValueError,'before Count'):t.execute(0x40024800,0x2BD720)
        self.assertEqual(before,snapshot(t))
    def test_count_exact_site_signed_low64_once_and_r0(self):
        t=FloatTimerTrace(b'',parameters());t.p['current']=0x80000000;t.r[0]=77
        with self.assertRaises(ValueError):t.execute(0x40024800,0x2BD4C8)
        t.execute(0x40024800,0x2BD720)
        self.assertEqual(t.r[2],sx32(0x80000000));self.assertEqual(t.r[0],0)
        self.assertEqual(t.counter_reads,1);self.assertEqual(t.instruction_count,1)
        before=snapshot(t)
        with self.assertRaisesRegex(ValueError,'Count count'):t.execute(0x40024800,0x2BD720)
        self.assertEqual(before,snapshot(t))
        self.assertEqual(len(t.events),1)
    def test_incoming_u32_upper_shape_rejected_before_invocation(self):
        t=FloatTimerTrace(b'',parameters());t.r[5]=0x80000000;before=snapshot(t)
        with self.assertRaisesRegex(ValueError,'signextended'):t.run(SELECTED[1][0])
        self.assertEqual(before,snapshot(t));self.assertEqual(sum(t.invocations),0)
    def test_memory_and_delay_control_guards_are_retained(self):
        t=FloatTimerTrace(b'',parameters())
        for a,n in ((0,4),(BASE+1,4),(BASE,3),(RATEGLOBAL,8)):
            with self.assertRaises(ValueError):t.save(a,0,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
        class Synthetic(FloatTimerTrace):
            def fetch(self,pc):
                entry=SELECTED[0][0]
                return {entry:0x10800001,entry+4:self.delay,entry+8:0x03E00008,entry+12:0}[pc]
        for delay in (0x14A00001,0x04A10001,0x45000001,0x08000000,0x03E00008):
            t=Synthetic(b'',parameters());t.delay=delay;t.r[4]=1;t.r[5]=0;t.r[31]=RETURN
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(SELECTED[0][0])
    def test_observer_only_domain_and_explicit_exact_boundaries(self):
        for kwargs in (dict(lazy=1),dict(divider=0),dict(divider=3),dict(routine=1,previous=0xffffffff),dict(routine=1,previous=0x1000001)):
            with self.assertRaises(ValueError):fixture(b'',**kwargs)
        for rational,raw in ((Fraction(0),0),(Fraction(1,1<<31),0x30000000),(Fraction(1<<31),0x4f000000),(Fraction(0xffffff00),0x4f7fffff)):
            self.assertEqual(encode_exact(rational),raw);self.assertEqual(decode_exact(raw),rational)
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'local original required')
    def test_original_capture_fresh_mutation_and_rate_word_alias(self):
        original=(ROOT/'orig/SLUS_216.68').read_bytes()
        a=fixture(original,current=4096,previous=64,divider=256,mutation=4)
        self.assertEqual(a['denominator'],256);self.assertEqual(a['globals_expected'][0],0x20140)
        b=fixture(original,current=4096,previous=64,divider=256,mutation=8)
        self.assertEqual(b['denominator'],0x40000000)
        c=fixture(original,current=1024,previous=256,divider=256,timer_alias=1,reset=1)
        self.assertEqual(c['numerator'],768);self.assertEqual(c['denominator'],1024)
        lazy=fixture(original,lazy=1,mutation=1)
        self.assertEqual(lazy['denominator'],0x80000000)

if __name__=='__main__':unittest.main()
