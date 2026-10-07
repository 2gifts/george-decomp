"""Bounded decoder/source-domain regressions; original fixture read is optional."""
from pathlib import Path
import copy,struct,sys,unittest
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_geometry import RETURN
from trace_gnu_exception_queries import QueryTrace,initialize,parameters,fixture,ENTRIES,RECORDS,PROVIDER,CONTEXTS
from analyze import validated_elf

class QueryGuards(unittest.TestCase):
    def trace(self):
        p=parameters()[0];t=QueryTrace(bytes(0x280000),p);initialize(t,p);return t
    def test_budget_precedes_every_register_memory_coverage_change(self):
        t=self.trace();t.instruction_count=96;before=copy.deepcopy((t.r,t.memory,t.visited,t.events,t.instruction_count))
        with self.assertRaisesRegex(ValueError,'budget'):t.execute(0x24420008,ENTRIES[0])
        self.assertEqual(before,(t.r,t.memory,t.visited,t.events,t.instruction_count))
    def test_provider_budget_precedes_mutation(self):
        t=self.trace();t.p['mutation']=7;t.instruction_count=96;before=copy.deepcopy((t.r,t.memory,t.events))
        with self.assertRaisesRegex(ValueError,'budget'):t.external(PROVIDER)
        self.assertEqual(before,(t.r,t.memory,t.events))
    def test_owned_aligned_initialized_memory(self):
        t=self.trace()
        for a,n in ((RECORDS[0]+1,4),(0x999000,4),(RECORDS[0],1)):
            with self.assertRaisesRegex(ValueError,'ownership'):t.load(a,n)
        del t.memory[RECORDS[0]+8]
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(RECORDS[0]+8,4)
    def test_unreviewed_pc_and_unsupported_opcode_fail(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'instruction'):t.fetch(ENTRIES[0]+36)
        with self.assertRaisesRegex(ValueError,'unsupported'):t.execute(0x42000018,ENTRIES[0])
    def test_control_encoding_in_delay_rejected_even_untaken(self):
        for delay in (0x10220000,0x04410000,0x45000000,0x03E00008):
            t=self.trace();raw=bytearray(t.original)
            struct.pack_into('<II',raw,ENTRIES[0]-0xFF000,0x03E00008,delay);t.original=bytes(raw)
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run()
    def test_old_null_record_and_noncanonical_caught_domain_not_fixtures(self):
        p=dict(query=0,initial0=2,initial1=2,context=0,new_record=2,mutation=0,caught=0)
        self.assertNotIn(p,parameters())
        p=parameters()[0].copy();p['caught']=4
        with self.assertRaisesRegex(ValueError,'source contract'):initialize(QueryTrace(b'',p),p)
    def test_provider_exact_contract_only_once(self):
        t=self.trace();before=copy.deepcopy((t.r,t.memory,t.events))
        with self.assertRaisesRegex(ValueError,'provider'):t.external(PROVIDER+4)
        self.assertEqual(before,(t.r,t.memory,t.events))
        t.external(PROVIDER);before=copy.deepcopy((t.r,t.memory,t.events))
        with self.assertRaisesRegex(ValueError,'repetition'):t.external(PROVIDER)
        self.assertEqual(before,(t.r,t.memory,t.events))
    def test_r0_stays_zero_under_real_decoder(self):
        t=self.trace();t.r[0]=77;t.execute(0,ENTRIES[0]);self.assertEqual(t.r[0],0)
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'local original ELF required')
    def test_actual_provider_mutation_fresh_record_and_null_predicate(self):
        _,raw=validated_elf(ROOT/'orig/SLUS_216.68')
        cases=[p for p in parameters() if (p['query']==0 and p['initial0']==0 and p['context']==0 and p['new_record']==1 and p['mutation']==7 and p['caught']==0) or (p['query']==1 and p['initial0']==2 and p['initial1']==2 and p['mutation']==0 and p['caught']==0)]
        self.assertTrue(cases)
        for p in cases:
            row=fixture(raw,p)
            if p['query']==0:self.assertEqual(row['expected'][:2],[RECORDS[1]+8,0x21030])
            else:self.assertEqual(row['expected'][0],0)
            self.assertEqual(row['expected'][2],1)
if __name__=='__main__':unittest.main()
