"""Fail-closed synthetic primitives; retail execution is optional private evidence."""
from pathlib import Path
import copy,hashlib,struct,sys,unittest
R=Path(__file__).resolve().parents[1];sys.path.insert(0,str(R/'tools'))
from trace_matrix_copy_identity import CopyIdentityTrace,BUFFER,END,LIMIT,MASKS,RANGES,observe
from analyze import validated_elf
from trace_geometry import RETURN

def synthetic():
    original=bytearray(0x2A1C60-0xFF000)
    words=dict((pc,s[0])for pc,s in MASKS.items())
    for i,(rt,offset)in enumerate(((7,48),(2,0),(3,16),(6,32))):words[0x2A1C08+4*i]=(0x1E<<26)|(5<<21)|(rt<<16)|offset
    for i,(rt,offset)in enumerate(((2,0),(3,16),(6,32),(7,48))):words[0x2A1C18+4*i]=(0x1F<<26)|(4<<21)|(rt<<16)|offset
    for pc,rt,offset in((0x2A1C3C,0,48),(0x2A1C4C,3,0),(0x2A1C50,2,16),(0x2A1C54,1,32)):words[pc]=(0x3E<<26)|(4<<21)|(rt<<16)|offset
    for pc in(0x2A1C28,0x2A1C58):words[pc]=0x03E00008;words[pc+4]=0
    for pc,w in words.items():struct.pack_into('<I',original,pc-0xFF000,w)
    t=CopyIdentityTrace(bytes(original));t.memory={BUFFER+i:i&255 for i in range(END-BUFFER)};t.r[4]=BUFFER+80;t.r[5]=BUFFER+64;t.r[31]=RETURN
    return t

def snapshot(t):return copy.deepcopy({k:v for k,v in t.__dict__.items()if k!='original'})

class MatrixCopyIdentityGuards(unittest.TestCase):
    def rejected_unchanged(self,t,w,pc):
        old=snapshot(t)
        with self.assertRaises(ValueError):t.execute(w,pc)
        self.assertEqual(snapshot(t),old)
    def test_masks_define_only_selected_unknown_lanes(self):
        t=synthetic()
        for pc in(0x2A1C30,0x2A1C34,0x2A1C38):t.execute(t.fetch(pc),pc)
        self.assertEqual(t.defined[3],[True,False,False,False]);self.assertEqual(t.vf[3],[0x3F800000,None,None,None])
        self.assertEqual(t.defined[2],[False,True,False,False]);self.assertEqual(t.defined[1],[False,False,True,False])
        for pc in(0x2A1C40,0x2A1C44,0x2A1C48):t.execute(t.fetch(pc),pc)
        self.assertEqual(t.vf[3],[0x3F800000,0,0,0]);self.assertTrue(all(t.defined[1]))
    def test_unknown_sources_and_constant_register_rejected_before_mutation(self):
        t=synthetic();self.rejected_unchanged(t,t.fetch(0x2A1C4C),0x2A1C4C)
        t=synthetic();t.vf[0][3]=0;self.rejected_unchanged(t,t.fetch(0x2A1C30),0x2A1C30)
        t=synthetic();t.gpr_defined.discard(7);self.rejected_unchanged(t,t.fetch(0x2A1C24),0x2A1C24)
    def test_alignment_unknown_and_outside_storage_fail(self):
        for pc in(0x2A1C08,0x2A1C3C):
            t=synthetic();t.r[5]=BUFFER+1;t.r[4]=BUFFER+1;self.rejected_unchanged(t,t.fetch(pc),pc)
            t=synthetic();t.r[5]=END;t.r[4]=END;self.rejected_unchanged(t,t.fetch(pc),pc)
            t=synthetic();t.memory.pop(BUFFER+(112 if pc==0x2A1C08 else 128));self.rejected_unchanged(t,t.fetch(pc),pc)
    def test_reserved_pair_word_PC_and_delay_control_rejected(self):
        t=synthetic();self.rejected_unchanged(t,t.fetch(0x2A1C30)^1,0x2A1C30)
        t=synthetic();self.rejected_unchanged(t,0x4B0000D3,0x2A1C08)
        t=synthetic();self.rejected_unchanged(t,0,0x2A1C60)
        t=synthetic();raw=bytearray(t.original);struct.pack_into('<I',raw,0x2A1C2C-0xFF000,0x10000000);t.original=bytes(raw)
        with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(0x2A1C08)
    def test_budget_failure_preserves_full_state(self):
        for pc in(0x2A1C08,0x2A1C30,0x2A1C3C):
            t=synthetic();t.instruction_count=LIMIT;self.rejected_unchanged(t,t.fetch(pc),pc)
    def test_raw_snapshot_overlap_and_whole_arena(self):
        for delta in(-48,-32,-16,0,16,32,48):
            t=synthetic();t.r[4]=BUFFER+64+delta;before=bytes(t.memory[BUFFER+i]for i in range(256));expected=bytearray(before);expected[64+delta:128+delta]=before[64:128]
            t.run(0x2A1C08);self.assertEqual(bytes(t.memory[BUFFER+i]for i in range(256)),bytes(expected));self.assertEqual(t.instruction_count,10)
    def test_whole_licensed_method_and_license_notice_retained(self):
        source=(R/'src/game/matrix_identity.c').read_bytes();span=source[source.index(b' void matrix_unit('):]
        self.assertEqual(len(span),200);self.assertEqual(hashlib.sha256(span).hexdigest(),'51cef7738d5b2cd7b5857a565d7d321cec0056b68d0f2eee621fea48e4782b5b')
        self.assertIn(b'(c) 2005 Naomi Peori',source);self.assertIn(b'Modified work:',source)
        self.assertEqual(hashlib.sha256((R/'LICENSES/PS2SDK-AFL-2.0.txt').read_bytes()).hexdigest(),'1ecee940922a6886baccddd9133d17f1ce677d32c5a954fac8e48224f2766fe8')
    @unittest.skipUnless((R/'orig/SLUS_216.68').is_file(),'private original ELF unavailable')
    def test_actual_original_full_arena_and_all22_words(self):
        _,raw=validated_elf(R/'orig/SLUS_216.68');cases,visited,total=observe(raw)
        self.assertEqual(len(cases),364);self.assertEqual(len(visited),22);self.assertEqual(total,3822)

if __name__=='__main__':unittest.main()
