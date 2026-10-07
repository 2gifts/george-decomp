import copy
from pathlib import Path
import struct,sys,unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
from trace_geometry import Trace, RETURN
from trace_resource_pointer import ResourcePointerTrace,BASE,WORDS,RANGES,parameters,fixture

def controlled_image(routine,pc,word):
    """Synthetic rejection word only; no replacement original execution."""
    b=bytearray(RANGES[routine][1]-0xFF000)
    struct.pack_into('<I',b,pc-0xFF000,word)
    return bytes(b)

class ResourceGuards(unittest.TestCase):
    def reject_unchanged(self,t,fn,message):
        before=copy.deepcopy(t.__dict__)
        with self.assertRaisesRegex(ValueError,message):fn()
        self.assertEqual(t.__dict__,before)
    def test_memory_alignment_ownership_initialization(self):
        t=ResourcePointerTrace(b'')
        for a,n in ((BASE-4,4),(BASE+WORDS*4,4),(BASE+1,2),(BASE,8)):
            self.reject_unchanged(t,lambda:t.load(a,n),'memory')
            self.reject_unchanged(t,lambda:t.save(a,1,n),'memory')
        self.reject_unchanged(t,lambda:t.load(BASE,4),'uninitialized')
        self.reject_unchanged(t,lambda:t.save(BASE,1,4),'uninitialized')
    def test_exact_word_PC_and_local_budget(self):
        pc=RANGES[0][0];word=0x8C850008;t=ResourcePointerTrace(controlled_image(0,pc,word))
        for w,p in ((word^1,pc),(word,pc+1),(word,RANGES[0][1])):
            self.reject_unchanged(t,lambda:t.execute(w,p),'PC/word')
        t.instruction_count=1200
        self.reject_unchanged(t,lambda:t.execute(word,pc),'budget')
    def test_reserved_family_and_operand_forms(self):
        pc=RANGES[0][0]
        for word in (0x0000082D|0x40,0x3C240003,0x0000000D,0x46000000):
            t=ResourcePointerTrace(controlled_image(0,pc,word))
            self.reject_unchanged(t,lambda:t.execute(word,pc),'reserved')
    def test_register_definedness_lower64_and_memory_preflight(self):
        pc=RANGES[0][0];word=0x8C850008;t=ResourcePointerTrace(controlled_image(0,pc,word))
        t.defined.remove(4)
        self.reject_unchanged(t,lambda:t.execute(word,pc),'undefined')
        t.defined.add(4);t.r[4]=1<<64
        self.reject_unchanged(t,lambda:t.execute(word,pc),'lower64')
        t.r[4]=BASE
        self.reject_unchanged(t,lambda:t.execute(word,pc),'uninitialized')
        t.r[4]=BASE+1
        self.reject_unchanged(t,lambda:t.execute(word,pc),'unaligned')
    def test_unreviewed_JR_branch_and_delay(self):
        pc=RANGES[0][1]-8;word=0x03E00008;t=ResourcePointerTrace(controlled_image(0,pc,word))
        t.r[31]=RETURN-4
        self.reject_unchanged(t,lambda:t.execute(word,pc),'JR31')
        pc=RANGES[0][0];word=0x10007FFF;t=ResourcePointerTrace(controlled_image(0,pc,word))
        self.reject_unchanged(t,lambda:t.execute(word,pc),'branch target')
        pc=RANGES[0][1]-4;t=ResourcePointerTrace(controlled_image(0,pc,0x10000001));t.r[31]=RETURN
        self.reject_unchanged(t,lambda:t.run(),'control transfer')
    def test_entry_schema_and_owned_extent(self):
        t=ResourcePointerTrace(b'')
        self.reject_unchanged(t,lambda:t.run(RANGES[1][0]),'entry/return')
        for r in (-1,4,True):
            with self.assertRaisesRegex(ValueError,'entry'):ResourcePointerTrace(b'',r)
        with self.assertRaisesRegex(ValueError,'storage'):ResourcePointerTrace(b'',0,WORDS+1)
        with self.assertRaisesRegex(ValueError,'schema'):fixture(b'',{})
    def test_store_preflight_preserves_complete_state(self):
        pc=RANGES[1][0]+8;word=0xAC830004;t=ResourcePointerTrace(controlled_image(1,pc,word),1)
        t.defined.add(3);t.r[3]=0xABC;t.r[4]=BASE
        self.reject_unchanged(t,lambda:t.execute(word,pc),'uninitialized')
        Trace.save(t,BASE+4,0,4);t.r[4]=BASE+1
        self.reject_unchanged(t,lambda:t.execute(word,pc),'unaligned')

class ResourceOriginal(unittest.TestCase):
    def setUp(self):
        p=ROOT/'orig/SLUS_216.68'
        if not p.is_file():self.skipTest('private original absent')
        _,self.original=validated_elf(p)
    def test_count_field_and_fresh_slot_aliases(self):
        cases=parameters()
        samples=[next(p for p in cases if p['routine']==2 and p['flavor']==1),
                 next(p for p in cases if p['routine']==3 and p['flavor']==1 and p['mode']==1 and p['index']==1),
                 next(p for p in cases if p['routine']==3 and p['flavor']==2 and p['mode']==1 and p['index']==1)]
        for p in samples:
            c=fixture(self.original,p)
            self.assertGreater(len(c['changes']),0)
            self.assertTrue(any(e[0]=='store' for e in c['events']))
    def test_full64_mode_and_nonBoolean_nochange(self):
        cases=parameters()
        high=next(p for p in cases if p['routine']==3 and p['flavor']==0 and p['mode']==0x100000000 and p['index']==0)
        c=fixture(self.original,high)
        self.assertEqual((c['expected'][high['owner_word']]>>24)&255,1)
        nochange=next(p for p in cases if p['routine']==3 and p['flavor']==0 and p['mode']==1 and p['index']==1)
        self.assertEqual(fixture(self.original,nochange)['source_result'],3)
if __name__=='__main__':unittest.main()
