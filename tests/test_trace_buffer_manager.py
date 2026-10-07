import importlib.util,struct,sys,unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from trace_buffer_manager import (ManagerTrace,RANGES,BUFFER,CTYPE,RETURN,fixture,signed)
from analyze import validated_elf

class Synthetic(ManagerTrace):
    def __init__(self,instructions):
        super().__init__(b'');self.instructions=instructions;self.r[31]=RETURN
    def fetch(self,pc):
        if pc not in self.instructions:raise ValueError('missing synthetic instruction')
        return self.instructions[pc]

class ManagerGuards(unittest.TestCase):
    def test_owned_code_and_whole_entry(self):
        t=ManagerTrace(b'')
        for pc in (RANGES[0][0]+1,RANGES[0][1],0x100000):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(RANGES[0][0]+4)
        with self.assertRaises(ValueError):t.run(RANGES[0][0],depth=5)
    def test_initialized_aligned_bounded_memory(self):
        t=ManagerTrace(b'')
        for address,size in ((BUFFER,4),(BUFFER+1,4),(BUFFER-1,1),(BUFFER,2)):
            with self.assertRaises(ValueError):t.load(address,size)
        t.save(BUFFER,0x12345678,4);self.assertEqual(t.load(BUFFER,4),0x12345678)
        with self.assertRaises(ValueError):t.save(0x81000,0,1)
    def test_ctype_known_domain_and_whole_identity(self):
        t=ManagerTrace(b'')
        with self.assertRaises(ValueError):t.load(CTYPE-1,1)
        with self.assertRaises(ValueError):t.load(CTYPE,4)
        with self.assertRaises(ValueError):t.load(CTYPE,1)
        with self.assertRaises(ValueError):fixture(b'',character=-2)
    def test_reserved_forms_and_bulk_rejected(self):
        t=ManagerTrace(b'');pc=RANGES[0][0]
        # Reserved LUI rs, shifted ADDU, nonzero JALR rt/rd, REGIMM trap,
        # unsupported packed-byte compare/MMI and COP1 all fail closed.
        for word in ((15<<26)|(1<<21), (4<<21)|(5<<16)|(2<<11)|(1<<6)|33,
                     (2<<21)|(1<<16)|(31<<11)|9,(2<<21)|(30<<11)|9,
                     (1<<26)|(8<<16),0x70000028,0x46000064):
            with self.assertRaises(ValueError):t.execute(word,pc)
    def test_signed_low_word_byte_shift_and_mult(self):
        t=ManagerTrace(b'');t.save(BUFFER,0x80,1);t.r[4]=BUFFER
        t.execute((32<<26)|(4<<21)|(2<<16),RANGES[0][0]);self.assertEqual(t.r[2],0xFFFFFF80)
        t.execute((2<<21)|(3<<11)|45,RANGES[0][0]);self.assertEqual(t.r[3],t.r[2])
        t.r[2]=0x80000000;t.execute((2<<16)|(3<<11)|(24<<6)|3,RANGES[0][0]);self.assertEqual(t.r[3],0xFFFFFF80)
        t.r[5]=0x7FFFFFFF;t.r[2]=8
        t.execute((5<<21)|(2<<16)|(2<<11)|24,0x2AAF74);self.assertEqual(t.r[2],0xFFFFFFF8)
        self.assertEqual(signed(0xFFFFFFFF),-1)
    def test_untaken_delays_reject_all_encoded_controls(self):
        entry=RANGES[2][0]
        delay_words=((4<<26),(1<<26)|(1<<16),(17<<26)|(8<<21),(2<<26),0x03E00008)
        for take in (False,True):
            for delay in delay_words:
                # Outer ordinary BEQ has a delay under both outcomes.
                t=Synthetic({entry:(4<<26)|(2<<21)|(3<<16)|1,entry+4:delay,entry+8:0x03E00008,entry+12:0})
                t.r[2]=0;t.r[3]=0 if take else 1
                with self.assertRaisesRegex(ValueError,'control in delay'):t.run(entry)
    def test_likely_annul_and_actual_jr_stop(self):
        entry=RANGES[2][0]
        t=Synthetic({entry:(20<<26)|(2<<21)|(3<<16)|1,entry+4:0x03E00008,entry+8:0x03E00008,entry+12:0})
        t.r[2]=0;t.r[3]=1;t.run(entry);self.assertNotIn(entry+4,t.visited)
        t=Synthetic({entry:0x03E00008,entry+4:0});t.r[31]=entry+8;t.run(entry,stop=entry+8)
        t=Synthetic({entry:0x03E00008,entry+4:0});t.r[31]=entry+8
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(entry)
        # A JAL to the chosen external stop is not an actual JR31 return.
        t=Synthetic({entry:(3<<26)|((RETURN>>2)&0x3FFFFFF),entry+4:0})
        with self.assertRaises(ValueError):t.run(entry)
    def test_copy_contract_callback_and_instruction_bounds(self):
        t=ManagerTrace(b'');t.r[4]=BUFFER;t.r[5]=BUFFER+16;t.r[6]=16
        with self.assertRaisesRegex(ValueError,'bulk'):t.run(RANGES[7][0])
        t.r[4]=BUFFER+1;t.r[5]=BUFFER+3;t.r[6]=4
        with self.assertRaisesRegex(ValueError,'overlapping'):t.run(RANGES[7][0])
        with self.assertRaises(ValueError):t.callback(0xF0000030)
        t.instruction_count=12000
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,RANGES[0][0])
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'private original ELF not present')
    def test_actual_alias_reload_and_known_ctype_originals(self):
        _,original=validated_elf(ROOT/'orig/SLUS_216.68')
        for alias,routine,text in ((1,0,b''),(1,1,b'\n'),(2,0,b'go'),(3,0,b'a'),(4,0,b'a'),(5,1,b'abc'),(6,1,b'\n')):
            result=fixture(original,routine,character=10 if alias in (1,2,6) else 65,text=text,alias=alias)
            self.assertGreater(result['instructions'],0)
        for character in (-1,0,65,127):
            self.assertGreater(fixture(original,character=character)['instructions'],0)

if __name__=='__main__':unittest.main()
