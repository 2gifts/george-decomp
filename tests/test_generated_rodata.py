"""Generated readonly linking guards use only synthetic ELF/code/data."""
import hashlib
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

# Standard tracked tests import the normal tools; the ignored staged runner
# preloads its candidate modules without mutating those paths.
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'tests'))
import test_link as fixtures
import link_match
import verify
import build

CODE_ADDRESS = 0x110000
DATA_ADDRESS = 0x348008


def generated_mapping(data=None, address=DATA_ADDRESS, original_size=16):
    if data is None:
        data = struct.pack('<4I', CODE_ADDRESS + 8, CODE_ADDRESS + 4, 0x12345678, 0x76543210)
    return {**fixtures.data_mapping(data, address), 'original_function_size': original_size}


def generated_object(data=None, relocations=None, symbols=None, extra_sections=(), code_relocations=None):
    if data is None:
        data = struct.pack('<4I', 8, 0xFFFFFFFC, 0x12345678, 0x76543210)
    if relocations is None:
        relocations = [(0, 2, 3), (4, 2, 4)]
    table = b''.join(struct.pack('<II', offset, index << 8 | kind) for offset, kind, index in relocations)
    image = fixtures.synthetic_object(target_size=16,
        code_words=(0x3C020000, 0x24420000, 0x03E00008, 0),
        relocations=[(0, 5, 2), (4, 6, 2)] if code_relocations is None else code_relocations,
        extra_symbols=symbols if symbols is not None else [('', 0, 0, 3, 6), ('', 0, 0, 3, 1), ('local_case', 8, 0, 0, 1)],
        extra_sections=[('.rodata', data, 2), ('.rel.rodata', table, 0), *extra_sections])
    for field, value in ((1, 9), (6, 3), (7, 6), (9, 8)):
        image = fixtures.section_field(image, 7, field, value)
    return image


class GeneratedPlanTests(unittest.TestCase):
    def inspect(self, image=None, mapping=None, **kwargs):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / 'fixture.o'
            path.write_bytes(generated_object() if image is None else image)
            return link_match.inspect_function(path, 'fixture', CODE_ADDRESS, {},
                mapped_codegen_readonly={'.rodata': generated_mapping() if mapping is None else mapping}, **kwargs)

    def test_actual_section_and_named_negative_addend_labels_are_unchanged(self):
        plan = self.inspect()
        mapping = plan['mapped_codegen_readonly'][0]
        self.assertEqual(plan['symbol_patches'], {})
        self.assertEqual(plan['mapped_sections'], [])
        self.assertEqual(mapping['literal_byte_count'], 8)
        self.assertEqual([r['relative_offset'] for r in mapping['input_relocations']], [8, 4])
        self.assertEqual([r['addend'] for r in mapping['input_relocations']], [8, -4])
        self.assertNotEqual(mapping['data'], mapping['expected_bytes'])
        self.assertEqual(mapping['input_sha256'], hashlib.sha256(mapping['data']).hexdigest())

    def test_existing_exact_original_mapper_stays_strict(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / 'fixture.o'; path.write_bytes(generated_object())
            with self.assertRaisesRegex(link_match.LinkError, 'bytes differ'):
                link_match.inspect_function(path, 'fixture', CODE_ADDRESS, {}, mapped_sections={'.rodata': fixtures.data_mapping(generated_mapping()['expected_bytes'])})
            raw = struct.pack('<4I', 8, 0xFFFFFFFC, 0x12345678, 0x76543210)
            with self.assertRaisesRegex(link_match.LinkError, 'Relocation-bearing'):
                link_match.inspect_function(path, 'fixture', CODE_ADDRESS, {}, mapped_sections={'.rodata': fixtures.data_mapping(raw)})

    def test_literal_and_alignment_bytes_cannot_be_changed_or_omitted(self):
        for position in (8, 15):
            data = bytearray(struct.pack('<4I', 8, 0xFFFFFFFC, 0x12345678, 0x76543210)); data[position] ^= 1
            with self.subTest(position=position), self.assertRaisesRegex(link_match.LinkError, 'unrelocated'):
                self.inspect(generated_object(data=bytes(data)))
        with self.assertRaisesRegex(link_match.LinkError, 'whole'):
            self.inspect(mapping=generated_mapping(generated_mapping()['expected_bytes'][:12]))

    def test_only_unique_aligned_bounded_r_mips_32_entries_are_allowed(self):
        for entries in ([(0, 0, 3)], [(0, 5, 3)], [(2, 2, 3)], [(16, 2, 3)], [(0, 2, 999)], [(0, 2, 3), (0, 2, 3)]):
            with self.subTest(entries=entries), self.assertRaises(link_match.LinkError):
                self.inspect(generated_object(relocations=entries))

    def test_actual_rel_table_link_geometry_and_nonempty_requirement(self):
        for field, value in ((1, 4), (6, 2), (9, 0), (9, 12), (5, 15)):
            with self.subTest(field=field), self.assertRaises(link_match.LinkError):
                self.inspect(fixtures.section_field(generated_object(), 7, field, value))
        with self.assertRaisesRegex(link_match.LinkError, 'one nonempty'):
            self.inspect(generated_object(relocations=[]))

    def test_other_code_data_external_absolute_and_global_symbols_fail(self):
        for symbol in (('external', 0, 0, 0x10, 0), ('absolute', CODE_ADDRESS + 8, 0, 0x10, 0xFFF1),
                       ('data', 8, 0, 0, 6), ('other_code', 8, 0, 0, 8), ('global_label', 8, 0, 0x10, 1),
                       ('object', 8, 0, 1, 1), ('function', 8, 0, 2, 1), ('sized_label', 8, 4, 0, 1)):
            image = generated_object(symbols=[('', 0, 0, 3, 6), ('', 0, 0, 3, 1), symbol], extra_sections=[('.text.other', bytes(16), 6)])
            with self.subTest(symbol=symbol), self.assertRaisesRegex(link_match.LinkError, 'local label'):
                self.inspect(image)

    def test_label_bounds_reject_start_end_negative_unaligned_and_padding(self):
        for addend in (0, 16, 0xFFFFFFFF, 9):
            data = struct.pack('<4I', addend, 0xFFFFFFFC, 0x12345678, 0x76543210)
            with self.subTest(addend=addend), self.assertRaisesRegex(link_match.LinkError, 'addend'):
                self.inspect(generated_object(data=data))
        image = generated_object(data=struct.pack('<4I', 12, 0xFFFFFFFC, 0x12345678, 0x76543210))
        # A source section may contain padding; it is never a valid case target.
        image = bytearray(image); symbol_table = struct.unpack_from('<I', image, 32)[0]
        symtab_offset = struct.unpack_from('<I', image, symbol_table + 3 * 40 + 16)[0]
        struct.pack_into('<I', image, symtab_offset + 16 + 8, 12)
        with self.assertRaisesRegex(link_match.LinkError, 'padding'):
            self.inspect(bytes(image))

    def test_original_case_pointers_must_be_inside_whole_original_function(self):
        for target in (CODE_ADDRESS, CODE_ADDRESS + 16, CODE_ADDRESS + 9, 0x120000):
            expected = struct.pack('<4I', target, CODE_ADDRESS + 4, 0x12345678, 0x76543210)
            with self.subTest(target=target), self.assertRaisesRegex(link_match.LinkError, 'Original generated-table pointer'):
                self.inspect(mapping=generated_mapping(expected))

    def test_schema_hash_alignment_flags_and_nobits_are_strict(self):
        proof = generated_mapping()
        for mapping in ({**proof, 'unexpected': True}, {k:v for k,v in proof.items() if k!='original_function_size'},
                        {**proof, 'expected_sha256': '0'*64}, {**proof, 'original_function_size': True},
                        {**proof, 'address': DATA_ADDRESS+1}):
            with self.subTest(mapping=mapping), self.assertRaises(link_match.LinkError): self.inspect(mapping=mapping)
        for flags in (0, 3, 6, 0x12, 0x22):
            with self.subTest(flags=flags), self.assertRaisesRegex(link_match.LinkError, 'whole plain'):
                self.inspect(fixtures.section_field(generated_object(), 6, 2, flags))
        with self.assertRaisesRegex(link_match.LinkError, 'PROGBITS'):
            self.inspect(fixtures.section_field(generated_object(), 6, 1, 8))

    def test_multiple_tables_unused_data_and_all_mapping_overlaps_fail(self):
        second = struct.pack('<II', 8, 3 << 8 | 2)
        image = generated_object(extra_sections=[('.rel.rodata.extra', second, 0)])
        for field, value in ((1,9),(6,3),(7,6),(9,8)): image=fixtures.section_field(image,8,field,value)
        with self.assertRaisesRegex(link_match.LinkError, 'one nonempty'): self.inspect(image)
        with self.assertRaisesRegex(link_match.LinkError, 'Unused'): self.inspect(generated_object(code_relocations=[]))
        with self.assertRaisesRegex(link_match.LinkError, 'overlap'): self.inspect(mapping=generated_mapping(address=CODE_ADDRESS))
        raw = struct.pack('<4I', 8, 0xFFFFFFFC, 0x12345678, 0x76543210)
        with self.assertRaises(link_match.LinkError): self.inspect(mapped_sections={'.rodata': fixtures.data_mapping(raw)})


class GeneratedRealLinkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls): fixtures.RealLinkTests.setUpClass.__func__(cls)

    def link(self, expected=None):
        body='lui $v0,%hi(.rodata.cases)\naddiu $v0,$v0,%lo(.rodata.cases)\n.Lcase:\njr $ra\nnop'
        extra='.section .rodata.cases,"a",@progbits\n.align 2\n.word .Lcase,.Lcase,0x12345678,0x76543210\n'
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);source=root/'fixture.s';obj=root/'fixture.o'
            source.write_text('.set noreorder\n.section .text.fixture,"ax",@progbits\n.align 3\n.globl fixture\n.ent fixture\nfixture:\n'+body+'\n.end fixture\n.size fixture,.-fixture\n'+extra)
            process=subprocess.run([str(self.assembler),'-EL','-march=r5900','-mabi=eabi','-G0','-no-pad-sections','-o',str(obj),str(source)],capture_output=True,text=True)
            self.assertEqual(process.returncode,0,process.stderr);before=obj.read_bytes()
            if expected is None: expected=struct.pack('<4I',CODE_ADDRESS+8,CODE_ADDRESS+8,0x12345678,0x76543210)
            result=link_match.link_function(obj,'fixture',CODE_ADDRESS,{},root/'linked',mapped_codegen_readonly={'.rodata.cases':generated_mapping(expected)})
            self.assertEqual(obj.read_bytes(),before);self.assertEqual(result['remaining_relocations'],[])
            return result

    def test_actual_ld_returns_complete_exact_code_and_table_without_input_edits(self):
        result=self.link();mapping=result['mapped_codegen_readonly'][0]
        self.assertEqual(result['code'],struct.pack('<4I',0x3C020035,0x24428008,0x03E00008,0))
        self.assertTrue(mapping['identical']);self.assertEqual(mapping['data'],mapping['expected_bytes'])
        self.assertEqual(mapping['sha256'],mapping['expected_sha256']);self.assertEqual(len(mapping['input_relocations']),2)

    def test_different_genuine_case_table_is_reported_and_blocks_code_only_match(self):
        expected=struct.pack('<4I',CODE_ADDRESS+12,CODE_ADDRESS+8,0x12345678,0x76543210)
        result=self.link(expected);mapping=result['mapped_codegen_readonly'][0]
        self.assertFalse(mapping['identical']);self.assertNotEqual(mapping['sha256'],mapping['expected_sha256'])
        comparison=verify.gate_generated_comparison(verify.compare_function(result['code'],result['code'],[]),[mapping])
        self.assertTrue(comparison['code_identical']);self.assertFalse(comparison['generated_data_identical']);self.assertFalse(comparison['identical'])


class GeneratedVerifierBuildTests(unittest.TestCase):
    def original_proof(self, code_words=(0x3C020000,0x24424000,0x03E00008,0), pointer=None):
        code=struct.pack('<'+'I'*len(code_words),*code_words);data=struct.pack('<4I',CODE_ADDRESS+8,CODE_ADDRESS+8,0x12345678,0x76543210)
        original=code+bytes(32-len(code))+data
        function={'name':'fixture','address':hex(CODE_ADDRESS),'file_offset':0,'size':len(code),'link_codegen_readonly':{'.rodata':{'address':0x4000,'file_offset':32,'size':16,'original_sha256':hashlib.sha256(data).hexdigest(),
            'code_bindings':[{'hi_offset':0,'lo_offset':4,'relative_offset':0} if pointer is None else pointer]}}}
        sections=[{'offset':32,'address':0x4000,'size':16}]
        return function,original,sections

    def test_original_hash_geometry_and_real_nonzero_pointer_are_required(self):
        function,original,sections=self.original_proof();mapping=verify.mapped_codegen_data(function,original,sections)['.rodata']
        self.assertEqual(mapping['original_function_size'],16);self.assertEqual(mapping['expected_bytes'],original[32:])
        for changed in ({'address':0x4004},{'file_offset':33},{'size':12},{'original_sha256':'0'*64},{'code_bindings':[]},{'relocation_policy':'self_r_mips_32'}):
            fn,raw,secs=self.original_proof();fn['link_codegen_readonly']['.rodata'].update(changed)
            with self.subTest(changed=changed),self.assertRaises(ValueError): verify.mapped_codegen_data(fn,raw,secs)
        for lower in (0x24404000,0x24624000,0x34424000):
            fn,raw,secs=self.original_proof((0x3C020000,lower,0x03E00008,0))
            with self.subTest(lower=lower),self.assertRaisesRegex(ValueError,'nonzero'):verify.mapped_codegen_data(fn,raw,secs)

    def test_intervening_shift_is_narrow_and_cannot_clobber_pointer_register(self):
        pointer={'hi_offset':0,'lo_offset':8,'relative_offset':0}
        fn,raw,secs=self.original_proof((0x3C020000,0x00101880,0x24424000,0x03E00008,0),pointer)
        self.assertTrue(verify.mapped_codegen_data(fn,raw,secs))
        for middle in (0x00001000,0x34420000,0x10000000):
            fn,raw,secs=self.original_proof((0x3C020000,middle,0x24424000,0x03E00008,0),pointer)
            with self.subTest(middle=middle),self.assertRaisesRegex(ValueError,'intervening'):verify.mapped_codegen_data(fn,raw,secs)

    def test_code_and_data_must_both_match_and_optional_old_behavior_is_preserved(self):
        exact=verify.compare_function(b'code',b'code',[]);different=verify.compare_function(b'code',b'coda',[])
        self.assertEqual(verify.gate_generated_comparison(exact,[]),exact)
        self.assertTrue(verify.gate_generated_comparison(exact,[{'identical':True}])['identical'])
        self.assertFalse(verify.gate_generated_comparison(different,[{'identical':True}])['identical'])
        self.assertFalse(verify.gate_generated_comparison(exact,[{'identical':True},{'identical':False}])['identical'])
        with self.assertRaises(ValueError):verify.gate_generated_comparison(exact,[{'identical':'true'}])

    def test_hybrid_rechecks_complete_actual_data_even_with_forged_match_flag(self):
        _,original,sections=self.original_proof();data=original[32:]
        mapping={'name':'.rodata','output_section':'.george_generated_rodata_0','address':0x4000,'file_offset':32,'size':16,
                 'original_sha256':hashlib.sha256(data).hexdigest(),'sha256':hashlib.sha256(data).hexdigest(),'identical':True}
        function={'mapped_codegen_readonly':[mapping],'generated_data_identical':True}
        class Section(dict):
            def data(self):return self['actual']
        class Linked:
            def __init__(self,actual):self.actual=actual
            def get_section_by_name(self,name):return Section(sh_type='SHT_PROGBITS',sh_flags=2,sh_addr=0x4000,sh_size=16,actual=self.actual)
        build.validate_codegen_hybrid(function,Linked(data),original,sections)
        for actual in (bytes(16),data[:-1]+b'\x01'):
            with self.subTest(actual=actual),self.assertRaisesRegex(ValueError,'complete original'):build.validate_codegen_hybrid(function,Linked(actual),original,sections)
        for flag in (False,None):
            changed={**function,'generated_data_identical':flag}
            with self.assertRaisesRegex(ValueError,'complete linked'):build.validate_codegen_hybrid(changed,Linked(data),original,sections)
        with self.assertRaisesRegex(ValueError,'complete linked'):build.validate_codegen_hybrid(function,None,original,sections)
        with self.assertRaisesRegex(ValueError,'Differing'):build.validate_codegen_hybrid({**function,'mapped_codegen_readonly':[{**mapping,'identical':False}]},Linked(data),original,sections)


if __name__=='__main__': unittest.main()
