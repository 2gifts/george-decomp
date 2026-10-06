"""Generate bounded synthetic parser fixtures from the validated retail ELF.

The full parser, token copier and lowercase helper execute through the reviewed
integer/byte decoder. Length, bounded compare and case-insensitive compare use
controlled byte substitutes, with explicit callback mutations. Common ASCII
delimiters/classifications are synthetic inputs, not copied game data. Invalid
buffers, unbounded malformed inputs and hardware effects are outside this model.
"""
import argparse
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import RETURN
from trace_text_tokens import TextTrace, signed32, TABLES, QUOTES

ROOT = Path(__file__).resolve().parents[1]
BUFFER, CONTEXT, TAGS, STRINGS = 0x10000, 0x20000, 0x50000, 0x60000
CTYPE = 0x456118
RANGES = ((0x2B3190, 0x2B3460), (0x2B3460, 0x2B3EDC), (0x2B45A0, 0x2B45F0))
STRINGS_DATA = (b'tag', b'other', b' ', b'\t', b'\r', b'\n', b'</', b'/>', b'>', b'/tag')
WORDS = (0, 0x10, 0x14, 0x18, 0x101C, 0x1020, 0x1024, 0x1028, 0x102C)


class ParserTrace(TextTrace):
    def __init__(self, original, mutation=0):
        super().__init__(original)
        self.parser_mutation = mutation
        self.tag_calls = 0
        self.total_instructions = 0

    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed parser instruction: %#x' % pc)
        return struct.unpack_from('<I', self.original, pc - 0xFF000)[0]

    def execute(self, instruction, pc):
        # The inherited decoder's 3,000-step limit is for its smaller standalone
        # helpers. Count every parser/helper/delay instruction under one separate
        # finite budget, without editing the already published decoder.
        self.total_instructions += 1
        if self.total_instructions > 120000:
            raise ValueError('parser trace exceeded its finite instruction bound')
        self.instruction_count = 0
        op, rs, rt, rd = instruction >> 26, (instruction >> 21) & 31, (instruction >> 16) & 31, (instruction >> 11) & 31
        imm = instruction & 0xFFFF
        signed_imm = imm - 0x10000 if imm & 0x8000 else imm
        fn = instruction & 63
        target = None
        if op == 7:
            if rt != 0:
                raise ValueError('invalid BGTZ encoding')
            if signed32(self.r[rs]) > 0:
                target = (pc + 4 + (signed_imm << 2)) & 0xFFFFFFFF
        elif op == 0x0C:
            self.r[rt] = self.r[rs] & imm
        elif op == 0 and fn == 0x23:
            self.r[rd] = (self.r[rs] - self.r[rt]) & 0xFFFFFFFF
        else:
            return super().execute(instruction, pc)
        self.r[0] = 0
        return target, False

    def text(self, pointer):
        result = bytearray()
        while self.load(pointer, 1):
            result.append(self.load(pointer, 1))
            pointer += 1
            if len(result) > 4095:
                raise ValueError('controlled comparison lacks a bounded terminator')
        return bytes(result)

    def library_call(self, target):
        if target != 0x3983E8:
            super().library_call(target)
            if target == 0x295050 and self.length_calls == 1 and self.parser_mutation == 1:
                self.save(CONTEXT + 0x18, 0xFFFFFFFF, 4)
            return
        left, right = self.text(self.r[4]), self.text(self.r[5])
        lower = lambda byte: byte + 32 if 65 <= byte <= 90 else byte
        result = 0
        for first, second in zip(left + b'\0', right + b'\0'):
            if lower(first) != lower(second):
                result = lower(first) - lower(second)
                break
        self.tag_calls += 1
        if self.tag_calls == 1:
            if self.parser_mutation == 2:
                self.save(TAGS, 0x12345678, 4)
                self.save(CONTEXT + 0x14, 99, 4)
            elif self.parser_mutation == 3:
                self.save(TAGS + 8, 0x8002DEAD, 4)
            elif self.parser_mutation == 4:
                self.save(CONTEXT + 0x101C, 4, 4)
                self.save(CONTEXT + 0x1020, 2, 4)
                self.save(CONTEXT + 0x1024, 0xFFFFFFFE, 4)
            elif self.parser_mutation == 5:
                self.save(CONTEXT + 0xC, BUFFER + 7, 4)
        self.r[2] = result & 0xFFFFFFFF

    def run(self, entry=0x2B3460):
        pc = entry
        while pc != RETURN:
            target, annul = self.execute(self.fetch(pc), pc)
            if target is not None:
                nested = self.execute(self.fetch(pc + 4), pc + 4)
                if nested != (None, False):
                    raise ValueError('control transfer in parser delay slot')
                if target in (0x295050, 0x393E48, 0x3983E8):
                    self.library_call(target)
                    pc = self.r[31]
                else:
                    pc = target
            else:
                pc += 8 if annul else 4


def setup(original, spec):
    trace = ParserTrace(original, spec['mutation'])
    trace.initialize(BUFFER, bytes([0x7E]) * 256)
    trace.initialize(CONTEXT, bytes([0x7E]) * 0x4030)
    trace.initialize(TAGS, bytes(32))
    trace.initialize(0x7E000, bytes(0x2000))
    trace.initialize(QUOTES, bytes((0, 39, 34)))
    trace.initialize(CTYPE, bytes([0] + [int(65 <= code <= 90) for code in range(256)]))
    for index, text in enumerate(STRINGS_DATA):
        trace.initialize(STRINGS + index * 32, text + bytes(32 - len(text)))
    for address, cells in zip(TABLES, ((2, 3), (4, 5), (6, 7, 8))):
        trace.initialize(address, bytes(20))
        for index, cell in enumerate(cells):
            trace.save(address + index * 4, STRINGS + cell * 32, 4)
    for index, (code, name) in enumerate(((41, 0), (42, 1), (0x8000DEAD, 0), (0x8000DEAD, 0))):
        trace.save(TAGS + index * 8, code, 4)
        trace.save(TAGS + index * 8 + 4, STRINGS + name * 32, 4)
    if spec['entries'] == 2:
        trace.save(TAGS + 4, STRINGS + 32, 4)
        trace.save(TAGS + 12, STRINGS, 4)
    elif spec['entries'] == 3:
        trace.save(TAGS, 0x8002DEAD, 4)
    elif spec['entries'] == 4:
        trace.save(TAGS, 0x8000DEAD, 4)
    elif spec['entries'] == 5:
        trace.save(TAGS + 4, STRINGS + 9 * 32, 4)
    source = BUFFER + 32 if spec['alias'] < 0 else CONTEXT + spec['alias']
    for index, byte in enumerate(spec['text'] + b'\0\0\0\0'):
        trace.save(source + index, byte, 1)
    trace.save(CONTEXT, spec['flags'], 4)
    trace.save(CONTEXT + 8, source, 4)
    trace.save(CONTEXT + 0xC, source + spec['cursor'], 4)
    trace.save(CONTEXT + 0x10, spec['length'], 4)
    trace.save(CONTEXT + 0x18, spec['line'], 4)
    trace.save(CONTEXT + 0x1024, spec['depth'], 4)
    return trace, source


def specifications():
    specs = []
    texts = (b'', b' \n\r\t', b'<tag>', b'<TAG A="two words">Text</TAG>',
             b'<tag a=1/>', b'</tag>', b'<?xml x="y"?>', b'<!thing>',
             b'<!--a\nb-->\n<tag>one</tag>', b'<!--first--><!--second--><tag/>',
             b'<tag>one\n two</tag>', b'<tag><other/>', b'/>', b'?>',
             b'<tag a="/> hidden">x</tag>', b'<tag a=\'quoted\'>x</tag>',
             b'<!--unclosed', b'<tag a=\xff>x</tag>')
    for text in texts:
        for flags in (0, 1):
            for entries in (0, 1, 2, 3, 4, 5):
                specs.append(dict(text=text, flags=flags, entries=entries, alias=-1,
                                  cursor=0, length=len(text), line=0, depth=7,
                                  mutation=0, iterations=1))
    for alias in (0x1C + 32, 0x1030 + 32, 0x2030 + 32, 0x3030 + 32):
        specs.append(dict(text=b'<TAG a="x">two</TAG>', flags=1, entries=1,
                          alias=alias, cursor=0, length=19, line=0xFFFFFFFF,
                          depth=0xFFFFFFFF, mutation=0, iterations=1))
    for mutation in range(1, 6):
        specs.append(dict(text=b'<tag a="x">two</tag>', flags=0, entries=1,
                          alias=-1, cursor=0, length=19, line=4, depth=7,
                          mutation=mutation, iterations=1))
    specs.append(dict(text=b'<tag/>', flags=0, entries=2, alias=-1, cursor=0,
                      length=6, line=4, depth=7, mutation=3, iterations=1))
    specs.append(dict(text=b'</tag>', flags=0, entries=5, alias=-1, cursor=0,
                      length=6, line=4, depth=7, mutation=2, iterations=1))
    for length in (0, 1, 2, 3, 4):
        specs.append(dict(text=b' \n\r\t', flags=0, entries=1, alias=-1, cursor=0,
                          length=length, line=0xFFFFFFFF, depth=7, mutation=0, iterations=1))
    for cursor, length in ((4, 4), (5, 4), (0, 0xFFFFFFFF)):
        specs.append(dict(text=b'<tag>', flags=0, entries=1, alias=-1,
                          cursor=cursor, length=length, line=99, depth=7,
                          mutation=0, iterations=1))
    for iterations in (1, 2, 3, 4, 5):
        specs.append(dict(text=b'<tag>one</tag><other/>', flags=0, entries=1,
                          alias=-1, cursor=0, length=21, line=0, depth=0,
                          mutation=0, iterations=iterations))
    return specs


def fixtures(original):
    cases = []
    for spec in specifications():
        trace, source = setup(original, spec)
        initial = [trace.load(CONTEXT + i, 1) for i in range(0x4030)]
        for iteration in range(spec['iterations']):
            trace.r[4:6] = (CONTEXT, 0 if spec['entries'] == 0 else TAGS)
            trace.r[29], trace.r[31] = 0x80000, RETURN
            trace.run()
        cursor = trace.load(CONTEXT + 0xC, 4)
        if BUFFER <= cursor < BUFFER + 256:
            cursor_kind, cursor_offset = 0, cursor - BUFFER
        elif CONTEXT <= cursor < CONTEXT + 0x4030:
            cursor_kind, cursor_offset = 1, cursor - CONTEXT
        else:
            raise ValueError('final cursor outside fixture storage')
        changes = [(i, trace.load(CONTEXT + i, 1)) for i in range(0x4030)
                   if not 8 <= i < 16 and trace.load(CONTEXT + i, 1) != initial[i]]
        case = dict(spec)
        case['text'] = list(spec['text'])
        case.update(changes=changes, cursor_kind=cursor_kind, cursor_offset=cursor_offset,
                    buffer=[trace.load(BUFFER + i, 1) for i in range(256)],
                    length_calls=trace.length_calls, compare_calls=trace.compare_calls,
                    tag_calls=trace.tag_calls, instruction_count=trace.total_instructions)
        cases.append(case)
    return cases


def golden_header(cases):
    lines = ['/* Synthetic full-parser outputs; no original code or assets. */',
             'struct ParserPatch { u16 offset; u8 byte; };',
             'struct ParserGolden { const u8 *text; unsigned text_size; u32 flags, entries;',
             '    int alias; u32 cursor, length, line, depth, mutation, iterations;',
             '    const struct ParserPatch *patches; unsigned patch_count;',
             '    int cursor_kind; unsigned cursor_offset, length_calls, compare_calls, tag_calls;',
             '    const u8 *expected_buffer; };']
    for index, case in enumerate(cases):
        patches = ','.join('{%d,%d}' % tuple(patch) for patch in case['changes']) or '{0,0}'
        text = ','.join(map(str, case['text'])) or '0'
        buffer = ','.join(map(str, case['buffer']))
        lines += ['static const u8 parser_text_%d[] = {%s};' % (index, text),
                  'static const struct ParserPatch parser_patches_%d[] = {%s};' % (index, patches),
                  'static const u8 parser_buffer_%d[] = {%s};' % (index, buffer)]
    lines.append('static const struct ParserGolden parser_golden[] = {')
    for index, case in enumerate(cases):
        values = ','.join('0x%Xu' % case[key] for key in ('cursor', 'length', 'line', 'depth', 'mutation', 'iterations'))
        lines.append('    {parser_text_%d,%d,%du,%du,%d,%s,parser_patches_%d,%d,%d,%d,%d,%d,%d,parser_buffer_%d},' %
                     (index, len(case['text']), case['flags'], case['entries'], case['alias'], values,
                      index, len(case['changes']), case['cursor_kind'], case['cursor_offset'],
                      case['length_calls'], case['compare_calls'], case['tag_calls'], index))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/text_parser_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'limitation': __doc__, 'cases': cases}, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(golden_header(cases))
    print('%d synthetic parser cases; %d total instructions; maximum %d' %
          (len(cases), sum(case['instruction_count'] for case in cases),
           max(case['instruction_count'] for case in cases)))


if __name__ == '__main__':
    main()
