#!/usr/bin/env python3
"""Find robust exact or relocation-normalized PS2 runtime archive functions.

Only bits covered by an actual object relocation are masked. A candidate must
have a unique aligned occurrence, at least 256 fixed bits when relocations are
present (192 for exact matches), and an 8-byte exact
anchor. This identifies candidates; matching source progress needs a separate
source compile, link, and byte verification.
"""

import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_TOOLS = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee'
DEFAULT_ARCHIVES = [DEFAULT_TOOLS / 'ee/lib/libc.a', DEFAULT_TOOLS / 'ee/lib/libm.a',
                    DEFAULT_TOOLS / 'lib/gcc-lib/ee/3.2.3/libgcc.a']
RELOCATION_MASKS = {1: 0xFFFF0000, 2: 0, 3: 0, 4: 0xFC000000, 5: 0xFFFF0000,
                    6: 0xFFFF0000, 7: 0xFFFF0000, 9: 0xFFFF0000, 10: 0xFFFF0000,
                    12: 0}
RELOCATION_NAMES = {1: 'R_MIPS_16', 2: 'R_MIPS_32', 3: 'R_MIPS_REL32',
                    4: 'R_MIPS_26', 5: 'R_MIPS_HI16', 6: 'R_MIPS_LO16',
                    7: 'R_MIPS_GPREL16', 9: 'R_MIPS_GOT16', 10: 'R_MIPS_PC16',
                    12: 'R_MIPS_GPREL32'}


def elf(blob):
    if blob[:6] != b'\x7fELF\x01\x01':
        return None
    offset = struct.unpack_from('<I', blob, 32)[0]
    entry_size, count, names_index = struct.unpack_from('<HHH', blob, 46)
    if entry_size < 40 or offset + count * entry_size > len(blob):
        raise ValueError('Invalid ELF section table')
    headers = [struct.unpack_from('<10I', blob, offset + i * entry_size) for i in range(count)]
    h = headers[names_index]
    names = blob[h[4]:h[4] + h[5]]
    sections = []
    for index, h in enumerate(headers):
        name = names[h[0]:].split(b'\0', 1)[0].decode('ascii')
        sections.append(dict(index=index, name=name, type=h[1], flags=h[2], address=h[3],
                             offset=h[4], size=h[5], link=h[6], info=h[7], entsize=h[9],
                             data=blob[h[4]:h[4]+h[5]] if h[1] != 8 else b''))
    symbols = {}
    for section in sections:
        if section['type'] != 2:
            continue
        string_bytes = sections[section['link']]['data']
        entries = []
        for pos in range(0, section['size'], section['entsize'] or 16):
            no, value, size, info, other, si = struct.unpack_from('<IIIBBH', section['data'], pos)
            name = string_bytes[no:].split(b'\0', 1)[0].decode('ascii')
            if not name and 0 < si < len(sections) and info & 15 == 3:
                name = sections[si]['name']
            entries.append(dict(name=name, value=value, size=size, type=info & 15,
                                binding=info >> 4, section=si))
        symbols[section['index']] = entries
    relocations = {}
    for section in sections:
        if section['type'] not in (4, 9):
            continue
        if section['type'] == 4:
            raise ValueError('SHT_RELA is not supported by this scanner')
        entries = relocations.setdefault(section['info'], [])
        for pos in range(0, section['size'], section['entsize'] or 8):
            offset, info = struct.unpack_from('<II', section['data'], pos)
            entries.append(dict(offset=offset, type=info & 255,
                                symbol=symbols[section['link']][info >> 8]))
    return dict(sections=sections, symbols=symbols, relocations=relocations)


def archive_members(blob):
    if blob[:8] != b'!<arch>\n':
        raise ValueError('Not an ar archive')
    cursor, long_names = 8, b''
    while cursor + 60 <= len(blob):
        header = blob[cursor:cursor+60]
        name = header[:16].decode('ascii').strip()
        size = int(header[48:58])
        data = blob[cursor+60:cursor+60+size]
        if name == '//':
            long_names = data
        elif name.startswith('/') and name[1:].isdigit():
            position = int(name[1:])
            yield long_names[position:].split(b'/\n', 1)[0].decode('ascii'), data
        elif name != '/':
            yield name.rstrip('/'), data
        cursor += 60 + size + size % 2


def signed16(word):
    immediate = word & 0xFFFF
    return immediate if immediate < 0x8000 else immediate - 0x10000


def longest_anchor(masks):
    best_start = best_size = current_start = current_size = 0
    for i, mask in enumerate(masks):
        if mask == 0xFFFFFFFF:
            if not current_size:
                current_start = i
            current_size += 1
            if current_size > best_size:
                best_start, best_size = current_start, current_size
        else:
            current_size = 0
    return best_start * 4, best_size * 4


def infer_bindings(relocations, words, linked, function, address, gp):
    pending = {}
    bindings = {}
    unresolved = []
    details = []
    for rel in sorted(relocations, key=lambda x: x['offset']):
        pos = (rel['offset'] - function['value']) // 4
        original, actual = words[pos], linked[pos]
        symbol = rel['symbol']
        key = (symbol['name'], symbol['section'], symbol['value'])
        value = None
        if rel['type'] == 5:
            pending.setdefault(key, []).append((original, actual, rel['offset']))
        elif rel['type'] == 6 and key in pending:
            for old_hi, new_hi, hi_offset in pending.pop(key):
                old_address = ((old_hi & 0xFFFF) << 16) + signed16(original)
                new_address = ((new_hi & 0xFFFF) << 16) + signed16(actual)
                candidate = (new_address - old_address) & 0xFFFFFFFF
                bindings.setdefault(key, set()).add(candidate)
                details.append(dict(kind='HI16_LO16', symbol=symbol['name'],
                                    hi_offset=hex(hi_offset-function['value']),
                                    lo_offset=hex(rel['offset']-function['value']),
                                    address=hex(candidate)))
        elif rel['type'] == 4:
            target = ((address + pos * 4 + 4) & 0xF0000000) | ((actual & 0x3FFFFFF) << 2)
            value = (target - ((original & 0x3FFFFFF) << 2)) & 0xFFFFFFFF
        elif rel['type'] == 2:
            value = (actual - original) & 0xFFFFFFFF
        elif rel['type'] == 7 and gp is not None:
            value = (gp + signed16(actual) - signed16(original)) & 0xFFFFFFFF
        elif rel['type'] == 10:
            target = address + pos * 4 + 4 + signed16(actual) * 4
            addend_target = rel['offset'] + 4 + signed16(original) * 4
            value = (target - addend_target) & 0xFFFFFFFF
        elif rel['type'] not in (5, 6):
            unresolved.append(dict(type=RELOCATION_NAMES.get(rel['type'], str(rel['type'])),
                                   offset=hex(rel['offset']-function['value']), symbol=symbol['name']))
        if value is not None:
            bindings.setdefault(key, set()).add(value)
            details.append(dict(kind=RELOCATION_NAMES[rel['type']], symbol=symbol['name'],
                                offset=hex(rel['offset']-function['value']), address=hex(value)))
    consistent = all(len(values) == 1 for values in bindings.values())
    external = {}
    section_bases = {}
    for (name, section, symbol_value), values in bindings.items():
        if len(values) == 1:
            if section == 0:
                external[name] = hex(next(iter(values)))
            elif name.startswith('.'):
                section_bases[name] = hex((next(iter(values))-symbol_value) & 0xFFFFFFFF)
    return dict(consistent=consistent, external_symbols=external,
                section_bases=section_bases, relocation_details=details,
                unresolved_relocations=unresolved, unpaired_hi16=sum(map(len, pending.values())))


def scan(game_path, archives, minimum_size=24):
    game_bytes = game_path.read_bytes()
    game = elf(game_bytes)
    game_text = next(s for s in game['sections'] if s['name'] == '.text')
    target = game_text['data']
    gp_section = next((s for s in game['sections'] if s['name'] == '.reginfo'), None)
    gp = struct.unpack_from('<I', gp_section['data'], 20)[0] if gp_section and len(gp_section['data']) >= 24 else None
    if not gp:
        gp = None
    matches, ambiguous, unsupported = [], [], []
    tested = 0
    for archive in archives:
        archive_data = archive.read_bytes()
        archive_sha256 = hashlib.sha256(archive_data).hexdigest()
        for member, blob in archive_members(archive_data):
            obj = elf(blob)
            if not obj:
                continue
            for entries in obj['symbols'].values():
                for function in entries:
                    if function['type'] != 2 or not minimum_size <= function['size'] <= 65536:
                        continue
                    si = function['section']
                    if not 0 < si < len(obj['sections']) or obj['sections'][si]['name'] != '.text':
                        continue
                    data = obj['sections'][si]['data'][function['value']:function['value']+function['size']]
                    if len(data) != function['size'] or len(data) % 4:
                        continue
                    words = struct.unpack('<'+'I'*(len(data)//4), data)
                    masks = [0xFFFFFFFF] * len(words)
                    relocations = [r for r in obj['relocations'].get(si, [])
                                   if function['value'] <= r['offset'] < function['value']+function['size']]
                    if any(r['type'] not in RELOCATION_MASKS or r['offset'] % 4 for r in relocations):
                        unsupported.append(dict(member=member, function=function['name']))
                        continue
                    for rel in relocations:
                        masks[(rel['offset']-function['value'])//4] &= RELOCATION_MASKS[rel['type']]
                    anchor_offset, anchor_size = longest_anchor(masks)
                    fixed_bits = sum(mask.bit_count() for mask in masks)
                    if anchor_size < 8 or fixed_bits < (256 if relocations else 192):
                        continue
                    tested += 1
                    anchor = data[anchor_offset:anchor_offset+anchor_size]
                    found = target.find(anchor)
                    candidates = []
                    while found != -1:
                        start = found - anchor_offset
                        if start >= 0 and start % 4 == 0 and start + len(data) <= len(target):
                            linked = struct.unpack_from('<'+'I'*len(words), target, start)
                            if all((a & m) == (b & m) for a, b, m in zip(words, linked, masks)):
                                bindings = infer_bindings(relocations, words, linked, function,
                                                          game_text['address']+start, gp)
                                if bindings['consistent']:
                                    candidates.append((start, bindings))
                        found = target.find(anchor, found+1)
                    result = dict(archive=archive.relative_to(ROOT).as_posix() if archive.is_relative_to(ROOT) else str(archive),
                                  archive_sha256=archive_sha256,
                                  member=member, name=function['name'], size=function['size'],
                                  fixed_bits=fixed_bits, anchor_bytes=anchor_size,
                                  relocation_count=len(relocations),
                                  match_kind='relocation_normalized' if relocations else 'exact')
                    if len(candidates) == 1:
                        start, bindings = candidates[0]
                        result.update(address=hex(game_text['address']+start),
                                      file_offset=hex(game_text['offset']+start),
                                      original_sha256=hashlib.sha256(target[start:start+len(data)]).hexdigest(),
                                      bindings=bindings)
                        matches.append(result)
                    elif candidates:
                        result['addresses'] = [hex(game_text['address']+p) for p, _ in candidates]
                        ambiguous.append(result)
    return dict(schema_version=1, binary=game_path.name, target_sha256=hashlib.sha256(game_bytes).hexdigest(),
                gp=hex(gp) if gp is not None else None, tested_functions=tested,
                unique_matches=matches, ambiguous_matches=ambiguous,
                unsupported_relocations=unsupported,
                progress_policy='Archive candidates only; compile and link upstream source before recording matched source progress.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--archive', action='append', type=Path)
    parser.add_argument('--output', type=Path, default=ROOT/'build/reuse/runtime_scan.json')
    parser.add_argument('--minimum-size', type=int, default=24)
    args = parser.parse_args()
    result = scan(args.elf, args.archive or DEFAULT_ARCHIVES, args.minimum_size)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(f"Scanned {result['tested_functions']} functions: {len(result['unique_matches'])} unique archive matches; "
          f"{len(result['ambiguous_matches'])} ambiguous patterns. Wrote {args.output}")
    for match in result['unique_matches']:
        print(f"{match['address']} {match['size']:5d} {match['name']} ({match['match_kind']})")


if __name__ == '__main__':
    main()
