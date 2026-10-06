"""Generate finite alias fixtures for three reviewed rotation leaf routines.

Reuses trace_geometry's bounded host-IEEE instruction subset. The same EE
precision, special-value, FCR and timing limitations apply. Only synthetic
input/output words are exported, never original instruction arrays.
"""
import argparse
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x2A2658, 0x2A26C8), (0x2A26C8, 0x2A2780),
          (0x2A2F68, 0x2A2FDC))


class RotationTrace(Trace):
    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError("unreviewed rotation instruction: %#x" % pc)
        return struct.unpack_from("<I", self.original, pc - 0xFF000)[0]


def fixtures(original):
    cases = []
    base = 0x10000
    for routine, entry in enumerate((0x2A2658, 0x2A26C8, 0x2A2F68)):
        destinations = (0, 1, 2, 3, 4, 8, 12, 16) if routine == 0 else (0, 4, 5, 6, 7, 8, 16, 17, 18, 19, 20)
        rights = (16,) if routine == 0 else (4, 5, 16)
        for right in rights:
            for destination in destinations:
                trace = RotationTrace(original)
                for index in range(32):
                    trace.single(base + index * 4, (index % 13) - 6)
                trace.r[4] = base + destination * 4
                trace.r[5], trace.r[6] = base + 16, base + right * 4
                trace.r[29], trace.r[31] = 0x80000, RETURN
                trace.f[12] = 0.25
                initial = [trace.load(base + index * 4, 4) for index in range(32)]
                trace.run(entry)
                cases.append({"routine": routine, "output": destination,
                              "left": 4, "right": right, "initial": initial,
                              "expected": [trace.load(base + index * 4, 4)
                                           for index in range(32)],
                              "instruction_count": trace.instruction_count})
    return cases


def golden_header(cases):
    lines = ["/* Finite instruction-derived synthetic alias data; no original code. */",
             "struct RotationGolden { int routine,output,left,right; u32 initial[32],expected[32]; };",
             "static const struct RotationGolden rotation_golden[] = {"]
    for case in cases:
        initial = ",".join("0x%08Xu" % value for value in case["initial"])
        expected = ",".join("0x%08Xu" % value for value in case["expected"])
        lines.append("    {%d,%d,%d,%d,{%s},{%s}}," %
                     (case["routine"], case["output"], case["left"],
                      case["right"], initial, expected))
    return "\n".join(lines + ["};", ""])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "orig/SLUS_216.68")
    parser.add_argument("--output", type=Path, default=ROOT / "build/rotation_trace.json")
    parser.add_argument("--golden-header", type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({"limitation": __doc__, "cases": cases}, indent=2) + "\n", encoding="utf-8")
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), encoding="utf-8")
    print("%d finite original-instruction alias fixtures" % len(cases))


if __name__ == "__main__":
    main()
