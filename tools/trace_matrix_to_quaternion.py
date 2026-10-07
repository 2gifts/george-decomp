"""Bounded original matrix-to-quaternion scalar observations.

Nominal normal/zero host arithmetic, initialized scalar aliases and a separate
writable three-word table only. No general EE RTZ/FCR/exceptions/hardware or
original class/prototype identity. Original instructions are never exported.
"""
import argparse
import hashlib
import itertools
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace, RETURN, rounded, scalar, word, is_control_transfer
from trace_resource_base import ResourceBaseTrace

ROOT = Path(__file__).resolve().parents[1]
ENTRY, END = 0x2A0B00, 0x2A0CE0
BUFFER, BYTES, MATRIX, TABLE = 0x30000, 256, 0x30040, 0x3FC940
COUNT_LIMIT = 120
OUTPUTS = (BUFFER, MATRIX, MATRIX + 4, MATRIX + 16, MATRIX + 24, MATRIX + 36)


def nominal(value):
    """Validate an existing host binary32 normal/zero value before mutation."""
    if type(value) not in (int, float) or not math.isfinite(value):
        raise ValueError('outside nominal finite scalar domain')
    try:
        bits = word(value)
    except (OverflowError, struct.error) as error:
        raise ValueError('outside nominal scalar range') from error
    if bits & 0x7F800000 == 0x7F800000 or bits & 0x7FFFFFFF and not bits & 0x7F800000:
        raise ValueError('outside nominal normal/zero scalar domain')
    return value


class QuaternionTrace(ResourceBaseTrace):
    def __init__(self, original, output=BUFFER, matrix=MATRIX):
        super().__init__(original)
        for pointer, size in ((output, 16), (matrix, 64)):
            if type(pointer) is not int or pointer & 3 or not BUFFER <= pointer <= BUFFER + BYTES - size:
                raise ValueError('unowned quaternion prefix')
        self.output, self.matrix = output, matrix
        self.expected_words = {}
        for pc in range(ENTRY, END, 4):
            offset = pc - 0xFF000
            if 0 <= offset <= len(original) - 4:
                self.expected_words[pc] = struct.unpack_from('<I', original, offset)[0]
        self.visited = set()
        self.f_defined = set()
        self.writes = []
        self.operations = []
        self.routes = {'integer': 0, 'scalar': 0}
        self.active_pc = None

    def memory_check(self, address, size):
        if (type(address) is not int or size != 4 or address & 3 or
                not (BUFFER <= address <= BUFFER + BYTES - 4 or TABLE <= address <= TABLE + 8)):
            raise ValueError('unowned or unaligned quaternion memory')
        if any(address + i not in self.memory for i in range(4)):
            raise ValueError('uninitialized quaternion memory')

    def load(self, address, size):
        self.memory_check(address, size)
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        Trace.save(self, address, value, size)
        if self.active_pc is not None:
            self.writes.append(dict(pc=self.active_pc, address=address, bits=value & 0xFFFFFFFF))

    def fetch(self, pc):
        if pc not in self.expected_words:
            raise ValueError('unreviewed quaternion PC')
        offset = pc - 0xFF000
        instruction = struct.unpack_from('<I', self.original, offset)[0]
        if instruction != self.expected_words[pc]:
            raise ValueError('changed quaternion instruction')
        return instruction

    def _check(self, instruction, pc):
        # Every check is read-only and precedes the unchanged delegate's count,
        # register, condition or memory updates.
        if pc not in self.expected_words or instruction != self.expected_words[pc]:
            raise ValueError('unreviewed quaternion PC/word')
        if self.instruction_count >= COUNT_LIMIT or pc in self.visited:
            raise ValueError('quaternion instruction budget or repeated PC')
        if self.r[0] != 0:
            raise ValueError('quaternion nonzero register zero')
        op, rs, rt = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
        rd, sh, fn = instruction >> 11 & 31, instruction >> 6 & 31, instruction & 63
        imm = instruction & 65535
        simm = imm - 65536 if imm & 32768 else imm
        if op in (35, 49, 57):
            raw = self.r[rs] + simm
            if not 0 <= raw <= 0xFFFFFFFF:
                raise ValueError('quaternion address overflow')
            self.memory_check(raw, 4)
            if op == 35:
                if not TABLE <= raw <= TABLE + 8 or not 0 <= self.load(raw, 4) <= 2:
                    raise ValueError('invalid writable-table index/value')
            elif op == 49:
                if not self.matrix <= raw <= self.matrix + 60:
                    raise ValueError('quaternion matrix load outside prefix')
                nominal(scalar(self.load(raw, 4)))
            else:
                if not self.output <= raw <= self.output + 12:
                    raise ValueError('quaternion store outside output prefix')
                if rt not in self.f_defined:
                    raise ValueError('uninitialized quaternion float source')
                nominal(self.f[rt])
        elif op == 17:
            if rs == 4:
                if instruction & 0x7FF:
                    raise ValueError('reserved quaternion MTC1 form')
                nominal(scalar(self.r[rt] & 0xFFFFFFFF))
            elif rs == 8:
                if rt not in (0, 1, 3):
                    raise ValueError('reserved quaternion branch form')
                target = pc + 4 + simm * 4
                if target <= pc or not ENTRY <= target < END:
                    raise ValueError('escaped quaternion branch')
            elif rs == 16:
                if fn not in (0, 1, 2, 3, 4, 0x32, 0x34):
                    raise ValueError('unsupported quaternion scalar operation')
                if fn == 4 and rd or fn in (0x32, 0x34) and sh:
                    raise ValueError('reserved quaternion scalar form')
                operands = (rt,) if fn == 4 else (rd, rt)
                if any(i not in self.f_defined for i in operands):
                    raise ValueError('uninitialized quaternion float source')
                values = [nominal(self.f[i]) for i in operands]
                if fn == 3 and values[1] == 0:
                    raise ValueError('quaternion zero denominator')
                if fn == 4 and values[0] < 0:
                    raise ValueError('quaternion negative root')
                if fn in (0, 1, 2, 3, 4):
                    a = values[0]
                    b = values[1] if len(values) == 2 else None
                    value = (a + b if fn == 0 else a - b if fn == 1 else a * b if fn == 2 else
                             a / b if fn == 3 else math.sqrt(a))
                    try:
                        nominal(rounded(value))
                    except (OverflowError, struct.error) as error:
                        raise ValueError('quaternion arithmetic outside nominal range') from error
            else:
                raise ValueError('unsupported quaternion COP1 form')
        elif op == 0:
            if fn == 0:
                if rs:
                    raise ValueError('reserved quaternion shift form')
            elif fn in (0x21, 0x2D, 0x18):
                if sh:
                    raise ValueError('reserved quaternion SPECIAL form')
                if fn == 0x18 and (not 0 <= self.r[rs] <= 2 or self.r[rt] != 20):
                    raise ValueError('quaternion matrix index outside domain')
                if fn != 0x18 and not 0 <= self.r[rs] + self.r[rt] <= 0xFFFFFFFF:
                    raise ValueError('quaternion integer address overflow')
            elif fn == 8:
                if instruction != 0x03E00008 or self.r[31] != RETURN:
                    raise ValueError('unexpected quaternion return')
            else:
                raise ValueError('unsupported quaternion integer operation')
        elif op == 15:
            if rs:
                raise ValueError('reserved quaternion LUI form')
        elif op == 9:
            if not 0 <= self.r[rs] + simm <= 0xFFFFFFFF:
                raise ValueError('quaternion integer immediate overflow')
        else:
            raise ValueError('unsupported quaternion instruction family')

    def execute(self, instruction, pc):
        self._check(instruction, pc)
        op, rs, rt = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
        fd, fs, fn = instruction >> 6 & 31, instruction >> 11 & 31, instruction & 63
        old_count = self.instruction_count
        operation = None
        if op == 17 and rs == 16 and fn in (0, 1, 2, 3, 4):
            inputs = (rt,) if fn == 4 else (fs, rt)
            operation = dict(pc=pc, function=fn, operands=[word(self.f[i]) for i in inputs], destination=fd)
        self.active_pc = pc
        scalar_route = op in (17, 49, 57)
        if scalar_route:
            result = CameraTrace.execute(self, instruction, pc)
        else:
            result = ResourceBaseTrace.execute(self, instruction, pc)
        self.active_pc = None
        assert self.instruction_count == old_count + 1 and self.r[0] == 0
        self.visited.add(pc)
        self.routes['scalar' if scalar_route else 'integer'] += 1
        if op == 49:
            self.f_defined.add(rt)
        elif op == 17 and rs == 4:
            self.f_defined.add(fs)
        elif op == 17 and rs == 16 and fn in (0, 1, 2, 3, 4):
            self.f_defined.add(fd)
            operation['result'] = word(self.f[fd])
            self.operations.append(operation)
        return result

    def advance(self, pc):
        instruction = self.fetch(pc)
        self._check(instruction, pc)
        control = is_control_transfer(instruction)
        annul = (instruction >> 26 == 17 and instruction >> 21 & 31 == 8 and
                 instruction >> 16 & 3 == 3 and not self.condition)
        if control and not annul:
            delay = self.fetch(pc + 4)
            if is_control_transfer(delay):
                raise ValueError('encoded control transfer in quaternion delay')
            if self.instruction_count + 2 > COUNT_LIMIT:
                raise ValueError('quaternion delay instruction budget')
            self._check(delay, pc + 4)
        target, actual_annul = self.execute(instruction, pc)
        assert actual_annul == annul
        if control and not annul:
            if self.execute(delay, pc + 4) != (None, False):
                raise ValueError('quaternion transfer in delay')
            return target if target is not None else pc + 8
        return pc + (8 if annul else 4)

    def run(self, entry=ENTRY):
        if entry != ENTRY or self.r[31] != RETURN:
            raise ValueError('unreviewed quaternion entry/return')
        pc = entry
        while pc != RETURN:
            pc = self.advance(pc)


def cube_rotations():
    result = []
    for permutation in itertools.permutations(range(3)):
        inversions = sum(permutation[i] > permutation[j] for i in range(3) for j in range(i + 1, 3))
        parity = -1 if inversions & 1 else 1
        for signs in itertools.product((-1, 1), repeat=3):
            if parity * math.prod(signs) != 1:
                continue
            m = [0.0] * 16
            m[15] = 1.0
            for i in range(3):
                m[4 * i + permutation[i]] = float(signs[i])
            result.append(m)
    assert len(result) == 24
    return result


def make_fixture(original, matrix, table, output, label):
    if (len(matrix) != 16 or len(table) != 3 or any(type(v) is not int or not 0 <= v <= 2 for v in table)):
        raise ValueError('invalid initialized quaternion fixture')
    t = QuaternionTrace(original, output)
    for i in range(BYTES // 4):
        bits = word((i % 11 - 5) / 4.0)
        for j, byte in enumerate(bits.to_bytes(4, 'little')):
            t.memory[BUFFER + i * 4 + j] = byte
    for i, value in enumerate(matrix):
        nominal(value)
        Trace.save(t, MATRIX + i * 4, word(value), 4)
    for i, value in enumerate(table):
        Trace.save(t, TABLE + i * 4, value, 4)
    initial = [t.load(BUFFER + 4 * i, 4) for i in range(BYTES // 4)]
    t.r[4], t.r[5], t.r[31] = output, MATRIX, RETURN
    t.run()
    return dict(label=label, output_word=(output - BUFFER) // 4, matrix_word=(MATRIX - BUFFER) // 4,
                table=table, initial=initial, expected=[t.load(BUFFER + 4 * i, 4) for i in range(BYTES // 4)],
                stores=t.writes, operations=t.operations, instructions=t.instruction_count,
                visited=sorted(t.visited), routes=t.routes,
                table_final=[t.load(TABLE + i * 4, 4) for i in range(3)])


def fixtures(original):
    cases = []
    for i, matrix in enumerate(cube_rotations()):
        for j, output in enumerate(OUTPUTS):
            cases.append(make_fixture(original, matrix, [1, 2, 0], output, 'cube_%d_alias_%d' % (i, j)))
    targeted = []
    for diagonal, table in (((1, -2, -2), (0, 1, 2)),
                            ((1, -2, -2), (0, 0, 0)),
                            ((-1, -1, -1), (1, 2, 0)),
                            ((-1, -1, -1), (2, 0, 1)),
                            ((0, 0, 0), (1, 2, 0)),
                            ((0, 0, 0), (0, 1, 2))):
        m = [0.0] * 16
        m[0], m[5], m[10], m[15] = (*map(float, diagonal), 1.0)
        m[1], m[2], m[4], m[6], m[8], m[9] = (0.25, -0.5, 0.75, 1.0, -1.0, 1.5)
        targeted.append((m, list(table)))
    for i, (matrix, table) in enumerate(targeted):
        for j, output in enumerate((BUFFER, MATRIX, MATRIX + 4, MATRIX + 36)):
            cases.append(make_fixture(original, matrix, table, output, 'target_%d_alias_%d' % (i, j)))
    assert len(cases) <= 192
    keys = [json.dumps({k: c[k] for k in ('output_word', 'matrix_word', 'table', 'initial')}, sort_keys=True) for c in cases]
    assert len(keys) == len(set(keys)), 'Unique initial parameters, not labels'
    return cases


def golden_header(cases):
    lines = ['/* Synthetic initialized scalar effects; no original code/data arrays. */',
             'struct MatrixQuaternionGolden { u32 output_word,matrix_word; s32 table[3]; u32 initial[64],expected[64]; };',
             'static const struct MatrixQuaternionGolden matrix_quaternion_golden[] = {']
    for c in cases:
        array = lambda k: ','.join('0x%08Xu' % b for b in c[k])
        lines.append(' {%d,%d,{%s},{%s},{%s}},' % (c['output_word'], c['matrix_word'],
                     ','.join(str(i) for i in c['table']), array('initial'), array('expected')))
    return '\n'.join(lines + ['};', ''])


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, default=ROOT / 'build/matrix_to_quaternion/trace.json')
    p.add_argument('--golden-header', type=Path)
    args = p.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    inputs = [{k: c[k] for k in ('output_word', 'matrix_word', 'table', 'initial')} for c in cases]
    data = dict(fixtures=len(cases), unique_fixtures=len(cases), instructions=sum(c['instructions'] for c in cases),
                max_instructions=max(c['instructions'] for c in cases),
                coverage=len({pc for c in cases for pc in c['visited']}),
                input_sha256=hashlib.sha256(json.dumps(inputs, sort_keys=True).encode()).hexdigest(),
                limitation=__doc__, cases=cases)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(data, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), newline='\n')
    print('Quaternion %d unique nominal cases/%d instructions/%d PCs' % (len(cases), data['instructions'], data['coverage']))


if __name__ == '__main__':
    main()
