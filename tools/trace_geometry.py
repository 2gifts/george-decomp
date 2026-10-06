"""Trace finite geometry fixtures through the verified retail instruction bodies.

This is a bounded host-IEEE instruction subset, not a complete R5900 emulator.
It excludes FCR flags, special-value arithmetic, timing and EE precision quirks.
No original instruction arrays are included: instructions are read from the
locally supplied, hash-validated ELF. Output contains only synthetic test data.
"""

import argparse
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf

ROOT = Path(__file__).resolve().parents[1]
ENTRY = 0x002A3640
RETURN = 0xFFFFFFFC
CODE_RANGES = ((0x002A3538, 0x002A35C0), (ENTRY, 0x002A3C9C))
FRAME_OFFSETS = (None, 0, 4, 12, 16, 24, 28, 32, 40, 48, 56, 64, 80)
FRAME = (1, 0, 0, 90, 0, 1, 0, 91, 0, 0, 1, 92, 3, -2, 5, 93)


def word(value):
    return struct.unpack("<I", struct.pack("<f", value))[0]


def scalar(value):
    return struct.unpack("<f", struct.pack("<I", value))[0]


def rounded(value):
    result = scalar(word(value))
    if not math.isfinite(result):
        raise ValueError("fixture left the supported finite arithmetic subset")
    return result


class Trace:
    def __init__(self, original):
        self.original = original
        self.r = [0] * 32
        self.f = [0.0] * 32
        self.memory = {}
        self.condition = False
        self.instruction_count = 0

    def load(self, address, size):
        # Reject unknown memory instead of silently assuming an initial value.
        return int.from_bytes(bytes(self.memory[(address + i) & 0xFFFFFFFF]
                                    for i in range(size)), "little")

    def save(self, address, value, size):
        data = (value & ((1 << (size * 8)) - 1)).to_bytes(size, "little")
        for i, byte in enumerate(data):
            self.memory[(address + i) & 0xFFFFFFFF] = byte

    def single(self, address, value):
        if not math.isfinite(value):
            raise ValueError("nonfinite input is outside this trace's scope")
        self.save(address, word(value), 4)

    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in CODE_RANGES):
            raise ValueError("unreviewed instruction address: %#x" % pc)
        # The validated build's .text has file offset 0x1000, VA 0x100000.
        return struct.unpack_from("<I", self.original, pc - 0xFF000)[0]

    def execute(self, instruction, pc):
        self.instruction_count += 1
        if self.instruction_count > 3000:
            raise ValueError("instruction trace exceeded its bound")
        op = instruction >> 26
        rs = (instruction >> 21) & 31
        rt = (instruction >> 16) & 31
        rd = (instruction >> 11) & 31
        imm = instruction & 0xFFFF
        imm = imm - 0x10000 if imm & 0x8000 else imm
        fn = instruction & 63
        target, annul = None, False
        if instruction == 0:
            pass
        elif op == 0:
            if fn in (0x21, 0x2D):
                self.r[rd] = (self.r[rs] + self.r[rt]) & 0xFFFFFFFF
            elif fn == 8:
                target = self.r[rs]
            else:
                raise ValueError("unsupported SPECIAL at %#x" % pc)
        elif op == 9:
            self.r[rt] = (self.r[rs] + imm) & 0xFFFFFFFF
        elif op == 0xF:
            self.r[rt] = (instruction & 0xFFFF) << 16
        elif op == 0xD:
            self.r[rt] = self.r[rs] | (instruction & 0xFFFF)
        elif op == 3:
            self.r[31] = pc + 8
            target = ((pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)
        elif op == 4:
            if self.r[rs] == self.r[rt]:
                target = (pc + 4 + (imm << 2)) & 0xFFFFFFFF
        elif op in (0x23, 0x37, 0x1E):
            size = {0x23: 4, 0x37: 8, 0x1E: 16}[op]
            self.r[rt] = self.load((self.r[rs] + imm) & 0xFFFFFFFF, size)
        elif op in (0x2B, 0x3F, 0x1F):
            size = {0x2B: 4, 0x3F: 8, 0x1F: 16}[op]
            self.save((self.r[rs] + imm) & 0xFFFFFFFF, self.r[rt], size)
        elif op == 0x31:
            self.f[rt] = scalar(self.load((self.r[rs] + imm) & 0xFFFFFFFF, 4))
        elif op == 0x39:
            self.single((self.r[rs] + imm) & 0xFFFFFFFF, self.f[rt])
        elif op == 0x11:
            ft, fs, fd = rt, rd, (instruction >> 6) & 31
            if rs == 0:
                self.r[rt] = word(self.f[fs])
            elif rs == 4:
                self.f[fs] = scalar(self.r[rt] & 0xFFFFFFFF)
            elif rs == 8:
                wanted, likely = bool(rt & 1), bool(rt & 2)
                if self.condition == wanted:
                    target = (pc + 4 + (imm << 2)) & 0xFFFFFFFF
                elif likely:
                    annul = True
            elif rs == 16:
                if fn == 0:
                    self.f[fd] = rounded(self.f[fs] + self.f[ft])
                elif fn == 1:
                    self.f[fd] = rounded(self.f[fs] - self.f[ft])
                elif fn == 2:
                    self.f[fd] = rounded(self.f[fs] * self.f[ft])
                elif fn == 3:
                    self.f[fd] = rounded(self.f[fs] / self.f[ft])
                elif fn == 4:
                    # Authentic EE SQRT.S takes ft, unlike ordinary MIPS fs.
                    self.f[fd] = rounded(math.sqrt(self.f[ft]))
                elif fn == 6:
                    self.f[fd] = self.f[fs]
                elif fn == 7:
                    self.f[fd] = -self.f[fs]
                elif fn == 0x32:
                    self.condition = self.f[fs] == self.f[ft]
                else:
                    raise ValueError("unsupported COP1.S at %#x" % pc)
            else:
                raise ValueError("unsupported COP1 at %#x" % pc)
        else:
            raise ValueError("unsupported opcode at %#x" % pc)
        self.r[0] = 0
        return target, annul

    def run(self, entry=ENTRY):
        pc = entry
        while pc != RETURN:
            target, annul = self.execute(self.fetch(pc), pc)
            if target is not None:
                nested = self.execute(self.fetch(pc + 4), pc + 4)
                if nested != (None, False):
                    raise ValueError("control transfer in delay slot")
                pc = target
            else:
                # Untaken ordinary branches execute their delay on the next
                # iteration; untaken likely branches skip it altogether.
                pc += 8 if annul else 4


def fixtures(original):
    cases = []
    for offset in FRAME_OFFSETS:
        trace = Trace(original)
        output = 0x10000
        frame = 0x20000 if offset is None else output + offset
        for i in range(56):
            trace.single(output + i * 4, (i % 13) - 6)
        for i, value in enumerate(FRAME):
            trace.single(frame + i * 4, value)
        initial = [trace.load(output + i * 4, 4) for i in range(56)]
        trace.r[4], trace.r[5] = output, frame
        trace.r[29], trace.r[31] = 0x80000, RETURN
        trace.f[12], trace.f[13], trace.f[14] = 4, 2, 1
        trace.run()
        cases.append({"frame_offset": -1 if offset is None else offset,
                      "initial": initial,
                      "expected": [trace.load(output + i * 4, 4)
                                   for i in range(35)],
                      "instruction_count": trace.instruction_count})
    return cases


def golden_header(cases):
    lines = ["/* Finite instruction-derived golden outputs; no original code or assets. */",
             "struct GeometryGolden { int frame_offset; u32 initial[56],expected[35]; };",
             "static const struct GeometryGolden geometry_golden[] = {"]
    for case in cases:
        initial = ",".join("0x%08Xu" % value for value in case["initial"])
        expected = ",".join("0x%08Xu" % value for value in case["expected"])
        lines.append("    {%d, {%s}, {%s}}," %
                     (case["frame_offset"], initial, expected))
    return "\n".join(lines + ["};", ""])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "orig/SLUS_216.68")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "build/geometry_trace.json")
    parser.add_argument("--golden-header", type=Path,
                        help="explicitly regenerate a synthetic native golden header")
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    report = {"limitation": "Finite host-IEEE operation subset only; excludes EE "
              "special/FCR/cycle behavior. Reads original instructions at runtime; "
              "no original instruction arrays copied.", "cases": cases}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), encoding="utf-8")
    print("%d finite cases; instruction counts %s" %
          (len(cases), [case["instruction_count"] for case in cases]))


if __name__ == "__main__":
    main()
