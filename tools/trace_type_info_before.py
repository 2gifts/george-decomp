"""Finite initialized GNU before observations; genuine strcmp body executes.

No new instruction decoder, host-runtime ABI identity or game reachability claim.
Inherited RegistryTrace executes unchanged. Every optimized read has initialized
padding; unknown/unowned memory and all unreviewed code remain rejected.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_string_registry import RegistryTrace,BASE,END,STACK,MASK,RETURN
from trace_geometry import is_control_transfer

ROOT=Path(__file__).resolve().parents[1]
ENTRY,END_ENTRY,CMP,END_CMP=0x375F88,0x375FAC,0x393A28,0x393B74

class BeforeTrace(RegistryTrace):
    def __init__(self,original):super().__init__(original,{})
    def fetch(self,pc):
        if pc&3 or not (ENTRY<=pc<END_ENTRY or CMP<=pc<END_CMP):
            raise ValueError('unowned before instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('before code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def run_before(self,stop=RETURN):
        pc=ENTRY
        while True:
            if not ENTRY<=pc<END_ENTRY:raise ValueError('before transfer outside body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                if annul:raise ValueError('unobserved before annul')
                if pc+4>=END_ENTRY:raise ValueError('before delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                    raise ValueError('before control in delay')
                if w==0x03E00008:
                    if pc!=END_ENTRY-8 or target!=stop:raise ValueError('before actual terminal JR31/stop')
                    return
                if w>>26!=3 or target!=CMP:raise ValueError('unreviewed before callee/control')
                continuation=self.r[31]&MASK
                super().run(CMP,continuation,1)
                pc=continuation
            else:
                if target is not None or annul:raise ValueError('before unexpected transfer')
                pc+=4

PAIRS=(
 (b'',b''),(b'',b'a'),(b'a',b''),(b'abc',b'abd'),(b'abd',b'abc'),
 (b'prefix',b'prefix-more'),(b'prefix-more',b'prefix'),
 (b'\x80',b'\x7f'),(b'\x7f',b'\x80'),(b'\xff\x81',b'\xff\x82'),
 (b'A'*8,b'A'*8),(b'B'*16,b'B'*16),(b'C'*32,b'C'*32),
 (b'D'*31+b'x',b'D'*31+b'y'),(b'E'*47,b'E'*47+b'!'),
)
def fixture(original,left,right,left_offset,right_offset,alias=0):
    if not isinstance(left,bytes) or not isinstance(right,bytes) or len(left)>48 or len(right)>48 or b'\0'in left or b'\0'in right:
        raise ValueError('before initialized byte-string domain')
    if type(left_offset)is not int or type(right_offset)is not int or not 0<=left_offset<16 or not 0<=right_offset<16 or alias not in (0,1,2):
        raise ValueError('before fixture domain')
    if alias and (left!=right or left_offset!=right_offset):raise ValueError('before alias must describe same initialized name')
    t=BeforeTrace(original)
    t.memory={a:0xA5 for a in range(BASE,END)}
    a,b=BASE+0x100+left_offset,BASE+0x200+right_offset
    if alias:b=a
    for p,s in ((a,left),(b,right)):
        for i,c in enumerate(s+b'\0'+bytes(64-len(s))):t.save(p+i,c,1)
    obj1,obj2=BASE+0x40,BASE+0x50
    if alias==2:obj2=obj1
    t.save(obj1,a,4);t.save(obj2,b,4)
    # Historical target layout has name at0 and its unaccessed second word at4.
    t.save(obj1+4,0xF0000010,4);t.save(obj2+4,0xF0000020,4)
    t.r[4],t.r[5],t.r[29],t.r[31]=obj1,obj2,STACK[1]-0x100,RETURN
    before=bytes(t.memory[a+i] for i in range(64));before2=bytes(t.memory[b+i] for i in range(64))
    t.run_before();result=t.r[2]&MASK;expected=int(left<right)
    assert result==expected and result in (0,1)
    assert bytes(t.memory[a+i] for i in range(64))==before and bytes(t.memory[b+i] for i in range(64))==before2
    assert t.r[29]==STACK[1]-0x100 and t.r[31]==RETURN
    assert set(range(ENTRY,END_ENTRY,4))<=t.visited
    return dict(left=list(left),right=list(right),left_offset=left_offset,right_offset=right_offset,alias=alias,result=result),t

def generate(original):
    rows=[];instructions=maximum=0;selected=set();support=set()
    for left,right in PAIRS:
        for la in range(16):
            for ra in range(16):
                q,t=fixture(original,left,right,la,ra);rows.append(q);instructions+=t.instruction_count;maximum=max(maximum,t.instruction_count)
                selected|={pc for pc in t.visited if ENTRY<=pc<END_ENTRY};support|={pc for pc in t.visited if CMP<=pc<END_CMP}
    for alias in (1,2):
        for la in range(16):
            q,t=fixture(original,b'\xff-same',b'\xff-same',la,la,alias);rows.append(q);instructions+=t.instruction_count;maximum=max(maximum,t.instruction_count)
            selected|={pc for pc in t.visited if ENTRY<=pc<END_ENTRY};support|={pc for pc in t.visited if CMP<=pc<END_CMP}
    assert len(selected)==9
    serialized=json.dumps(rows,separators=(',',':')).encode()
    return dict(fixtures=len(rows),instructions=instructions,maximum_instructions=maximum,visited_selected=len(selected),visited_support=len(support),
        input_sha256=hashlib.sha256(serialized).hexdigest(),records=rows,
        limits='Finite initialized/padded byte strings and typed historical name fields. Inherited exact integer/MMI decoder, no new opcode. No invalid objects/uninitialized padding/SDK/data/helper/runtime-invocation or host C++ ABI identity claim.')

def header(packet):
    out=['/* Authored finite byte-string fixtures; no original code or assets. */',
         'struct BeforeFixture { unsigned char left[49], right[49]; unsigned int left_size, right_size, left_offset, right_offset, alias, expected; };',
         'static const BeforeFixture before_fixtures[] = {']
    for q in packet['records']:
        a=','.join(map(str,q['left']+[0]));b=','.join(map(str,q['right']+[0]))
        out.append('{{%s},{%s},%d,%d,%d,%d,%d,%d},'%(a,b,len(q['left']),len(q['right']),q['left_offset'],q['right_offset'],q['alias'],q['result']))
    out+=['};','']
    return '\n'.join(out)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--golden-header',type=Path,required=True);args=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');packet=generate(raw)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(packet,indent=2)+'\n',newline='\n')
    args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(header(packet),newline='\n')
    print({k:v for k,v in packet.items() if k not in ('records','limits')})
