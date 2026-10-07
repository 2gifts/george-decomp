import copy
from pathlib import Path
import struct,sys,unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
from trace_geometry import Trace
from trace_word_cursor import WordCursorTrace,BASE,END,RANGES,RETURN,fixtures,make_fixture

def controlled_image(routine,pc,word):
    """One synthetic guard word, not a replacement original body."""
    b=bytearray(RANGES[routine][1]-0xFF000)
    struct.pack_into('<I',b,pc-0xFF000,word)
    return bytes(b)

class CursorGuards(unittest.TestCase):
    def reject_unchanged(self,t,fn,message):
        state=copy.deepcopy(t.__dict__)
        with self.assertRaisesRegex(ValueError,message):fn()
        self.assertEqual(t.__dict__,state)
    def test_memory_alignment_ownership_and_initialization(self):
        t=WordCursorTrace(b'')
        for a,n in [(BASE-4,4),(END,4),(BASE+1,4),(BASE,8)]:
            self.reject_unchanged(t,lambda:t.load(a,n),'memory')
            self.reject_unchanged(t,lambda:t.save(a,1,n),'memory')
        self.reject_unchanged(t,lambda:t.load(BASE,4),'uninitialized')
        self.reject_unchanged(t,lambda:t.save(BASE,1,4),'uninitialized')
    def test_exact_PC_word_precedes_mutation(self):
        pc=RANGES[0][0];w=0x8C850008;t=WordCursorTrace(controlled_image(0,pc,w))
        for z,p in [(w^1,pc),(w,pc+1),(w,RANGES[0][1])]:
            self.reject_unchanged(t,lambda:t.execute(z,p),'PC/word')
    def test_reserved_operand_forms_precede_mutation(self):
        pc=RANGES[0][0];w=0x8C650008;t=WordCursorTrace(controlled_image(0,pc,w))
        self.reject_unchanged(t,lambda:t.execute(w,pc),'reserved')
        pc=RANGES[0][0]+4;w=0x00A51880;t=WordCursorTrace(controlled_image(0,pc,w))
        self.reject_unchanged(t,lambda:t.execute(w,pc),'reserved')
    def test_local_budget_and_memory_preflight(self):
        pc=RANGES[0][0];w=0x8C850008;t=WordCursorTrace(controlled_image(0,pc,w));t.r[4]=BASE
        t.instruction_count=8
        self.reject_unchanged(t,lambda:t.execute(w,pc),'budget')
        t.instruction_count=0
        self.reject_unchanged(t,lambda:t.execute(w,pc),'uninitialized')
        t.r[4]=BASE+1
        self.reject_unchanged(t,lambda:t.execute(w,pc),'unaligned')
    def test_register_definedness_and_return_target(self):
        pc=RANGES[0][0]+16;w=0x8C620010;t=WordCursorTrace(controlled_image(0,pc,w))
        self.reject_unchanged(t,lambda:t.execute(w,pc),'undefined')
        pc=RANGES[0][1]-8;w=0x03E00008;t=WordCursorTrace(controlled_image(0,pc,w));t.r[31]=RETURN-4
        self.reject_unchanged(t,lambda:t.execute(w,pc),'JR31')
    def test_control_in_delay_rejected_before_run_mutation(self):
        pc=RANGES[0][1]-4;t=WordCursorTrace(controlled_image(0,pc,0x10000001));t.r[31]=RETURN
        self.reject_unchanged(t,lambda:t.run(),'control transfer')
    def test_entry_and_fixture_domain(self):
        t=WordCursorTrace(b'')
        self.reject_unchanged(t,lambda:t.run(RANGES[1][0]),'entry/return')
        for n in [-1,3,True]:
            with self.assertRaisesRegex(ValueError,'entry'):WordCursorTrace(b'',n)
        for args in [(0,1<<32,0,0,0),(0,0,0,1,0)]:
            with self.assertRaisesRegex(ValueError,'fixture'):make_fixture(b'',*args)

class CursorOriginal(unittest.TestCase):
    def setUp(self):
        p=ROOT/'orig/SLUS_216.68'
        if not p.is_file():self.skipTest('private original absent')
        _,self.original=validated_elf(p)
    def test_load_before_commit_and_full_sign_result(self):
        for routine,cursor in [(0,0xFFFFFFFE),(1,0xFFFFFDFF),(2,0xFFFFFDFF)]:
            c=make_fixture(self.original,routine,cursor,0,8,7)
            self.assertEqual(c['result'],0xFFFFFFFF00000000|cursor)
            self.assertEqual([e[0] for e in c['events']],['load','load']+([] if routine==2 else ['store']))
        c=make_fixture(self.original,0,0,0x80000000,0,7)
        self.assertEqual(c['result'],0xFFFFFFFF80000000)
    def test_all_real_PCs_and_whole_arenas(self):
        cases=fixtures(self.original)
        self.assertEqual(len(cases),798)
        self.assertEqual(sum(c['instructions'] for c in cases),5040)
        self.assertEqual({p for c in cases for p in c['visited']},{p for a,b in RANGES for p in range(a,b,4)})

if __name__=='__main__':unittest.main()
