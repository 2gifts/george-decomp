"""Bounded original record queries, segment distance and supporting math.

The two selected bodies and five scalar/VU helpers execute original code.
Four soft-double ABI calls have the previously reviewed finite normal/zero
models. The VU accumulator uses rounded binary32 products/adds; this does not
claim EE FCR, VU extended accumulator precision, nonfinite/subnormal or timing.
No original instruction or asset arrays are exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, rounded, scalar, word, is_control_transfer
from trace_camera_motion import CameraTrace
from trace_resource_base import ResourceBaseTrace
from trace_segment_distance import (SegmentTrace, CALLS as SOFT_CALLS,
                                    double_bits, double_value, normal_single, MASK64)

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x270A00, 0x270C90), (0x270C90, 0x270D98))
HELPERS = ((0x2A1098, 0x2A11A4), (0x2A1C60, 0x2A1CDC),
           (0x2A1D78, 0x2A1DEC), (0x2A3538, 0x2A35C0),
           (0x2A35C0, 0x2A3640), (0x29D5D0, 0x29DD5C))
ENTRIES = tuple(a for a, _ in RANGES)
BUFFER, END = 0x20000, 0x21600
OBJECT, RECORDS, MAP, BASIS, FRAME = 0x20000, 0x20800, 0x20A00, 0x20C00, 0x21000
FIRST, SECOND, FRACTION, ID, NORMAL = 0x21100, 0x21120, 0x21200, 0x21210, 0x21220
WORDS = (END - BUFFER) // 4
MEMORY_RANGES = ((BUFFER, END), (0x7E000, 0x81000))
POINTER_CELLS = tuple(OBJECT + p for p in (0x388, 0x38C, 0x390, 0x394, 0x398,
                                        0x3D8, 0x3DC, 0x3E0, 0x3E4))
MASK128 = (1 << 128) - 1


class RecordTrace(ResourceBaseTrace):
    def __init__(self, original):
        super().__init__(original)
        self.vf = [[0.0] * 4 for _ in range(32)]
        self.vf[0][3] = 1.0
        self.vu_acc = None
        self.events = []
        self.helper_calls = [0] * len(HELPERS)
        self.soft_calls = [0] * 4
        self.segment_active = False
        self.regions = set()
        self.branch_outcomes = []
        # Reuse the reviewed class's complete decoder with shared register/
        # memory state; its lexical super() remains bound to SegmentTrace.
        self.segment_decoder = SegmentTrace.__new__(SegmentTrace)
        self.segment_decoder.__dict__ = self.__dict__

    def memory_check(self, address, size):
        if (size not in (2, 4, 8, 16) or address % size or
                not any(a <= address and address + size <= b for a, b in MEMORY_RANGES) or
                any(address + i not in self.memory for i in range(size))):
            raise ValueError('record memory outside initialized aligned windows')

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in (*RANGES, *HELPERS)):
            raise ValueError('unreviewed record instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('record instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def execute(self, instruction, pc):
        if self.segment_active:
            # The reviewed segment decoder dispatches to its scalar base, not
            # this method, and keeps its separate whole-invocation bound.
            return self.segment_decoder.execute(instruction, pc)
        if self.instruction_count >= 3000:
            raise ValueError('record instruction bound exceeded')
        op, rs, rt, rd, fn = (instruction >> 26, instruction >> 21 & 31,
                              instruction >> 16 & 31, instruction >> 11 & 31, instruction & 63)
        shift = instruction >> 6 & 31
        custom = False
        if op == 0 and fn in (0x0B, 0x26, 0x3E):
            if fn == 0x3E:
                if rs:
                    raise ValueError('reserved record DSRL32 source')
                self.r[rd] = (self.r[rt] & MASK64) >> (32 + shift)
            else:
                if shift:
                    raise ValueError('reserved record SPECIAL shift')
                if fn == 0x0B:
                    if self.r[rt] != 0:
                        self.r[rd] = self.r[rs]
                else:
                    self.r[rd] = (self.r[rs] ^ self.r[rt]) & MASK64
            custom = True
        elif op == 1:
            if rt != 0:
                raise ValueError('unsupported record REGIMM')
            value = self.r[rs] & 0xFFFFFFFF
            signed = value - 0x100000000 if value & 0x80000000 else value
            immediate = instruction & 65535
            if immediate & 32768:
                immediate -= 65536
            self.instruction_count += 1
            self.r[0] = 0
            return (pc + 4 + immediate * 4, False) if signed < 0 else (None, False)
        elif op == 0x0E:
            self.r[rt] = (self.r[rs] ^ (instruction & 65535)) & MASK64
            custom = True
        elif op == 0x1C:
            if instruction & 0x7FF == 0x488:  # PEXTLW
                first, second = self.r[rs], self.r[rt]
                lanes = (second & 0xFFFFFFFF, first & 0xFFFFFFFF,
                         second >> 32 & 0xFFFFFFFF, first >> 32 & 0xFFFFFFFF)
                self.r[rd] = sum(v << (32 * i) for i, v in enumerate(lanes))
            elif instruction & 0x7FF == 0x3A9:  # PCPYUD
                self.r[rd] = ((self.r[rt] >> 64) & MASK64) | ((self.r[rs] >> 64 & MASK64) << 64)
            else:
                raise ValueError('unsupported record MMI encoding')
            custom = True
        elif op == 0x36:  # LQC2 captures all four float lanes.
            immediate = instruction & 65535
            if immediate & 32768:
                immediate -= 65536
            bits = self.load((self.r[rs] + immediate) & 0xFFFFFFFF, 16)
            self.vf[rt] = [normal_single(scalar(bits >> (i * 32) & 0xFFFFFFFF)) for i in range(4)]
            custom = True
        elif op == 18:
            if rs in (1, 5):
                if instruction & 0x7FF:
                    raise ValueError('reserved record quad COP2 transfer')
                if rs == 5:
                    self.vf[rd] = [normal_single(scalar(self.r[rt] >> (i * 32) & 0xFFFFFFFF)) for i in range(4)]
                else:
                    self.r[rt] = sum(word(v) << (i * 32) for i, v in enumerate(self.vf[rd]))
            else:
                # Only the complete supporting bodies' xyzw broadcast forms.
                base = instruction & ~((31 << 16) | (31 << 11) | (31 << 6))
                if rs != 31 or base not in (0x4BE0003C, 0x4BE0003D, 0x4BE0003E,
                                           0x4BE0000A, 0x4BE0000B):
                    raise ValueError('unsupported record VU broadcast encoding')
                if fn in (0x3C, 0x3D, 0x3E) and shift != (6 if fn == 0x3C else 2):
                    raise ValueError('unsupported record accumulator destination')
                broadcast = fn & 3
                products = [normal_single(rounded(self.vf[rd][i] * self.vf[rt][broadcast])) for i in range(4)]
                if fn == 0x3C:
                    self.vu_acc = products
                else:
                    if self.vu_acc is None:
                        raise ValueError('record VU accumulator read before producer')
                    result = [normal_single(rounded(self.vu_acc[i] + products[i])) for i in range(4)]
                    if fn in (0x3D, 0x3E):
                        self.vu_acc = result
                    else:
                        self.vf[shift] = result
            custom = True
        if custom:
            self.instruction_count += 1
            self.r[0] = 0
            self.vf[0] = [0.0, 0.0, 0.0, 1.0]
            return None, False
        if op in (0x31, 0x39, 17):
            if op == 17 and not (rs in (0, 4) or rs == 8 and rt in (0, 1, 2, 3) or
                                 rs == 16 and fn in (0, 1, 2, 3, 4, 6, 7, 0x32, 0x34, 0x36)):
                raise ValueError('unsupported record COP1 encoding')
            if op == 17 and rs == 16:
                operands = (rt,) if fn == 4 else (rd,) if fn in (6, 7) else (rd, rt)
                for index in operands:
                    normal_single(self.f[index])
                if fn == 3 and self.f[rt] == 0:
                    raise ValueError('record zero denominator')
                if fn == 4 and self.f[rt] < 0:
                    raise ValueError('record negative square root')
            result = CameraTrace.execute(self, instruction, pc)
            if op == 0x31 or op == 17 and rs in (4, 16) and fn not in (0x32, 0x34, 0x36):
                index = rt if op == 0x31 else rd if rs == 4 else shift
                normal_single(self.f[index])
            return result
        return ResourceBaseTrace.execute(self, instruction, pc)

    def library_call(self, target):
        if target not in SOFT_CALLS:
            raise ValueError('unknown record controlled soft call')
        index = SOFT_CALLS.index(target)
        self.soft_calls[index] += 1
        a, b = self.r[4] & MASK64, self.r[5] & MASK64
        if target == 0x374848:
            a = b = 0
        elif target == 0x3734F8:
            b = 0
        self.events.extend((index, a & 0xFFFFFFFF, a >> 32, b & 0xFFFFFFFF,
                            b >> 32, word(self.f[12]) if target == 0x374848 else 0))
        if target == 0x374848:
            self.r[2] = double_bits(normal_single(self.f[12]))
        elif target == 0x373250:
            first, second = double_value(a), double_value(b)
            self.r[2] = ((first > second) - (first < second)) & MASK64
        elif target == 0x372CC0:
            self.r[2] = double_bits(double_value(a) - double_value(b))
        else:
            self.f[0] = normal_single(rounded(double_value(a)))

    def run(self, entry, stop=RETURN):
        body = next((r for r in (*RANGES, *HELPERS) if r[0] == entry), None)
        if body is None:
            raise ValueError('record invocation requires whole approved entry')
        segment = entry == 0x29D5D0
        parent_count = self.instruction_count
        self.segment_active = segment
        if segment:
            self.instruction_count = 0
        pc = entry
        while True:
            if not body[0] <= pc < body[1]:
                raise ValueError('record transfer outside current complete body')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if not annul:
                    if pc + 4 >= body[1]:
                        raise ValueError('record delay outside complete body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('record control transfer in delay')
                if instruction >> 26 == 0 and instruction & 63 == 8:
                    if instruction != 0x03E00008 or target != stop:
                        raise ValueError('record return requires actual JR31 to stop')
                    if segment:
                        self.instruction_count += parent_count
                    self.segment_active = False
                    return
                if instruction >> 26 == 3:
                    continuation = self.r[31]
                    if target in tuple(a for a, _ in HELPERS):
                        self.helper_calls[tuple(a for a, _ in HELPERS).index(target)] += 1
                        if target in (0x2A1C60, 0x2A1D78):
                            a, b = self.r[4] & 0xFFFFFFFF, self.r[5] & 0xFFFFFFFF
                            self.events.extend((4 if target == 0x2A1C60 else 5,
                                                *[self.load(a + 4 * i, 4) for i in range(16)],
                                                *[self.load(b + 4 * i, 4) for i in range(3)]))
                        self.run(target, continuation)
                    else:
                        self.library_call(target)
                    self.segment_active = segment
                    if self.r[31] != continuation:
                        raise ValueError('record callee corrupted return register')
                    pc = continuation
                else:
                    if target is not None and not body[0] <= target < body[1]:
                        raise ValueError('record local branch outside complete body')
                    pc = target if target is not None else pc + 8
            else:
                pc += 8 if annul else 4


def fixture(original, routine=0, selector=0, record_keys=(1,), mapped=(0, 1, 2),
            query=1, radius=0.5, length=3.0, offset=0.0, translate=(0, 0, 0),
            first=(-2, 1, 0), second=(2, 1, 0), outputs=(FRACTION, ID, NORMAL),
            output=NORMAL, table=True):
    t = RecordTrace(original)
    for a, b in MEMORY_RANGES:
        for p in range(a, b):
            t.memory[p] = 0
    t.save(OBJECT + 0x0C, selector, 4)
    for i in range(4):
        t.save(OBJECT + 0x388 + 4 * i, MAP + i * 16, 4)
        t.save(OBJECT + 0x3D8 + 4 * i, BASIS + i * 256, 4)
        for j, m in enumerate(mapped):
            t.save(MAP + i * 16 + j * 2, m, 2)
        for j in range(3):
            values = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, j * 3, i - 1, -1, 1)
            for k, value in enumerate(values):
                t.single(BASIS + i * 256 + j * 64 + k * 4, value)
    t.save(OBJECT + 0x398, RECORDS if table else 0, 4)
    t.save(RECORDS, len(record_keys), 4)
    for i, key in enumerate(record_keys):
        p = RECORDS + 4 + i * 20
        t.save(p, i, 4)
        t.save(p + 4, key, 4)
        t.single(p + 8, length)
        t.single(p + 12, radius)
        t.single(p + 16, offset)
    for i, value in enumerate((1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, *translate, 1)):
        t.single(FRAME + 4 * i, value)
    for pointer, values in ((FIRST, first), (SECOND, second)):
        for i, value in enumerate(values):
            t.single(pointer + 4 * i, value)
    for p in (FRACTION, ID, NORMAL, NORMAL + 4, NORMAL + 8):
        t.save(p, 0xBF800000, 4)
    initial = [t.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    args = (OBJECT, FRAME, FIRST, SECOND, *outputs) if routine == 0 else (OBJECT, output, query)
    t.r[4:4 + len(args)] = args
    t.r[29], t.r[31] = 0x80000, RETURN
    t.run(ENTRIES[routine])
    expected = [t.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    return dict(routine=routine, args=args, initial=initial, expected=expected,
                result=t.r[2] & 0xFFFFFFFF if routine == 0 else 0, events=t.events,
                soft_calls=t.soft_calls, instruction_count=t.instruction_count)


def fixtures(original):
    cases = []
    for selector in range(4):
        for records in ((), (1,), (2, 1), (2, 3, 1), (1, 1, 1)):
            for mapping in ((0, 1, 2), (-1, 0, 1), (-1, -1, -1)):
                for radius in (0.0, 0.5, 2.0, -2.0):
                    cases.append(fixture(original, selector=selector, record_keys=records, mapped=mapping, radius=radius))
                for query in (0, 1, 2, 3, 0xFFFFFFFF):
                    cases.append(fixture(original, routine=1, selector=selector, record_keys=records, mapped=mapping, query=query))
    for radius in (0, 0.5, 1, 2):
        for translate in ((0, 0, 0), (1, -2, 3), (-3, 1, -1)):
            for first, second in (((-2, -1, 0), (2, -1, 0)), ((0, 0, 0), (0, 0, 0)),
                                  ((0, 0, -3), (0, 0, 3)), ((1, 2, 3), (-2, -1, -3))):
                cases.append(fixture(original, radius=radius, translate=translate, first=first, second=second))
    for length in (0, -3, 0.5, 3):
        for offset in (-2, 0, 1.5):
            for outputs in ((FRACTION, ID, NORMAL), (FIRST, ID, NORMAL),
                            (RECORDS + 8, ID, NORMAL), (FRACTION, FRACTION, NORMAL),
                            (FRACTION, NORMAL, NORMAL), (FRACTION, ID, FIRST),
                            (FRACTION, ID, SECOND), (FRACTION, ID, RECORDS + 8)):
                cases.append(fixture(original, length=length, offset=offset, radius=4, outputs=outputs))
    for output in (NORMAL, OBJECT + 0x0C, OBJECT + 0x398, RECORDS, RECORDS + 8,
                   BASIS + 0x20, BASIS + 0x30, FRAME + 0x30):
        for selector in range(4):
            cases.append(fixture(original, routine=1, selector=selector, output=output))
    for routine in range(2):
        cases.append(fixture(original, routine=routine, table=False))
    return cases


def golden_header(cases):
    lines = ['/* Synthetic inputs/outputs only; no original code or assets. */',
             'struct RecordWord { u16 index; u32 value; };',
             'struct RecordGolden { u32 args[7], result; unsigned routine, initial_count, changed_count, event_count; const struct RecordWord *initial,*changed; const u32 *events; };']
    for i, case in enumerate(cases):
        initial = [(j, value) for j, value in enumerate(case['initial']) if value]
        changed = [(j, value) for j, value in enumerate(case['expected']) if value != case['initial'][j]]
        for label, pairs in (('initial', initial), ('changed', changed)):
            values = ','.join('{%d,0x%08Xu}' % p for p in pairs) or '{0,0}'
            lines.append('static const struct RecordWord record_%s_%d[] = {%s};' % (label, i, values))
        lines.append('static const u32 record_events_%d[] = {%s};' % (i, ','.join('0x%08Xu' % v for v in case['events']) or '0'))
    lines.append('static const struct RecordGolden record_golden[] = {')
    for i, case in enumerate(cases):
        arguments = list(case['args']) + [0] * (7 - len(case['args']))
        lines.append('    {{%s},0x%08Xu,%d,%d,%d,%d,record_initial_%d,record_changed_%d,record_events_%d},' %
                     (','.join('0x%08Xu' % a for a in arguments), case['result'], case['routine'],
                      sum(v != 0 for v in case['initial']), sum(a != b for a, b in zip(case['initial'], case['expected'])),
                      len(case['events']), i, i, i))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'build/record_collision/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    header = golden_header(cases)
    report = dict(fixtures=cases, fixture_count=len(cases),
                  executed_original_instructions=sum(c['instruction_count'] for c in cases),
                  maximum_instructions=max(c['instruction_count'] for c in cases),
                  input_sha256=hashlib.sha256(json.dumps([{k: v for k, v in c.items() if k in ('routine', 'args', 'initial')}
                                                         for c in cases], sort_keys=True).encode()).hexdigest(),
                  header_sha256_lf=hashlib.sha256(header.encode()).hexdigest(),
                  limitation=__doc__)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(header)
    print('RECORD FIXTURES', len(cases), report['executed_original_instructions'], report['maximum_instructions'])


if __name__ == '__main__':
    main()
