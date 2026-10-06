"""Finite controlled-call camera fixtures from the validated original bodies.

Reuses the bounded scalar geometry decoder. Only synthetic memory is exported;
matrix and normalization calls are explicit controlled ABI substitutes. This
does not model EE exceptional arithmetic, FCR flags, timing or full runtime.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, rounded, scalar, word, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x2B59F8,0x2B5D08),(0x2B5D08,0x2B5EA8),
          (0x2B5EA8,0x2B5EF0),(0x2B5F10,0x2B5F18),(0x2B5F18,0x2B5F58))
MEMORY_RANGES = ((0x10000,0x11000),(0x20000,0x21000),(0x30000,0x31000),
                 (0x40000,0x41000),(0x7F000,0x81000))
OBJECT,TRANSFORM,INPUT,SECOND_TRANSFORM,SECOND_INPUT = 0x10000,0x20000,0x30000,0x20100,0x30100


class CameraTrace(Trace):
    def __init__(self, original, matrix=None, mutation=0):
        super().__init__(original)
        self.matrix = matrix
        self.mutation = mutation
        self.matrix_calls = self.normalize_calls = 0
        self.first_vector = None

    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start,end in RANGES):
            raise ValueError('unreviewed camera instruction: %#x' % pc)
        offset = pc - 0xFF000
        if offset < 0 or offset + 4 > len(self.original):
            raise ValueError('camera instruction outside original image')
        return struct.unpack_from('<I',self.original,offset)[0]

    def save(self, address, value, size):
        if not any(start <= address and address + size <= end for start,end in MEMORY_RANGES):
            raise ValueError('camera store outside initialized address windows')
        if any(address+offset not in self.memory for offset in range(size)):
            raise ValueError('camera store into uninitialized memory')
        return super().save(address,value,size)

    def execute(self, instruction, pc):
        op,rs,rt,rd,fn = instruction >> 26,(instruction >> 21)&31,(instruction >> 16)&31,(instruction >> 11)&31,instruction&63
        if op == 0 and fn in (0x21,0x2D,8):
            if (instruction >> 6)&31 or (fn == 8 and (rt != 0 or rd != 0)):
                raise ValueError('unsupported camera SPECIAL operand form')
        if op == 0x11:
            if rs in (0,4) and instruction & 0x7FF:
                raise ValueError('unsupported camera COP1 transfer operand form')
            if rs == 16:
                fd = (instruction >> 6)&31
                if fn in (6,7) and rt != 0:
                    raise ValueError('unsupported camera unary COP1 operand form')
                if fn == 4 and rd != 0:
                    raise ValueError('unsupported camera EE SQRT source form')
                if fn in (0x32,0x34,0x36) and fd != 0:
                    raise ValueError('unsupported camera comparison condition code')
                operands = (rt,) if fn == 4 else (rd,) if fn in (6,7) else (rd,rt)
                if any(not math.isfinite(self.f[index]) for index in operands):
                    raise ValueError('camera operation outside finite model')
        custom = False
        target = None
        if op == 0 and fn == 0x24:
            if (instruction >> 6)&31:
                raise ValueError('unsupported camera AND encoding')
            self.r[rd] = (self.r[rs] & self.r[rt]) & 0xFFFFFFFF
            custom = True
        elif op == 5:
            if self.r[rs] != self.r[rt]:
                imm = instruction & 0xFFFF
                if imm & 0x8000:imm -= 0x10000
                target = (pc + 4 + (imm << 2)) & 0xFFFFFFFF
            custom = True
        elif op == 0x11 and rs == 16 and fn in (0x28,0x34,0x36):
            fs,ft,fd = rd,rt,(instruction >> 6)&31
            if not math.isfinite(self.f[fs]) or not math.isfinite(self.f[ft]):
                raise ValueError('camera operation outside finite model')
            if fn == 0x28:
                left,right = word(self.f[fs]),word(self.f[ft])
                signed_left = left - 0x100000000 if left & 0x80000000 else left
                signed_right = right - 0x100000000 if right & 0x80000000 else right
                take_left = signed_left < signed_right if left & right & 0x80000000 else signed_left > signed_right
                self.f[fd] = self.f[fs] if take_left else self.f[ft]
            else:
                if fd != 0:
                    raise ValueError('unsupported camera comparison condition code')
                self.condition = self.f[fs] < self.f[ft] if fn == 0x34 else self.f[fs] <= self.f[ft]
            custom = True
        elif op == 0x11 and rs == 8 and rt not in (0,1,2,3):
            raise ValueError('unsupported camera branch condition code')
        if not custom:
            result = super().execute(instruction,pc)
            if op == 0x31 and not math.isfinite(self.f[rt]):
                raise ValueError('camera load outside finite model')
            if op == 0x11 and rs == 4 and not math.isfinite(self.f[rd]):
                raise ValueError('camera transfer outside finite model')
            return result
        self.instruction_count += 1
        if self.instruction_count > 3000:raise ValueError('instruction trace exceeded its bound')
        self.r[0] = 0
        return target,False

    def library_call(self, target):
        if target == 0x29A308:
            self.matrix_calls += 1
            if self.r[4] != TRANSFORM:raise ValueError('uncaptured camera transform argument')
            if self.r[5] == self.r[6] or self.matrix is None:raise ValueError('invalid matrix-pair output pointers')
            for index,value in enumerate(self.matrix):
                self.single(self.r[6]+index*4,value)
                self.single(self.r[5]+index*4,-value)
            if self.mutation == 1:self.save(OBJECT+8,SECOND_INPUT,4)
            if self.mutation == 2:self.save(OBJECT+4,SECOND_TRANSFORM,4)
        elif target == 0x2A3538:
            self.normalize_calls += 1
            pointer = self.r[4]
            values = [scalar(self.load(pointer+index*4,4)) for index in range(3)]
            square = rounded(rounded(rounded(values[0]*values[0])+rounded(values[1]*values[1]))+rounded(values[2]*values[2]))
            length = rounded(math.sqrt(square))
            if length == 0:values = [1.0,0.0,0.0]
            else:
                inverse = rounded(1.0/length)
                values = [rounded(value*inverse) for value in values]
            for index,value in enumerate(values):self.single(pointer+index*4,value)
            self.f[0] = length
            if self.normalize_calls == 1:self.first_vector = pointer
            if self.normalize_calls == 2:
                if self.mutation == 3:self.save(OBJECT+8,SECOND_INPUT,4)
                if self.mutation == 4:self.save(OBJECT+4,SECOND_TRANSFORM,4)
                if self.mutation == 5:
                    for index,value in enumerate((2.0,-3.0,4.0)):self.single(self.first_vector+index*4,value)
        else:raise ValueError('unreviewed controlled camera call: %#x' % target)
        self.r[2] = 0

    def run(self, entry):
        pc = entry
        while pc != RETURN:
            instruction = self.fetch(pc)
            target,annul = self.execute(instruction,pc)
            if target is not None or is_control_transfer(instruction) and not annul:
                delay = self.fetch(pc+4)
                if is_control_transfer(delay):raise ValueError('control transfer in camera delay slot')
                if self.execute(delay,pc+4) != (None,False):raise ValueError('control transfer in camera delay slot')
                if target in (0x29A308,0x2A3538):
                    self.library_call(target)
                    pc = self.r[31]
                else:pc = target if target is not None else pc + 8
            else:pc += 8 if annul else 4


def fixtures(original):
    cases = []
    matrices = [(2,0,0,0, 0,1,0,0, 0,0,3,0, 0,0,0,1),
                (1,2,2,0, 0,1,0,0, -2,1,2,0, 0,0,0,1),
                (0,0,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,1)]
    masks = (0,0x01400000,0x00800000,0x00400000,0x01000000,0x01C00000,0x00C00000)
    for variant,entry in enumerate((0x2B59F8,0x2B5D08)):
        for matrix in matrices:
            for held in (masks if variant == 0 else (0,)):
                for mutation in range(6):
                    trace = CameraTrace(original,matrix,mutation)
                    for start,end in MEMORY_RANGES:
                        for address in range(start,end):trace.memory[address]=0x5A
                    trace.save(OBJECT,variant,4);trace.save(OBJECT+4,TRANSFORM,4);trace.save(OBJECT+8,INPUT,4)
                    transform = [0x12345678]+[word(v) for v in (3,-2,5)]+[0x5A5A5A5A]*8+[word(v) for v in (8,-0.25,0.75)]+[0x5A5A5A5A]
                    for index,value in enumerate(transform):trace.save(TRANSFORM+index*4,value,4);trace.save(SECOND_TRANSFORM+index*4,value,4)
                    for pointer,axes in ((INPUT,(-0.75,0.5,0.25,-0.5)),(SECOND_INPUT,(1,-1,0.75,-0.25))):
                        trace.save(pointer+0x18,held,4)
                        for offset,value in zip((0x60,0x64,0x6C,0x70,0x78,0x7C,0x84,0x88),(axes[0],axes[0],axes[1],axes[1],axes[2],axes[2],axes[3],axes[3])):trace.single(pointer+offset,value)
                    trace.r[4],trace.r[29],trace.r[31] = OBJECT,0x80000,RETURN
                    elapsed = 0.125
                    trace.f[12] = elapsed
                    trace.run(entry)
                    cases.append({'variant':variant,'held':held,'mutation':mutation,'elapsed':word(elapsed),'matrix':[word(v) for v in matrix],'initial':transform,'expected':[trace.load(TRANSFORM+i*4,4) for i in range(16)],'transform_changed':trace.load(OBJECT+4,4)!=TRANSFORM,'input_changed':trace.load(OBJECT+8,4)!=INPUT,'matrix_calls':trace.matrix_calls,'normalize_calls':trace.normalize_calls,'instruction_count':trace.instruction_count})
    return cases


def golden_header(cases):
    lines=['/* Synthetic finite controlled-call fixtures; no original code/assets. */',
           'struct CameraMotionGolden { u32 variant,held,mutation,elapsed,matrix[16],initial[16],expected[16],transform_changed,input_changed,matrix_calls,normalize_calls; };',
           'static const struct CameraMotionGolden camera_motion_golden[] = {']
    for case in cases:
        array=lambda key:','.join('0x%08Xu'%value for value in case[key])
        lines.append('    {%d,0x%08Xu,%d,0x%08Xu,{%s},{%s},{%s},%d,%d,%d,%d},'%(case['variant'],case['held'],case['mutation'],case['elapsed'],array('matrix'),array('initial'),array('expected'),case['transform_changed'],case['input_changed'],case['matrix_calls'],case['normalize_calls']))
    return '\n'.join(lines+['};',''])


def fixture_input_hash(cases):
    inputs=[{key:case[key] for key in ('variant','held','mutation','elapsed','matrix','initial')}
            for case in cases]
    return hashlib.sha256(json.dumps(inputs,sort_keys=True,separators=(',',':')).encode('utf-8')).hexdigest()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/camera_motion_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':fixture_input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases))
    print('%d finite controlled-call camera fixtures; %d instructions; maximum %d'%(len(cases),sum(case['instruction_count'] for case in cases),max(case['instruction_count'] for case in cases)))
    print('synthetic input SHA-256 '+fixture_input_hash(cases))


if __name__=='__main__':main()
