"""Trace finite engine angle fixtures through the user's validated ELF.

Reuses trace_geometry's strict host-IEEE subset. RSQRT.S uses a finite native
model with a rounded square root followed by rounded division; this does not
establish the hardware's precision, exceptional results, FCR effects or timing.
Only synthetic scalar inputs/results are exported, never original instructions.
"""
import argparse
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, rounded, scalar, word

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x29C230, 0x29C29C), (0x29C2A0, 0x29C2FC), (0x29C300, 0x29C41C))
ENTRIES = tuple(start for start, end in RANGES)


class AngleTrace(Trace):
    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError("unreviewed angle instruction: %#x" % pc)
        return struct.unpack_from("<I", self.original, pc - 0xFF000)[0]

    def execute(self, instruction, pc):
        op, rs, rt = instruction >> 26, (instruction >> 21) & 31, (instruction >> 16) & 31
        fs, fd, fn = (instruction >> 11) & 31, (instruction >> 6) & 31, instruction & 63
        if op != 0x11 or rs != 16 or fn not in (0x16, 0x34):
            return super().execute(instruction, pc)
        self.instruction_count += 1
        if self.instruction_count > 3000:
            raise ValueError("instruction trace exceeded its bound")
        if fn == 0x34:
            self.condition = self.f[fs] < self.f[rt]
        else:
            denominator = rounded(math.sqrt(self.f[rt]))
            self.f[fd] = rounded(self.f[fs] / denominator)
        return None, False


def fixtures(original):
    cases = []
    for routine in range(3):
        if routine == 0:
            values = [index / 128 for index in range(-127, 128) if index != 0]
            values += [-1.0, 1.0]
        elif routine == 1:
            values = [index / 128 for index in range(-127, 128)] + [-0.0]
        else:
            values = [index / 64 for index in range(-512, 513)]
            values += [-0.0, -1e-8, 1e-8, -16384.0, 16384.0]
            threshold = math.sqrt(scalar(0x3E2FB0CD))
            values += [scalar(word(threshold) + delta) * sign
                       for delta in (-2, -1, 0, 1, 2) for sign in (-1, 1)]
        for value in values:
            trace = AngleTrace(original)
            trace.f[12] = rounded(value)
            trace.f[20] = 97.0
            trace.r[29], trace.r[31] = 0x80000, RETURN
            trace.run(ENTRIES[routine])
            assert trace.r[29] == 0x80000 and trace.f[20] == 97.0
            cases.append({"routine": routine, "input": word(value), "expected": word(trace.f[0]),
                          "instruction_count": trace.instruction_count})
    return cases


def golden_header(cases):
    lines = ["/* Finite host-model original-instruction fixtures; no original code. */",
             "struct AngleGolden { unsigned routine; u32 input, expected; };",
             "static const struct AngleGolden angle_golden[] = {"]
    for case in cases:
        lines.append("    {%d,0x%08Xu,0x%08Xu}," % (case['routine'], case['input'], case['expected']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT/'build/engine_angles_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'limitation': __doc__, 'cases': cases}, indent=2)+'\n', encoding='utf-8')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), encoding='utf-8')
    print('%d finite angle cases; %d..%d instructions' % (len(cases),
          min(case['instruction_count'] for case in cases), max(case['instruction_count'] for case in cases)))


if __name__ == '__main__':
    main()
