"""Pure primitive guards; synthetic owned instruction words need no game ELF."""
from pathlib import Path
import copy,struct,sys,unittest
R=Path(__file__).resolve().parents[1];sys.path.insert(0,str(R/'tools'))
from trace_matrix_basis import BasisTrace,BUFFER,END,ENTRIES,NEW,LIMIT,word,coefficient_integers

def machine(pc,encoding):
 raw=bytearray(pc-0xFF000+4);struct.pack_into('<I',raw,pc-0xFF000,encoding)
 t=BasisTrace(bytes(raw));t.memory.update({a:0 for a in range(BUFFER,END)})
 t.r[4]=BUFFER;t.r[31]=0xFFFFFFFC
 return t
def state(t):
 # segment_decoder shares this same dictionary; exclude that recursive view
 # and immutable original bytes while retaining every other observer field.
 return copy.deepcopy({k:v for k,v in t.__dict__.items()if k not in('segment_decoder','original')})

class BasisGuards(unittest.TestCase):
 def reject(self,t,w,pc):
  before=state(t)
  with self.assertRaises(ValueError):t.execute(w,pc)
  self.assertEqual(state(t),before)
 def test_exact_pc_word_mask_register_and_opcode(self):
  pc=ENTRIES[0]+48;w=NEW[pc][0]
  for corrupt in(w^(1<<21),w^(1<<6),w^(1<<11),w^(1<<16),w^1,w^(1<<26)):
   self.reject(machine(pc,corrupt),corrupt,pc)
  self.reject(machine(pc,w),w,pc+4)
  self.reject(machine(pc,w),w,pc+1)
 def test_masked_unknown_W_is_preserved(self):
  pc=ENTRIES[0]+48;w=NEW[pc][0];t=machine(pc,w)
  t.vf[10][3]=None;t.execute(w,pc)
  self.assertEqual(t.vf[10],[1.0,1.0,1.0,None])
  self.assertEqual(t.defined[10],[True,True,True,False])
  pc=ENTRIES[0]+48+19*4;w=NEW[pc][0];t=machine(pc,w)
  t.vf[13]=[None,0.5,None,0.0];t.defined[13]=[False,True,False,True]
  t.vf[8]=[None,None,0.25,None];t.defined[8]=[False,False,True,False]
  t.execute(w,pc);self.assertEqual(t.vf[13],[None,0.25,None,0.0])
 def test_demanded_lane_and_all_results_before_mutation(self):
  pc=ENTRIES[0]+52;w=NEW[pc][0];t=machine(pc,w)
  t.vf[1]=[1.0,2.0,None,4.0];t.defined[1]=[True,True,False,True]
  self.reject(t,w,pc)
  t.vf[1]=[1.0,2.0,3.0,64.0];t.defined[1]=[True]*4
  self.reject(t,w,pc) # Last doubled lane exceeds bounded result; no first-lane commit.
 def test_quadstore_definedness_alignment_and_initialized_bytes(self):
  pc=ENTRIES[0]+48+22*4;w=NEW[pc][0]
  t=machine(pc,w);t.vf[12]=[1.0,2.0,3.0,None];t.defined[12]=[True,True,True,False]
  self.reject(t,w,pc)
  for address in(BUFFER+4,END-8,BUFFER-16):
   t=machine(pc,w);t.vf[12]=[1.0,2.0,3.0,0.0];t.defined[12]=[True]*4;t.vf_defined.add(12);t.r[4]=address
   self.reject(t,w,pc)
  t=machine(pc,w);t.vf[12]=[1.0,2.0,3.0,0.0];t.defined[12]=[True]*4;t.vf_defined.add(12)
  del t.memory[BUFFER+15];self.reject(t,w,pc)
 def test_raw_load_prevalidation(self):
  pc=ENTRIES[0];w=0xC4A3000C
  for bits in(0x7FC00000,0x7F800000,1,word(1/256),word(128.0)):
   t=machine(pc,w);t.r[5]=BUFFER
   t.memory.update({BUFFER+12+i:b for i,b in enumerate(struct.pack('<I',bits))})
   self.reject(t,w,pc)
  t=machine(pc,w);t.r[5]=BUFFER+1;self.reject(t,w,pc)
 def test_packed_GPR_and_coefficient_domain(self):
  pc=ENTRIES[0]+44;w=0x48A20800
  for lanes in((0x80000000,0,0,0),(word(-1),0,word(1),word(1)),(word(5),word(1),word(1),word(1)),(1,0,0,0)):
   t=machine(pc,w);t.r[2]=sum(v<<(32*i)for i,v in enumerate(lanes));self.reject(t,w,pc)
  for bad in(-1,1<<128):
   t=machine(pc,w);t.r[2]=bad;self.reject(t,w,pc)
  self.assertEqual(coefficient_integers([1/16,2/16,3/16,4/16]),[1,2,3,4])
 def test_budget_and_zero_register(self):
  pc=ENTRIES[0]+48;w=NEW[pc][0];t=machine(pc,w);t.instruction_count=LIMIT
  self.reject(t,w,pc)
  t.instruction_count=LIMIT-1;t.r[0]=17;t.execute(w,pc)
  self.assertEqual(t.instruction_count,LIMIT);self.assertEqual(t.r[0],0)
 def test_actual_JR_and_owned_delay_required(self):
  entry=ENTRIES[0];raw=bytearray(0x2A1F14-0xFF000)
  struct.pack_into('<I',raw,entry-0xFF000,0x03E00008)
  struct.pack_into('<I',raw,entry+4-0xFF000,0x10000000)
  t=BasisTrace(bytes(raw));t.r[31]=0xFFFFFFFC
  with self.assertRaises(ValueError):t.run(entry)
  t=BasisTrace(bytes(raw))
  with self.assertRaises(ValueError):t.run(entry+4)

if __name__=='__main__':unittest.main()
