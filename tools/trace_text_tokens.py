"""Generate synthetic byte/alias fixtures for two reviewed text helpers.

Instructions are read from the hash-validated local ELF, never embedded. The
bounded decoder reuses trace_geometry's scalar core; only byte/branch operations
needed by these two bodies are added. Length/compare are controlled ordinary
byte substitutes, not models of the original vectorized library implementation.
No hardware timing, exceptions, stack-address exposure or invalid-memory paths
are claimed. All fixture memory must be initialized and every write is bounded.
"""
import argparse
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x2B3190, 0x2B3460), (0x2B3EE0, 0x2B4188))
BUFFER, CONTEXT, INPUT_CELL, OUTPUT_CELL = 0x10000, 0x20000, 0x30000, 0x30004
QUOTES = 0x4476C8
TABLES = (0x3FD258, 0x3FD270, 0x3FD288)
WORDS = (b"STOP", b"END", b"?", b"!", b";", b"Q", b"R", b"L", b"Z", b"", b"ab", b"xy")
WORD_BASE = 0x40000
MODE_TABLES = ((2, -1, 0, -1, -1), (3, -1, 1, -1, -1), (4, -1, -1, -1, -1))


def signed32(value):
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


class TextTrace(Trace):
    def __init__(self, original, mutation=0):
        super().__init__(original)
        self.mutation = mutation
        self.length_calls = 0
        self.compare_calls = 0
        self.calls = []

    def save(self, address, value, size):
        if any(((address + i) & 0xFFFFFFFF) not in self.memory for i in range(size)):
            raise ValueError('write outside initialized fixture memory: %#x' % address)
        super().save(address, value, size)

    def initialize(self, address, data):
        if any(address + i in self.memory for i in range(len(data))):
            raise ValueError('overlapping fixture initialization')
        self.memory.update({address + i: byte for i, byte in enumerate(data)})

    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed text instruction: %#x' % pc)
        return struct.unpack_from('<I', self.original, pc - 0xFF000)[0]

    def execute(self, instruction, pc):
        op, rs, rt, rd = instruction >> 26, (instruction >> 21) & 31, (instruction >> 16) & 31, (instruction >> 11) & 31
        imm = instruction & 0xFFFF
        signed_imm = imm - 0x10000 if imm & 0x8000 else imm
        fn, shift = instruction & 63, (instruction >> 6) & 31
        extra = op in (5, 0x14, 0x15, 0x0B, 0x0E, 0x20, 0x24, 0x28) or (op == 0 and fn in (0, 3, 0x0A, 0x0B, 0x2B) and instruction != 0)
        if not extra:
            return super().execute(instruction, pc)
        self.instruction_count += 1
        if self.instruction_count > 3000:
            raise ValueError('text instruction trace exceeded its bound')
        target, annul = None, False
        if op in (5, 0x14, 0x15):
            equal = self.r[rs] == self.r[rt]
            taken = equal if op == 0x14 else not equal
            if taken:
                target = (pc + 4 + (signed_imm << 2)) & 0xFFFFFFFF
            elif op != 5:
                annul = True
        elif op == 0x0B:
            self.r[rt] = int((self.r[rs] & 0xFFFFFFFF) < (signed_imm & 0xFFFFFFFF))
        elif op == 0x0E:
            self.r[rt] = (self.r[rs] ^ imm) & 0xFFFFFFFF
        elif op in (0x20, 0x24):
            byte = self.load((self.r[rs] + signed_imm) & 0xFFFFFFFF, 1)
            self.r[rt] = (byte - 256 if op == 0x20 and byte >= 128 else byte) & 0xFFFFFFFF
        elif op == 0x28:
            self.save((self.r[rs] + signed_imm) & 0xFFFFFFFF, self.r[rt], 1)
        elif fn == 0:
            self.r[rd] = (self.r[rt] << shift) & 0xFFFFFFFF
        elif fn == 3:
            self.r[rd] = (signed32(self.r[rt]) >> shift) & 0xFFFFFFFF
        elif fn in (0x0A, 0x0B):
            if (self.r[rt] == 0) == (fn == 0x0A):
                self.r[rd] = self.r[rs]
        elif fn == 0x2B:
            self.r[rd] = int((self.r[rs] & 0xFFFFFFFF) < (self.r[rt] & 0xFFFFFFFF))
        else:
            raise AssertionError('unhandled scoped instruction')
        self.r[0] = 0
        return target, annul

    def library_call(self, target):
        if target == 0x295050:
            pointer = self.r[4]
            length = 0
            while self.load(pointer + length, 1):
                length += 1
                if length > 128:
                    raise ValueError('controlled string length exceeds its bound')
            self.length_calls += 1
            self.calls.append((0, pointer, 0, length))
            if self.length_calls == 1:
                if self.mutation == 1:
                    self.save(TABLES[0], WORD_BASE + 32 * 10, 4)
                elif self.mutation == 2:
                    self.save(TABLES[0] + 4, WORD_BASE + 32 * 8, 4)
                elif self.mutation == 3:
                    self.save(CONTEXT + 0x18, 0xFFFFFFFF, 4)
            self.r[2] = length
        elif target == 0x393E48:
            left, right, count = self.r[4], self.r[5], self.r[6] & 0xFFFFFFFF
            if count > 128:
                raise ValueError('controlled compare exceeds its bound')
            result = 0
            for index in range(count):
                first, second = self.load(left + index, 1), self.load(right + index, 1)
                if first != second:
                    result = first - second
                    break
                if first == 0:
                    break
            self.compare_calls += 1
            self.calls.append((1, left, right, count))
            if self.compare_calls == 1 and self.mutation == 4:
                self.save(left, 0, 1)
            self.r[2] = result & 0xFFFFFFFF
        else:
            raise ValueError('unreviewed controlled call: %#x' % target)

    def run(self, entry):
        pc = entry
        while pc != RETURN:
            target, annul = self.execute(self.fetch(pc), pc)
            if target is not None:
                nested = self.execute(self.fetch(pc + 4), pc + 4)
                if nested != (None, False):
                    raise ValueError('control transfer in delay slot')
                if target in (0x295050, 0x393E48):
                    self.library_call(target)
                    pc = self.r[31]
                else:
                    pc = target
            else:
                pc += 8 if annul else 4


def setup(original, text, mutation):
    trace = TextTrace(original, mutation)
    data = bytearray([0x7E] * 128)
    if len(text) > 56:
        raise ValueError('synthetic text fixture is too long')
    data[32:32 + len(text)] = text
    data[32 + len(text):96] = bytes(64 - len(text))
    trace.initialize(BUFFER, data)
    trace.initialize(CONTEXT, bytes(32))
    trace.initialize(INPUT_CELL, bytes(8))
    trace.initialize(0x7F000, bytes(0x1000))
    trace.initialize(QUOTES, bytes((0, 39, 34)))
    for index, word in enumerate(WORDS):
        trace.initialize(WORD_BASE + index * 32, word + bytes(32 - len(word)))
    for address, cells in zip(TABLES, MODE_TABLES):
        trace.initialize(address, bytes(20))
        for index, word in enumerate(cells):
            trace.save(address + index * 4, 0 if word < 0 else WORD_BASE + word * 32, 4)
    trace.r[29], trace.r[31] = 0x80000, RETURN
    return trace


def fixtures(original):
    cases = []
    token_texts = (b'', b' \t\n\rA?rest', b'a\nb\rc?tail', b'"two words"?x', b"'a?b'?tail",
                   b'"?abc"rest', b'"abc', b'""?tail', b'a\x80\xff?x', b'Z\n?', b'STOPtail')
    specs = []
    for text in token_texts:
        for mode in (-1, 0, 1, 2, 3):
            for keep in (0, 1):
                specs.append((0, text, 0, 0, mode, keep, 0, 0, 64, 0))
    for destination in (0, 24, 30, 32):
        for keep in (0, 1):
            specs.append((0, b" \n'ab\ncd'?tail", destination, 0, 0, keep, 0, 0, 64, 0))
    for length in (0, 1, 2, 3, 4):
        specs.append((0, b' \n\r A?end', 0, 0, 0, 0, 0, 0, length, 0xFFFFFFFF))
    # A changed pointer retains the first delimiter's original length. A later
    # entry mutation must be observed when that cell is subsequently visited.
    for mutation, text in ((1, b'a!xx?tail'), (2, b'Z?tail'), (3, b'Z\n?tail'), (4, b'ZQ?tail')):
        specs.append((0, text, 0, 0, 0, 0, mutation, 0, 64, 0))
    specs.append((0, b'abc?tail', 32, 0, 0, 0, 0, 1, 64, 0))

    attribute_texts = (b'', b' \t\r\n', b'name', b'name=', b'name=x tail=y',
                       b' name = "two words" tail=z', b"a=' x\n\ry ' z=3", b'a=""',
                       b'a="   "', b'a="unclosed', b'a==\tvalue', b"a='='b",
                       b'a=x"y', b'\x80\xff=\xfe\x81 tail=2')
    for text in attribute_texts:
        specs.append((1, text, 0, 8, 0, 0, 0, 0, 64, 0))
    for name in (0, 32, 34, 48, 60):
        # Forward value aliases at 41/48 can overwrite their own terminator
        # indefinitely. Such invalid-buffer paths are deliberately excluded.
        for value in (0, 8, 32, 35, 39, 60):
            specs.append((1, b'  key="two words" tail=z', name, value, 0, 0, 0, 0, 64, 0))
    for routine, text, first, second, mode, keep, mutation, same_cell, length, line in specs:
        trace = setup(original, text, mutation)
        initial = [trace.load(BUFFER + index, 1) for index in range(128)]
        if routine == 0:
            trace.save(CONTEXT + 8, BUFFER + 32, 4)
            trace.save(CONTEXT + 0x10, length, 4)
            trace.save(CONTEXT + 0x18, line, 4)
            trace.save(INPUT_CELL, BUFFER + 32, 4)
            trace.save(OUTPUT_CELL, BUFFER + first, 4)
            trace.r[4:9] = (CONTEXT, INPUT_CELL, INPUT_CELL if same_cell else OUTPUT_CELL,
                            mode & 0xFFFFFFFF, keep & 0xFFFFFFFF)
            trace.run(0x2B3190)
            input_end = trace.load(INPUT_CELL, 4) - BUFFER
            output_end = trace.load(INPUT_CELL if same_cell else OUTPUT_CELL, 4) - BUFFER
            result = -1
        else:
            trace.r[4:7] = (BUFFER + 32, BUFFER + first, BUFFER + second)
            try:
                trace.run(0x2B3EE0)
            except ValueError as error:
                raise ValueError('attribute fixture name=%d value=%d text=%r: %s' %
                                 (first, second, text, error)) from error
            input_end = output_end = -1
            result = -1 if trace.r[2] == 0 else trace.r[2] - BUFFER
        expected = [trace.load(BUFFER + index, 1) for index in range(128)]
        cases.append({'routine': routine, 'name': first, 'value': second, 'mode': mode,
                      'keep': keep, 'mutation': mutation, 'same_cell': same_cell,
                      'length': length, 'line': line, 'initial': initial, 'expected': expected,
                      'result': result, 'input_end': input_end, 'output_end': output_end,
                      'final_line': trace.load(CONTEXT + 0x18, 4),
                      'length_calls': trace.length_calls, 'compare_calls': trace.compare_calls,
                      'instruction_count': trace.instruction_count})
    return cases


def golden_header(cases):
    lines = ['/* Synthetic bounded byte/alias golden data, no original instructions. */',
             'struct TextTokenGolden { int routine,name,value,mode,keep,mutation,same_cell; u32 length,line; u8 initial[128],expected[128]; int result,input_end,output_end; u32 final_line,length_calls,compare_calls; };',
             'static const struct TextTokenGolden text_token_golden[] = {']
    for case in cases:
        numbers = ','.join(str(case[key]) for key in ('routine', 'name', 'value', 'mode', 'keep', 'mutation', 'same_cell'))
        initial = ','.join(str(value) for value in case['initial'])
        expected = ','.join(str(value) for value in case['expected'])
        tail = ','.join(str(case[key]) for key in ('result', 'input_end', 'output_end'))
        lines.append('    {%s,%du,%du,{%s},{%s},%s,%du,%du,%du},' %
                     (numbers, case['length'], case['line'], initial, expected, tail,
                      case['final_line'], case['length_calls'], case['compare_calls']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/text_tokens_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'limitation': __doc__, 'cases': cases}, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d bounded synthetic byte/alias fixtures' % len(cases))


if __name__ == '__main__':
    main()
