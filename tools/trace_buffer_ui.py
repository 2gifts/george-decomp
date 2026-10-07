"""Initialized, bounded observations of the three original console UI bodies.

Actual buffer, map, string, and float-conversion instructions execute. Renderer,
parser, and completion callbacks are declared controls. This is a finite host
float model, not an EE emulator: FCR flags, timing, exceptional arithmetic, and
format output are excluded. Caller stack input is explicit; reads never invent
zero. Golden fixtures contain authored arena data, hashes, and numeric bindings.
"""
import argparse
import hashlib
import json
import math
import struct
import zlib
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer, scalar, word, rounded

ROOT = Path(__file__).resolve().parents[1]
MASK, U64, U128 = 0xFFFFFFFF, (1 << 64)-1, (1 << 128)-1
BUFFER, WORDS = 0x20000, 4096
END, STACK = BUFFER+WORDS*4, (0x7E000, 0x81000)
UI, ALT_UI, MANAGER, ALT_MANAGER = 0x20040, 0x20440, 0x20900, 0x20940
KEYS, LINES, ALT_LINES = 0x20A00, 0x20C00, 0x20C40
RECORDS = tuple(0x20D00+i*16 for i in range(16))
ALT_RECORDS = tuple(0x20E00+i*16 for i in range(16))
TEXTS = tuple(0x21001+i*0x90 for i in range(16))
ALT_TEXTS = tuple(0x22001+i*0x90 for i in range(16))
HISTORY, HISTORY_TEXTS, MAP = 0x23000, (0x23081,0x23101,0x23181), 0x23200
MAP_BUCKETS, MAP_NODES = 0x23240, (0x23300,0x23320,0x23340)
VALUES, TABLES, BUCKETS = (0x23400,0x23420,0x23440), (0x23500,0x23520,0x23540), (0x23600,0x23680,0x23700)
HASH_NODES = (0x23800,0x23820,0x23840,0x23860,0x23880)
GLOBALS = (0x3F9408,0x4683AC,0x481760)
SELECTED = ((0x2163E8,0x216730),(0x2167D8,0x2169BC),(0x2169C0,0x216DFC))
SUPPORT = ((0x295050,0x295080),(0x29C648,0x29C6C0),
 (0x2A4B70,0x2A4DB8),(0x2A4DB8,0x2A4EF0),(0x2A5230,0x2A528C),
 (0x2A5290,0x2A5314),(0x2A5398,0x2A53C0),(0x2A53C0,0x2A53C8),
 (0x2A53C8,0x2A53D0),(0x2A53D0,0x2A53E8),(0x2A53E8,0x2A53F4),
 (0x2A53F8,0x2A5430),(0x2A56C0,0x2A56D0),(0x2A7C08,0x2A7C58),
 (0x2A8130,0x2A81D0),(0x2AAF50,0x2AAF88),(0x2AAF88,0x2AAF90),
 (0x2CD990,0x2CDA20),(0x2CDA20,0x2CDAB0),(0x2CDD60,0x2CDD88),
 (0x2CDD88,0x2CDDB0),(0x2CDDB0,0x2CDDE4),(0x372858,0x372984),
 (0x3734C8,0x3734F4),(0x373DB8,0x373E48),(0x374848,0x374888),
 (0x3936A0,0x393758),(0x393758,0x393888),(0x393B74,0x393C8C),
 (0x394010,0x3941D8),(0x398558,0x3985D4),(0x3985D8,0x398624),(0x398628,0x39869C))
RANGES = SELECTED+SUPPORT
CONTROL = (0x2162D0,0x2CEB78,0x2D0258,0x2D04B0,0x23C908,0x2BA680,0x2BA708,
 0x290BF0,0x290CF8,0x28E8A0,0x28E668,0x28FE88,0x28FBC0,0x28FD30,0x28C590,0x23E0D0)
CTYPE, CTYPE_SIZE = 0x456118,257
CTYPE_SHA = '8b55a0d9c781d7001042c99e21523fef362440f572faef950e8e600fae5de813'
STRING_BINDINGS = ((0x43A6E8,1),(0x43A6F0,8),(0x43A6F8,3),(0x43A690,2),
 (0x43A700,21),(0x43A718,5),(0x43A720,12),(0x43A730,9),(0x43A740,15),
 (0x43A750,54),(0x43A788,9),(0x43A798,6))
# Optimized original reads may inspect initialized lanes beyond a zero byte.
# These bounded observer windows are not object extents or recovered assets.
READONLY = tuple((a,(a+n+15)&~15) for a,n in STRING_BINDINGS)+((CTYPE,CTYPE+CTYPE_SIZE),(0x445650,0x445A50))
TEXT_REGIONS = ((UI+0x10,0x300),(ALT_UI+0x10,0x300),
 *((a-1,0x90) for a in TEXTS+ALT_TEXTS),*((a-1,0x80) for a in HISTORY_TEXTS))

def signed(x):
    x &= MASK
    return x-0x100000000 if x&0x80000000 else x
def signed64(x):
    x &= U64
    return x-(1<<64) if x&(1<<63) else x
def sx32(x):return signed(x)&U64
def initial_word(i):return (0xA5030201^(i*0x10203))&MASK
def cvtw(bits):
    if (bits>>23)&255>0x9D:return 0x80000000 if bits>>31 else 0x7FFFFFFF
    value=scalar(bits)
    if not math.isfinite(value):raise ValueError('unsupported EE conversion value')
    return math.trunc(value)&MASK

class UiTrace(Trace):
    def __init__(self,original,mutation=0,completion=0,parse=0):
        super().__init__(original)
        self.fraw=[0]*32;self.lo=self.hi=0;self.events=[];self.visited=set()
        self.branches=[];self.invocations={a:0 for a,b in RANGES}
        self.mutation=mutation;self.completion=completion;self.parse=parse
        self.callback_count=0;self.renderer_count=0;self.pointer_cells=set()
        self.readonly_reads=set();self.call_counts={};self.stack_base=None
    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not (
          BUFFER<=address<address+size<=END or STACK[0]<=address<address+size<=STACK[1] or
          any(a<=address<address+size<=a+4 for a in GLOBALS)):
            raise ValueError('unowned or unaligned UI memory %#x/%d'%(address,size))
    def load(self,address,size):
        if any(a<=address<address+size<=b for a,b in READONLY):
            if address%size:raise ValueError('unaligned UI readonly read')
            self.readonly_reads.add((address,size))
            off=address-0xFF000
            if off<0 or off+size>len(self.original):raise ValueError('UI readonly image bound')
            return int.from_bytes(self.original[off:off+size],'little')
        self.memory_check(address,size)
        if not all(address+i in self.memory for i in range(size)):
            raise ValueError('uninitialized UI read')
        return super().load(address,size)
    def save(self,address,value,size):
        self.memory_check(address,size);super().save(address,value,size)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed UI code')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('UI code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def text(self,address):
        data=bytearray()
        for i in range(512):
            b=self.load((address+i)&MASK,1)
            if not b:return bytes(data)
            data.append(b)
        raise ValueError('bounded UI string observer')
    def put(self,address,data):
        for i,b in enumerate(data+b'\0'):self.save(address+i,b,1)
    def event(self,kind,*values):
        if len(self.events)>=2400:raise ValueError('UI controlled event bound')
        self.events.append([kind,*[v&MASK for v in values]])
    def execute(self,w,pc):
        if self.instruction_count>=60000:raise ValueError('UI instruction bound')
        self.instruction_count+=1;self.visited.add(pc)
        op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        im=w&65535;si=im-65536 if im&32768 else im;target=None;annul=False
        a,b=self.r[rs],self.r[rt]
        if w==0:pass
        elif op==0:
            if fn in (0x21,0x23) and sh==0:self.r[rd]=sx32(a+b if fn==0x21 else a-b)
            elif fn in (0x2D,0x2F) and sh==0:self.r[rd]=(a+b if fn==0x2D else a-b)&U64
            elif fn in (0,2,3) and rs==0:
                self.r[rd]=sx32((b<<sh) if fn==0 else (b&MASK)>>sh if fn==2 else signed(b)>>sh)
            elif fn in (4,6,7) and sh==0:
                n=a&31;self.r[rd]=sx32(b<<n if fn==4 else (b&MASK)>>n if fn==6 else signed(b)>>n)
            elif fn in (0x14,0x16,0x17) and sh==0:
                n=a&63;self.r[rd]=((b<<n) if fn==0x14 else (b&U64)>>n if fn==0x16 else signed64(b)>>n)&U64
            elif fn in (0x38,0x3A,0x3B,0x3C,0x3E,0x3F) and rs==0:
                n=sh+(32 if fn>=0x3C else 0)
                self.r[rd]=((b<<n) if fn in (0x38,0x3C) else (b&U64)>>n if fn in (0x3A,0x3E) else signed64(b)>>n)&U64
            elif fn in (0x24,0x25,0x26,0x27) and sh==0:
                self.r[rd]=(a&b if fn==0x24 else a|b if fn==0x25 else a^b if fn==0x26 else ~(a|b))&U64
            elif fn in (0x2A,0x2B) and sh==0:
                self.r[rd]=int(signed64(a)<signed64(b)) if fn==0x2A else int((a&U64)<(b&U64))
            elif fn in (0x0A,0x0B) and sh==0:
                if (b==0 if fn==0x0A else b!=0):self.r[rd]=a
            elif fn in (0x10,0x12) and rs==rt==sh==0:self.r[rd]=sx32(self.hi if fn==0x10 else self.lo)
            elif fn==0x1B and rd==sh==0:
                dividend,divisor=a&MASK,b&MASK
                if not divisor:raise ValueError('UI unsigned division by zero')
                self.lo,self.hi=dividend//divisor,dividend%divisor
            elif fn==0x18 and sh==0:
                product=signed(a)*signed(b);self.lo=product&MASK;self.hi=(product>>32)&MASK
                if rd:self.r[rd]=sx32(self.lo)
            elif fn==0x0D:raise ValueError('original UI BREAK trap')
            elif fn==8 and rt==rd==sh==0:target=a&MASK
            elif fn==9 and rt==sh==0 and rd==31:target=a&MASK;self.r[31]=pc+8
            else:raise ValueError('unsupported UI SPECIAL operands %#x at%#x'%(w,pc))
        elif op in (9,25):self.r[rt]=sx32(a+si) if op==9 else (a+si)&U64
        elif op in (10,11):self.r[rt]=int(signed64(a)<si) if op==10 else int((a&U64)<(si&U64))
        elif op in (12,13,14):self.r[rt]=(a&im if op==12 else a|im if op==13 else a^im)&U64
        elif op==15 and rs==0:self.r[rt]=sx32(im<<16)
        elif op in (2,3):
            if op==3:self.r[31]=pc+8
            target=((pc+4)&0xF0000000)|((w&0x3FFFFFF)<<2)
        elif op==1 and rt in (0,1,2,3):
            take=signed64(a)<0 if rt in (0,2) else signed64(a)>=0
            if take:target=(pc+4+4*si)&MASK
            elif rt in (2,3):annul=True
        elif op in (4,5,6,7,20,21,22,23):
            if op in (6,7,22,23) and rt:raise ValueError('reserved UI zero branch')
            take=(a==b if op in (4,20) else a!=b if op in (5,21) else signed64(a)<=0 if op in (6,22) else signed64(a)>0)
            if take:target=(pc+4+4*si)&MASK
            elif op>=20:annul=True
        elif op in (30,32,33,35,36,37,55):
            n={30:16,32:1,33:2,35:4,36:1,37:2,55:8}[op];v=self.load((a+si)&MASK,n)
            if op in (32,33):
                if v&(1<<(8*n-1)):v-=1<<(8*n)
                v&=U64
            elif op==35:v=sx32(v)
            self.r[rt]=v
        elif op in (31,40,41,43,63):self.save((a+si)&MASK,b,{31:16,40:1,41:2,43:4,63:8}[op])
        elif op in (26,27,44,45):
            address=(a+si)&MASK;base=address&~7;k=address&7
            if op==26:
                count=k+1;v=int.from_bytes(bytes(self.load(base+i,1) for i in range(count)),'little')
                shift=(8-count)*8;mask=((1<<(count*8))-1)<<shift
                self.r[rt]=((b&~mask)|(v<<shift))&U64
            elif op==27:
                count=8-k;v=int.from_bytes(bytes(self.load(address+i,1) for i in range(count)),'little')
                mask=(1<<(count*8))-1;self.r[rt]=((b&~mask)|v)&U64
            elif op==44:
                count=k+1;v=(b&U64)>>((8-count)*8)
                for i in range(count):self.save(base+i,v>>(8*i),1)
            else:
                count=8-k
                for i in range(count):self.save(address+i,b>>(8*i),1)
        elif op in (49,57):
            address=(a+si)&MASK
            if op==49:self.fraw[rt]=self.load(address,4)
            else:self.save(address,self.fraw[rt],4)
        elif op==17:
            if rs==4 and sh==fn==0:self.fraw[rd]=b&MASK
            elif rs==0 and sh==fn==0:self.r[rt]=sx32(self.fraw[rd])
            elif rs==20 and fn==0x20 and rt==0:self.fraw[sh]=word(rounded(float(signed(self.fraw[rd]))))
            elif rs==16:
                if fn==6 and rt==0:self.fraw[sh]=self.fraw[rd]
                elif fn==0x24 and rt==0:self.fraw[sh]=cvtw(self.fraw[rd])
                elif fn in (0,1,2,3):
                    fa,fb=scalar(self.fraw[rd]),scalar(self.fraw[rt])
                    if not math.isfinite(fa) or not math.isfinite(fb) or fn==3 and fb==0:
                        raise ValueError('UI unsupported nonfinite/divzero arithmetic')
                    value=fa+fb if fn==0 else fa-fb if fn==1 else fa*fb if fn==2 else fa/fb
                    self.fraw[sh]=word(rounded(value))
                else:raise ValueError('unsupported UI COP1 scalar encoding')
            else:raise ValueError('unsupported UI COP1 operands')
        elif op==28:
            if fn==9 and sh==0x0E: # PCPYLD
                self.r[rd]=((a&U64)<<64)|(b&U64)
            elif fn==0x29 and sh==0x0E: # PCPYUD: high rs -> low, high rt -> high
                self.r[rd]=((b>>64)<<64)|(a>>64)
            elif fn==8 and sh==9: # PSUBB
                self.r[rd]=sum((((a>>(8*i)&255)-(b>>(8*i)&255))&255)<<(8*i) for i in range(16))
            elif (fn,sh) in ((9,0x12),(0x29,0x13)):
                self.r[rd]=(a&b if sh==0x12 else ~(a|b))&U128
            else:raise ValueError('unsupported UI MMI operands %#x at%#x'%(w,pc))
        else:raise ValueError('unsupported UI opcode %#x at%#x'%(w,pc))
        self.r[0]=0;return target,annul
    def callback(self,target):
        if target not in CONTROL and target not in (0xF0000010,0xF0000020):
            raise ValueError('unreviewed UI controlled target %#x'%target)
        args=[self.r[i]&MASK for i in range(4,12)]
        self.call_counts[target]=self.call_counts.get(target,0)+1
        if target==0x2162D0:
            self.callback_count+=1;self.event(target,*args[:3])
            ui=self.load(GLOBALS[0],4)
            count=self.load(ui+0x0C,4)
            self.save(ui+0x0C,(count+self.completion)&MASK,4)
            self.put(ui+0x10,b'alpha');self.save(ui+0x210,3,4)
            if self.callback_count==1:
                if self.mutation==1:self.save(GLOBALS[0],ALT_UI,4)
                elif self.mutation==2:self.save(TABLES[0]+8,TABLES[2],4)
                elif self.mutation==3:
                    node=args[1]-8 if HASH_NODES[0]+8<=args[1]<=HASH_NODES[-1]+8 else HASH_NODES[1]
                    self.save(node,HASH_NODES[4],4)
                elif self.mutation==4:self.save(MAP_NODES[0],MAP_NODES[2],4)
        elif target==0x2CEB78:
            data=self.text(args[0]);self.event(target,len(data),zlib.crc32(data),args[1],args[2])
            self.r[2]=self.parse
            if self.mutation==5:self.save(GLOBALS[0],ALT_UI,4)
        elif target in (0x2D0258,0x2D04B0):self.event(target,*args[:3] if target==0x2D0258 else args[:2])
        elif target==0x23C908:
            fmt=args[1];values=list(self.fraw[12:18]);values.extend(args[:2])
            if fmt==0x43A750:
                for r in (6,7):values.extend((self.r[r]&MASK,(self.r[r]>>32)&MASK))
            elif fmt==0x43A788:values.extend((self.r[6]&MASK,(self.r[6]>>32)&MASK))
            elif fmt==0x43A798:values.extend((args[2],zlib.crc32(self.text(args[2]))))
            else:values.append(zlib.crc32(self.text(fmt)))
            self.event(target,*values);self.renderer_count+=1
            if self.mutation==6 and self.renderer_count==1:self.save(GLOBALS[0],ALT_UI,4)
            if self.mutation==7 and self.renderer_count==2:self.save(ALT_MANAGER+0x0C,ALT_LINES,4)
        elif target in (0x28FD30,0x28FE88):self.event(target,*self.fraw[12:15 if target==0x28FD30 else 16])
        elif target==0x290CF8:self.event(target,*args[:2])
        elif target in (0x290BF0,0x28E8A0,0x28E668,0x28FBC0,0x23E0D0):self.event(target,args[0])
        else:self.event(target)
        # Opaque bodies may clobber caller-saved argument lanes. These controls
        # preserve f0 only when its value is explicitly relevant (never here).
        result=self.r[2]
        for r in range(2,16):self.r[r]=0xDEADBEEFDEADBEEF
        self.r[2]=result
    def run(self,entry,stop=RETURN,depth=0):
        interval=next((p for p in RANGES if p[0]==entry),None)
        if interval is None or depth>12:raise ValueError('unreviewed UI entry/depth')
        a,b=interval;self.invocations[a]+=1;pc=a
        if entry==0x394010:
            dest,source,count=(self.r[i]&MASK for i in (4,5,6))
            if count>64 or count>=8 and (dest|source)&7==0:
                raise ValueError('UI strncpy observer byte-path domain')
            if count and not(dest+count<=source or source+count<=dest):
                raise ValueError('UI strncpy observer nonoverlap domain')
        while True:
            if not a<=pc<b:raise ValueError('UI control outside complete body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append((pc,target is not None,annul))
                if not annul:
                    if pc+4>=b:raise ValueError('UI delay beyond complete body')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                        raise ValueError('encoded control in UI delay')
                if w>>26==0 and w&63==8:
                    if target!=stop:raise ValueError('UI genuine return stop')
                    return
                if w>>26==3 or w>>26==0 and w&63==9:
                    continuation=self.r[31]&MASK
                    if target in self.invocations:self.run(target,continuation,depth+1)
                    else:self.callback(target)
                    if self.r[31]&MASK!=continuation:raise ValueError('UI changed return address')
                    pc=continuation
                else:
                    if target is not None and not a<=target<b:raise ValueError('UI branch crosses body')
                    pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('UI noncontrol transfer')
                pc+=4

def fixture(original,routine=0,text=b'al',key=65,delta=0.02,rows=12,limit=8,
            flags=0x80000001,disabled=0,pressed=1,parse=0,completion=1,table=0,
            mutation=0,history=0,tag=4):
    if routine not in range(3) or mutation not in range(8) or not 0<=key<=255:
        raise ValueError('UI fixture selector domain')
    if len(text)>96 or b'\0' in text or not math.isfinite(delta) or not 0<=rows<=16 or limit not in (0,1,2,4,8,16):
        raise ValueError('UI fixture initialized observer domain')
    if rows==0 and routine!=2:raise ValueError('UI buffer fixtures require real current record')
    t=UiTrace(original,mutation,completion,parse)
    for i in range(WORDS):t.save(BUFFER+4*i,initial_word(i),4)
    # Explicit initialized prior caller-stack input. This is not an implicit
    # decoder default and adds no production initialization or stack placement.
    for p in range(STACK[0],STACK[1]):t.save(p,(p*17+0xA3)&255,1)
    pointers=set(GLOBALS)
    def pointer(p,v):t.save(p,v,4);pointers.add(p)
    for ui,manager in ((UI,MANAGER),(ALT_UI,ALT_MANAGER)):
        pointer(ui,manager);t.save(ui+4,word((0.02 if routine==1 else delta) if ui==UI else 0.04),4)
        t.save(ui+8,0xFEDCBA98 if ui==UI else 0x87654321,4)
        t.save(ui+0x0C,0,4);t.save(ui+0x210,0,4)
        t.put(ui+0x10,b'');t.put(ui+0x110,b'');t.put(ui+0x214,b'')
        linearray=LINES if ui==UI else ALT_LINES
        records=RECORDS if ui==UI else ALT_RECORDS;texts=TEXTS if ui==UI else ALT_TEXTS
        t.save(manager,flags,4);t.save(manager+4,rows,4);t.save(manager+8,limit,4)
        pointer(manager+0x0C,linearray);pointer(manager+0x10,records[max(0,rows-1)])
        pointer(manager+0x14,0x23900);t.save(manager+0x18,0,4);t.save(manager+0x1C,3,4)
        t.save(manager+0x20,history,4);t.save(manager+0x24,2,4);pointer(manager+0x28,HISTORY)
        for k,(record,txt) in enumerate(zip(records,texts)):
            pointer(linearray+4*k,record);t.save(record,limit,4)
            content=text if k==max(0,rows-1) else b'row'+str(k).encode()
            if ui==ALT_UI:content=b'other'+str(k).encode()
            t.save(record+4,len(content),4);pointer(record+8,txt);t.put(txt,content)
    for i,txt in enumerate(HISTORY_TEXTS):pointer(HISTORY+4*i,txt);t.put(txt,b'hist'+str(i).encode())
    t.save(0x23900,0x20,4);t.save(0x23904,0,4);t.save(0x23908,0,4);pointer(0x2390C,0)
    for i in range(0x106):t.save(KEYS+i,0,1)
    t.save(KEYS+0x65,disabled,1);t.save(KEYS+4,key,1);t.save(KEYS+5+key,8 if pressed else 0,1)
    pointer(GLOBALS[0],UI);pointer(GLOBALS[1],KEYS);pointer(GLOBALS[2],MAP)
    t.save(MAP,0,4);t.save(MAP+4,17,4);t.save(MAP+8,3,4);pointer(MAP+0x0C,MAP_BUCKETS)
    for b in range(17):pointer(MAP_BUCKETS+4*b,0)
    for k,(tab,buckets) in enumerate(zip(TABLES,BUCKETS)):
        for off in (0,4):t.save(tab+off,0,4)
        pointer(tab+8,TABLES[1] if k==0 else 0);pointer(tab+0x0C,buckets)
        for b in range(17):pointer(buckets+4*b,0)
    # Actual hash values of authored tokens are generated independently by zlib;
    # retail CRC instructions still execute every production lookup.
    for k,node in enumerate(MAP_NODES):
        data=(b'o'*75 if text.startswith(b'o'*75) else b'one',b'two',b'plain')[k];keyhash=zlib.crc32(data)
        pointer(node,0);t.save(node+4,keyhash,4);pointer(node+8,VALUES[k])
        t.save(VALUES[k],tag if k==0 else 0,4);pointer(VALUES[k]+4,TABLES[0] if k==0 and table else 0)
        pointer(MAP_BUCKETS+4*(keyhash%17),node)
    for k,node in enumerate(HASH_NODES):
        pointer(node,0);t.save(node+4,0xABC00000+k,4);t.save(node+8,0,4);t.save(node+12,0,4)
    pointer(BUCKETS[0],HASH_NODES[1]);pointer(HASH_NODES[1],HASH_NODES[2])
    pointer(BUCKETS[1],HASH_NODES[3]);pointer(BUCKETS[2],HASH_NODES[4])
    if table:
        t.save(HASH_NODES[0]+4,zlib.crc32(b'two'),4);t.save(HASH_NODES[0]+8,4,4)
        pointer(HASH_NODES[0]+12,TABLES[1]);cell=BUCKETS[0]+4*(zlib.crc32(b'two')%17)
        pointer(HASH_NODES[0],t.load(cell,4));pointer(cell,HASH_NODES[0])
    if mutation==4:
        for b in range(17):pointer(MAP_BUCKETS+4*b,0)
        pointer(MAP_BUCKETS,MAP_NODES[0]);pointer(MAP_NODES[0],MAP_NODES[1])
    initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    initial_globals=[t.load(p,4) for p in GLOBALS]
    t.r[29]=0x80000;t.r[31]=RETURN;t.fraw[12]=word(delta)
    saved=[]
    for r in range(16,24):t.r[r]=(0x123456789ABCDEF<<64)|(0x100000000+r);saved.append(t.r[r])
    t.run(SELECTED[routine][0])
    if t.r[29]!=0x80000 or any(t.r[r]!=v for r,v in zip(range(16,24),saved)):
        raise AssertionError('UI saved register/frame conservation')
    expected=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    # Inline +210/+214 views overlap TEXT_REGIONS: exclude only actual character
    # regions, retain +210 count word as ordinary word memory.
    regions=[]
    for a,n in TEXT_REGIONS:
        if a in (UI+0x10,ALT_UI+0x10):regions.extend(((a,0x200),(a+0x204,0xFC)))
        else:regions.append((a,n))
    hashes=[[a,n,zlib.crc32(bytes(t.load(a+i,1) for i in range(n)))] for a,n in regions]
    def charword(i):return any(a<=BUFFER+4*i< a+n for a,n in regions)
    return dict(routine=routine,delta_bits=word(delta),mutation=mutation,completion=completion,parse=parse,
      initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],
      changes=[[i,v] for i,v in enumerate(expected) if v!=initial[i] and not charword(i)],
      text_hashes=hashes,pointers=sorted((p-BUFFER)//4 for p in pointers if BUFFER<=p<END),
      globals=initial_globals,expected_globals=[t.load(p,4) for p in GLOBALS],events=t.events,
      instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,
      invocations=[[a,n] for a,n in t.invocations.items() if n],readonly_reads=sorted(t.readonly_reads))

def fixtures(original):
    result=[]
    for text in (b'',b'al',b'x,y.z',b'one.al',b'one.two.al',b'alpha beta',b'abcd/ef:gh'):
        for completion in (0,1,2):
            for table in (0,1):result.append(fixture(original,text=text,completion=completion,table=table))
    for mutation in range(1,5):
        for table in (0,1):result.append(fixture(original,text=b'one.al' if table else b'al',table=table,mutation=mutation,completion=1))
    for tag in (3,0x8004,0xFFFF):result.append(fixture(original,text=b'one.al',table=1,tag=tag))
    result.append(fixture(original,text=b'o'*75+b'.al',table=1,completion=1))
    for key in (0,8,9,10,13,31,32,65,95,126,127,128,0xE5,0xE7,255):
        for pressed in (0,1):
            for flags,disabled in ((0x80000001,0),(1,0),(0x80000001,4)):
                result.append(fixture(original,1,text=b'ab',key=key,pressed=pressed,flags=flags,disabled=disabled))
    for key in (10,13):
        for parse in (0,1):
            for mutation in (0,5):result.append(fixture(original,1,text=b'go',key=key,parse=parse,mutation=mutation))
    for key in (8,0xE5,0xE7):
        for history in (0,1,2):result.append(fixture(original,1,text=b'abc',key=key,history=history))
    for rows in (0,1,5,8,9,10,12,16):
        for limit in (0,1,2,4,8,16):
            for delta in (0.02,0.04,-0.02):result.append(fixture(original,2,rows=rows,limit=limit,delta=delta))
    for mutation in (6,7):result.append(fixture(original,2,mutation=mutation))
    result.append(fixture(original,2,flags=1))
    return result

def golden(cases):
    sizes={k:max(len(c[k]) for c in cases) for k in ('initial','changes','pointers','text_hashes','events')}
    flat=lambda events:[v for e in events for v in (len(e),*e)]
    maxevents=max(len(flat(c['events'])) for c in cases)
    lines=['/* Authored UI inputs, memory hashes and numeric observer events only. */',
      '#define UI_WORDS %du'%WORDS,'#define UI_INITIAL %du'%sizes['initial'],
      '#define UI_CHANGES %du'%sizes['changes'],'#define UI_POINTERS %du'%sizes['pointers'],
      '#define UI_HASHES %du'%sizes['text_hashes'],'#define UI_EVENTS %du'%maxevents,
      'struct UiPair {u32 index,value;};struct UiHash {u32 address,size,crc;};',
      'struct UiGolden {u32 routine,delta_bits,mutation,completion,parse,initial_count,change_count,pointer_count,event_count;struct UiPair initial[UI_INITIAL],changes[UI_CHANGES];u32 pointers[UI_POINTERS],globals[3],expected_globals[3],events[UI_EVENTS];struct UiHash hashes[UI_HASHES];};',
      'static const struct UiGolden ui_golden[]={']
    for c in cases:
        pairs=lambda v:'{'+','.join('{%du,0x%08Xu}'%(i,x) for i,x in v)+'}'
        array=lambda v:'{'+','.join('0x%08Xu'%x for x in v)+'}'
        events=flat(c['events'])
        fields=[c[k] for k in ('routine','delta_bits','mutation','completion','parse')]+[len(c[k]) for k in ('initial','changes','pointers')]+[len(events)]
        hashes='{'+','.join('{0x%08Xu,%du,0x%08Xu}'%tuple(v) for v in c['text_hashes'])+'}'
        lines.append('{'+','.join('0x%08Xu'%x for x in fields)+','+pairs(c['initial'])+','+pairs(c['changes'])+','+array(c['pointers'])+','+array(c['globals'])+','+array(c['expected_globals'])+','+array(events)+','+hashes+'},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/buffer_ui/trace.json');p.add_argument('--header',type=Path);args=p.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68')
    cases=fixtures(original)
    inputs=[{k:c[k] for k in ('routine','delta_bits','mutation','completion','parse','initial','pointers','globals')} for c in cases]
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),max_instructions=max(c['instructions'] for c in cases),
      input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),cases=cases)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    if args.header:args.header.write_text(golden(cases),encoding='utf-8',newline='\n')
    print(json.dumps({k:report[k] for k in ('fixtures','instructions','max_instructions')}))
if __name__=='__main__':main()
