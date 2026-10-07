"""Private mandatory pre-mutation instruction guards before fixture authoring."""
import copy,math,struct,unittest
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_matrix_kernels as m

def pack(lanes):return sum(v<<(32*i)for i,v in enumerate(lanes))
def state(t):return copy.deepcopy((t.r,t.f,t.vf,t.vf_defined,t.vu_acc,t.memory,t.instruction_count,t.visited,t.writes))

class MatrixPrimitiveGuards(unittest.TestCase):
    def trace(self):
        t=m.MatrixTrace(b'')
        for p in range(m.BUFFER,m.END):t.memory[p]=0x5A
        return t

    def unchanged_failure(self,t,instruction,pc,pattern=None):
        before=state(t)
        with self.assertRaisesRegex(ValueError,pattern or'.'):
            t.execute(instruction,pc)
        self.assertEqual(state(t),before)

    def test_primary_upper_word_lane_order_and_complete_bit_domain(self):
        self.assertEqual(m.upper_words(pack([17,34,51,68]),pack([85,102,119,136])),pack([119,51,136,68]))
        self.assertEqual(m.upper_words(pack([0xFFFFFFFF,0,0x80000000,0xFFFFFFFF]),pack([1,2,0,0x12345678])),pack([0,0x80000000,0x12345678,0xFFFFFFFF]))
        for bad in(-1,1<<128,1.0,True):
            with self.assertRaises(ValueError):m.upper_words(bad,0)

    def test_both_actual_pextuw_pairs_capture_before_alias_write(self):
        t=self.trace();t.r[2]=pack([1,2,3,4]);t.r[0]=0
        self.assertEqual(t.execute(m.NEW[0x2A1E44],0x2A1E44),(None,False))
        self.assertEqual(t.r[3],pack([3,3,4,4]))
        t.execute(m.NEW[0x2A1E48],0x2A1E48)
        self.assertEqual(t.r[3],pack([4,4,4,4]));self.assertEqual(t.r[2],pack([1,2,3,4]))
        self.assertEqual(t.instruction_count,2);self.assertEqual(t.visited,{0x2A1E44,0x2A1E48});self.assertEqual(t.r[0],0)

    def test_exact_pc_word_and_reserved_rejection_is_pre_mutation(self):
        for pc,instruction in m.NEW.items():
            for altered,where in((instruction^1,pc),(instruction,pc+4),(instruction,0x2A1DF0)):
                t=self.trace();t.r[2]=pack([1,2,3,4]);t.r[3]=pack([1,2,3,4])
                self.unchanged_failure(t,altered,where,'exact')
        t=self.trace();t.r[2]=-1;self.unchanged_failure(t,m.NEW[0x2A1E44],0x2A1E44,'128')
        t.r[2]=1<<128;self.unchanged_failure(t,m.NEW[0x2A1E44],0x2A1E44,'128')

    def test_actual_four_quad_stores_full_order_and_low32_address(self):
        t=self.trace();t.r[4]=m.BUFFER+(1<<64)
        for index,(pc,instruction)in enumerate(list(m.NEW.items())[2:]):
            vf=15-index;t.vf[vf]=[float(index*4+j+1)for j in range(4)];t.vf_defined.add(vf)
            t.execute(instruction,pc)
            self.assertEqual(t.load(m.BUFFER+index*16,16),pack([m.word(float(index*4+j+1))for j in range(4)]))
        self.assertEqual(t.writes,[(m.BUFFER+i*16,16)for i in range(4)])
        self.assertEqual(t.instruction_count,4);self.assertEqual(t.visited,set(list(m.NEW)[2:]));self.assertEqual(t.r[0],0)

    def test_quad_complete_source_and_destination_fail_before_any_mutation(self):
        pc=0x2A2260;instruction=m.NEW[pc]
        for setup in('undefined','partialvf','nonfinite','subnormal','unaligned','outside','partialdest'):
            t=self.trace();t.r[4]=m.BUFFER;t.vf[15]=[1.,2.,3.,4.];t.vf_defined.add(15)
            if setup=='undefined':t.vf_defined.remove(15)
            elif setup=='partialvf':t.vf[15][3]=None
            elif setup=='nonfinite':t.vf[15][3]=math.inf
            elif setup=='subnormal':t.vf[15][3]=struct.unpack('<f',b'\x01\0\0\0')[0]
            elif setup=='unaligned':t.r[4]+=4
            elif setup=='outside':t.r[4]=m.END
            else:del t.memory[m.BUFFER+15]
            self.unchanged_failure(t,instruction,pc)

    def test_budget_precedes_every_new_instruction_mutation_and_counts_once(self):
        for pc,instruction in m.NEW.items():
            t=self.trace();t.r[2]=pack([1,2,3,4]);t.r[3]=t.r[2];t.r[4]=m.BUFFER
            for vf in(12,13,14,15):t.vf[vf]=[1.,2.,3.,4.];t.vf_defined.add(vf)
            t.instruction_count=m.COUNT_LIMIT-1;t.execute(instruction,pc)
            self.assertEqual(t.instruction_count,m.COUNT_LIMIT);self.assertEqual(t.visited,{pc})
            self.unchanged_failure(t,instruction,pc,'bound')

    def test_complete_entry_memory_and_no_other_calls(self):
        t=self.trace()
        for pc in(m.RANGES[0][0]-4,m.RANGES[0][0]+1,m.RANGES[0][1],m.RANGES[1][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.fetch(m.ENTRIES[0])
        with self.assertRaises(ValueError):t.run(m.ENTRIES[0]+4)
        with self.assertRaises(ValueError):t.library_call(m.ENTRIES[0])

    def test_unchanged_control_delay_guard_rejects_untaken_branches(self):
        for delay in((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(4<<21)|(1<<16)|1,(17<<26)|(8<<21)|1,0x03E00008):
            image=bytearray(m.RANGES[-1][1]-0xFF000)
            struct.pack_into('<II',image,m.ENTRIES[0]-0xFF000,0x03E00008,delay)
            t=m.MatrixTrace(bytes(image));t.r[31]=m.RETURN;t.r[4:6]=[1,2]
            with self.assertRaisesRegex(ValueError,'control transfer in delay'):t.run(m.ENTRIES[0])

if __name__=='__main__':unittest.main()
