"""Strict whole-object initialized writable data linking and build guards."""
import copy,hashlib,io,os,struct,subprocess,tempfile,unittest
from pathlib import Path
from elftools.elf.elffile import ELFFile
import test_link as fixtures
import link_match,verify,build
H=lambda b:hashlib.sha256(b).hexdigest()
DATA=struct.pack('<6I',5,25,125,625,3125,15625)
ADDRESS=0x348008
CODE=0x110000

def mapping(data=DATA,**changed):
    return dict(address=ADDRESS,expected_bytes=data,expected_sha256=H(data),compiled_symbol='coefficients',**changed)

def image(*,data=DATA,symbols=None,extra=(),words=(0x3C020000,0x24420000),relocations=None,flags=3):
    if symbols is None:symbols=[('coefficients',0,len(data),0x01,6)]
    return fixtures.synthetic_object(target_size=len(words)*4,code_words=words,extra_symbols=symbols,
        extra_sections=[('.data.coefficients',data,flags),*extra],
        relocations=[(0,5,2),(4,6,2)] if relocations is None else relocations)

def original_proof(data=DATA,words=(0x3C020035,0x24428008,0x03E00008,0)):
    code=struct.pack('<'+'I'*len(words),*words)
    # The original data container is larger than the whole mapped item.
    container=struct.pack('<2I',0xAABBCCDD,0x11223344)+data+struct.pack('<2I',0x55667788,0x99AABBCC)
    original=code+bytes(32-len(code))+container
    section=dict(name='.data',offset=32,address=ADDRESS-8,size=len(container),type='SHT_PROGBITS',
                 flags=3,alignment=8,entsize=0,sha256=H(container))
    proof=dict(file_offset=40,size=len(data),address=ADDRESS,original_sha256=H(data),original_section='.data',
               original_section_sha256=H(container),compiled_symbol='coefficients',
               code_bindings=[dict(hi_offset=0,lo_offset=4,relative_offset=0)])
    function=dict(name='fixture',file_offset=0,size=len(code),address=CODE,link_writable_data={'.data.coefficients':proof})
    return function,original,[section]

class WritablePlanTests(unittest.TestCase):
    def inspect(self,raw=None,proof=None,bindings=None,**other):
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'fixture.o';path.write_bytes(image() if raw is None else raw)
            return link_match.inspect_function(path,'fixture',CODE,bindings or {},
                mapped_writable_data={'.data.coefficients':mapping() if proof is None else proof},**other)

    def test_whole_local_object_and_section_symbol_preserved(self):
        symbols=[('coefficients',0,24,0x01,6),('',0,0,0x03,6)]
        plan=self.inspect(image(symbols=symbols))
        item=plan['mapped_writable_data'][0]
        self.assertEqual((item['flags'],item['data'],item['size'],item['compiled_symbol']),(3,DATA,24,'coefficients'))
        self.assertEqual(plan['symbol_patches'],{})

    def test_default_plan_has_no_added_report_field(self):
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'fixture.o';path.write_bytes(fixtures.synthetic_object())
            plan=link_match.inspect_function(path,'fixture',CODE,{})
            self.assertNotIn('mapped_writable_data',plan)

    def test_complete_byte_hash_schema_and_extent_rejections(self):
        for byte in (0,12,23):
            raw=bytearray(DATA);raw[byte]^=1
            with self.subTest(byte=byte),self.assertRaisesRegex(link_match.LinkError,'bytes differ'):
                self.inspect(image(data=bytes(raw)))
        for changed in ({'expected_bytes':DATA[:20],'expected_sha256':H(DATA[:20])},
                        {'expected_bytes':DATA+bytes(4),'expected_sha256':H(DATA+bytes(4))},
                        {'expected_sha256':'0'*64},{'expected_bytes':bytearray(DATA)},
                        {'address':True},{'address':0xFFFFFFF8},{'extra':True},
                        {'relocation_policy':'self_r_mips_32'},{'compiled_symbol':'bad name'}):
            with self.subTest(changed=changed),self.assertRaises(link_match.LinkError):
                self.inspect(proof={**mapping(),**changed})

    def test_writable_readonly_nobits_and_special_flags_remain_distinct(self):
        for field,value in ((1,8),(2,2),(2,6),(2,0x13),(2,0x10000003),(2,0x403),
                            (3,ADDRESS),(8,0),(8,2),(8,3),(8,16),(9,4)):
            with self.subTest(field=field,value=value),self.assertRaises(link_match.LinkError):
                self.inspect(fixtures.section_field(image(),6,field,value))
        with tempfile.TemporaryDirectory() as folder:
            path=Path(folder)/'fixture.o';path.write_bytes(image())
            with self.assertRaisesRegex(link_match.LinkError,'read-only'):
                link_match.inspect_function(path,'fixture',CODE,{},mapped_sections={'.data.coefficients':fixtures.data_mapping(DATA)})
            with self.assertRaises(link_match.LinkError):
                link_match.inspect_function(path,'fixture',CODE,{},mapped_nobits={'.data.coefficients':{'address':ADDRESS,'size':24,'zero_sha256':H(bytes(24))}})

    def test_complete_symbol_not_subobject_or_alias(self):
        for symbols in ([('coefficients',4,20,0x01,6)],[('coefficients',0,20,0x01,6)],
                        [('coefficients',0,24,0x11,6)],[('coefficients',0,24,0x02,6)],
                        [('',0,0,0x03,6)],
                        [('coefficients',0,24,0x01,6),('alias',0,24,0x01,6)],
                        [('coefficients',0,24,0x01,6),('sub',4,0,0x00,6)],
                        [('coefficients',0,24,0x01,6),('coefficients',0,0,0x10,0)]):
            with self.subTest(symbols=symbols),self.assertRaises(link_match.LinkError):self.inspect(image(symbols=symbols))
        with self.assertRaisesRegex(link_match.LinkError,'conflicts'):
            self.inspect(bindings={'coefficients':ADDRESS+4})

    def test_all_initializer_relocations_are_rejected(self):
        # Even zero addends and same-section pointers may not use a policy.
        for kind in (2,5,6):
            raw=image(extra=[('.rel.data',struct.pack('<II',0,2<<8|kind),0)])
            for field,value in ((1,9),(6,3),(7,6),(9,8)):raw=fixtures.section_field(raw,7,field,value)
            with self.subTest(kind=kind),self.assertRaisesRegex(link_match.LinkError,'Relocation-bearing'):self.inspect(raw)
        raw=fixtures.section_field(raw,7,1,4)
        with self.assertRaises(link_match.LinkError):self.inspect(raw)

    def test_code_relocation_bounds_control_and_unused_reject(self):
        for words,relocations in (((0x3C020000,0x24420018),None),((0x3C020000,0x2442FFFF),None),
                                  ((0x3C020000,0x24420000),[(0,5,2)]),
                                  ((0x0C000000,0),[(0,4,2)]),((0,0),[])):
            with self.subTest(words=words,relocations=relocations),self.assertRaises(link_match.LinkError):
                self.inspect(image(words=words,relocations=relocations))
        plan=self.inspect(image(words=(0x3C020000,0x24420014)))
        self.assertEqual(plan['mapped_writable_data'][0]['size'],24)

    def test_missing_ambiguous_and_overlapping_sections_reject(self):
        with self.assertRaisesRegex(link_match.LinkError,'ambiguous'):
            self.inspect(image(extra=[('.data.coefficients',DATA,3)]))
        with self.assertRaisesRegex(link_match.LinkError,'overlap'):
            self.inspect(proof={**mapping(),'address':CODE})
        for name,flags,other in (('.rodata',2,'mapped_sections'),('.bss',3,'mapped_nobits')):
            raw=image(extra=[(name,bytes(24),flags)])
            if name=='.bss':raw=fixtures.section_field(raw,7,1,8)
            proof=fixtures.data_mapping(bytes(24),ADDRESS) if flags==2 else {'address':ADDRESS,'size':24,'zero_sha256':H(bytes(24))}
            with self.subTest(name=name),self.assertRaisesRegex(link_match.LinkError,'overlap'):
                self.inspect(raw,**{other:{name:proof}})

    def test_generated_readonly_and_second_writable_overlaps_reject(self):
        words=(0x3C020000,0x24420000,0x3C040000,0x24840000,0x03E00008,0)
        raw=image(words=words,symbols=[('coefficients',0,24,0x01,6),('',0,0,0x03,7),('case',16,0,0x00,1)],
            extra=[('.rodata.cases',bytes(8),2),('.rel.cases',struct.pack('<4I',0,4<<8|2,4,4<<8|2),0)],
            relocations=[(0,5,2),(4,6,2),(8,5,3),(12,6,3)])
        for field,value in ((1,9),(6,3),(7,7),(9,8)):raw=fixtures.section_field(raw,8,field,value)
        generated=dict(address=ADDRESS,expected_bytes=struct.pack('<2I',CODE+16,CODE+16),
            expected_sha256=H(struct.pack('<2I',CODE+16,CODE+16)),original_function_size=24)
        with self.assertRaisesRegex(link_match.LinkError,'overlap'):
            self.inspect(raw,mapped_codegen_readonly={'.rodata.cases':generated})
        raw=image(extra=[('.data.second',DATA,3)],symbols=[('coefficients',0,24,0x01,6),('second',0,24,0x01,7)])
        with tempfile.TemporaryDirectory() as folder:
            p=Path(folder)/'fixture.o';p.write_bytes(raw)
            with self.assertRaisesRegex(link_match.LinkError,'overlap'):
                link_match.inspect_function(p,'fixture',CODE,{},mapped_writable_data={
                    '.data.coefficients':mapping(),'.data.second':{**mapping(),'compiled_symbol':'second'}})

class WritableOriginalTests(unittest.TestCase):
    def test_exact_item_inside_larger_authenticated_writable_section(self):
        function,raw,sections=original_proof()
        out=verify.mapped_initialized_data(function,raw,sections)['.data.coefficients']
        self.assertEqual(out,mapping())
        self.assertGreater(sections[0]['size'],len(out['expected_bytes']))
        self.assertEqual(verify.mapped_initialized_data({'name':'unused'},raw,sections),{})

    def test_containing_section_and_file_vma_full_hash_proofs_reject(self):
        for changed in ({'type':'SHT_NOBITS'},{'flags':2},{'flags':7},{'offset':33},{'address':ADDRESS},
                        {'size':24},{'sha256':'0'*64},{'alignment':3},{'entsize':4},{'offset':True}):
            f,raw,s=original_proof();s[0].update(changed)
            with self.subTest(changed=changed),self.assertRaises(ValueError):verify.mapped_initialized_data(f,raw,s)
        for changed in ({'address':ADDRESS+4},{'file_offset':44},{'size':20},{'size':True},
                        {'original_sha256':'0'*64},{'original_section_sha256':'0'*64},
                        {'original_section':'.missing'},{'extra':1},{'code_bindings':[]}):
            f,raw,s=original_proof();f['link_writable_data']['.data.coefficients'].update(changed)
            with self.subTest(changed=changed),self.assertRaises(ValueError):verify.mapped_initialized_data(f,raw,s)

    def test_real_pointer_carry_range_and_intervening_instruction_proof(self):
        f,raw,s=original_proof();self.assertTrue(verify.mapped_initialized_data(f,raw,s))
        for upper,lower in ((0x3C020034,0x24428008),(0x3C020035,0x2442800C),
                            (0x3C020035,0x24628008),(0x3C000035,0x24428008)):
            f,raw,s=original_proof(words=(upper,lower,0x03E00008,0))
            with self.subTest(upper=upper,lower=lower),self.assertRaises(ValueError):verify.mapped_initialized_data(f,raw,s)
        for middle,valid in ((0x00001880,True),(0x00001000,False),(0x10000000,False)):
            f,raw,s=original_proof(words=(0x3C020035,middle,0x24428008,0x03E00008,0))
            f['link_writable_data']['.data.coefficients']['code_bindings'][0]['lo_offset']=8
            if valid:self.assertTrue(verify.mapped_initialized_data(f,raw,s))
            else:
                with self.assertRaises(ValueError):verify.mapped_initialized_data(f,raw,s)

class WritableRealLinkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):fixtures.RealLinkTests.setUpClass.__func__(cls)

    def link(self,data=DATA,readonly=False):
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);source=root/'fixture.s';obj=root/'fixture.o'
            body='lui $v0,%hi(coefficients)\naddiu $v0,$v0,%lo(coefficients)\nlw $v1,0($v0)\nsw $v1,0($v0)\n'
            if readonly:body+='lui $a0,%hi(.LC0)\nlw $a0,%lo(.LC0)($a0)\n'
            extra='.section .data.coefficients,"aw",@progbits\n.align 3\n.type coefficients,@object\ncoefficients:\n'+''.join('.word '+hex(x)+'\n' for x in struct.unpack('<'+'I'*(len(data)//4),data))+'.size coefficients,.-coefficients\n'
            if readonly:extra+='.section .rodata.literal,"a",@progbits\n.align 2\n.LC0:\n.word 0x12345678\n'
            source.write_text('.set noreorder\n.section .text.fixture,"ax",@progbits\n.align 3\n.globl fixture\n.ent fixture\nfixture:\n'+body+'jr $ra\nnop\n.end fixture\n.size fixture,.-fixture\n'+extra)
            cmd=[str(self.assembler),'-EL','-march=r5900','-mabi=eabi','-G0','-no-pad-sections','-o',str(obj),str(source)]
            p=subprocess.run(cmd,capture_output=True,text=True);self.assertEqual(p.returncode,0,p.stderr)
            before=obj.read_bytes()
            result=link_match.link_function(obj,'fixture',CODE,{},root/'linked',
                mapped_writable_data={'.data.coefficients':mapping(data)},
                mapped_sections={'.rodata.literal':fixtures.data_mapping(struct.pack('<I',0x12345678),ADDRESS+0x100)} if readonly else None)
            self.assertEqual(obj.read_bytes(),before);self.assertEqual(result['remaining_relocations'],[])
            actual=Path(result['linked_path']).read_bytes()
            artifact_output=os.environ.get('WRITABLE_TEST_ARTIFACTS')
            if artifact_output:
                out=Path(artifact_output);out.mkdir(parents=True,exist_ok=True)
                tag=('combined' if readonly else 'data'+str(len(data)))
                (out/(tag+'.o')).write_bytes(before);(out/(tag+'.elf')).write_bytes(actual)
                (out/(tag+'.ld')).write_bytes(Path(result['script_path']).read_bytes())
            return result,actual

    def test_real_ld_keeps_whole24_bytes_writable_object_and_reports_exact(self):
        result,raw=self.link();item=result['mapped_writable_data'][0]
        self.assertEqual(item['data'],DATA);self.assertEqual(item['sha256'],H(DATA))
        with io.BytesIO(raw) as stream:
            e=ELFFile(stream);s=e.get_section_by_name(item['output_section'])
            self.assertEqual((s['sh_flags'],s['sh_type'],s['sh_size'],s.data()),(3,'SHT_PROGBITS',24,DATA))
            q=next(q for q in e.get_section_by_name('.symtab').iter_symbols() if q.name=='coefficients')
            self.assertEqual((q['st_value'],q['st_size'],q['st_info']['bind']),(ADDRESS,24,'STB_LOCAL'))

    def test_zero_initialized_progbits_and_readonly_remain_separate(self):
        _,raw=self.link(bytes(4))
        e=ELFFile(io.BytesIO(raw));s=e.get_section_by_name('.george_writable_data_0')
        self.assertEqual((s['sh_type'],s['sh_flags'],s.data()),('SHT_PROGBITS',3,bytes(4)))
        result,raw=self.link(readonly=True);self.assertEqual(len(result['mapped_sections']),1)
        e=ELFFile(io.BytesIO(raw))
        self.assertEqual(e.get_section_by_name('.george_rodata_0')['sh_flags'],2)
        self.assertEqual(e.get_section_by_name('.george_writable_data_0')['sh_flags'],3)

    def test_hybrid_rechecks_actual_whole_writable_output_and_canonical_declaration(self):
        result,raw=self.link();f,original,s=original_proof()
        expected=verify.mapped_initialized_data(f,original,s)
        reports=verify.writable_mapping_report(f['link_writable_data'],expected,result['mapped_writable_data'],s)
        report=dict(mapped_writable_data=reports)
        build.validate_writable_hybrid(report,ELFFile(io.BytesIO(raw)),original,s,f)
        for position in (0,12,23):
            changed=bytearray(raw);e=ELFFile(io.BytesIO(raw));section=e.get_section_by_name('.george_writable_data_0');changed[section['sh_offset']+position]^=1
            with self.subTest(position=position),self.assertRaises(ValueError):
                build.validate_writable_hybrid(report,ELFFile(io.BytesIO(changed)),original,s,f)
        for field,value in ((1,8),(2,2),(3,ADDRESS+8),(5,20),(8,4),(9,4)):
            e=ELFFile(io.BytesIO(raw));changed=fixtures.section_field(raw,e.get_section_index('.george_writable_data_0'),field,value)
            with self.subTest(field=field),self.assertRaises(ValueError):build.validate_writable_hybrid(report,ELFFile(io.BytesIO(changed)),original,s,f)
        e=ELFFile(io.BytesIO(raw));symtab=e.get_section_by_name('.symtab')
        symindex=next(i for i,q in enumerate(symtab.iter_symbols()) if q.name=='coefficients')
        for relative,value in ((4,ADDRESS+4),(8,20)):
            changed=bytearray(raw);struct.pack_into('<I',changed,symtab['sh_offset']+symindex*16+relative,value)
            with self.subTest(symbol_field=relative),self.assertRaises(ValueError):
                build.validate_writable_hybrid(report,ELFFile(io.BytesIO(changed)),original,s,f)
        for changed in ({},dict(mapped_writable_data=[]),dict(mapped_writable_data=reports*2)):
            with self.subTest(report=changed),self.assertRaises(ValueError):build.validate_writable_hybrid(changed,ELFFile(io.BytesIO(raw)),original,s,f)
        with self.assertRaises(ValueError):build.validate_writable_hybrid(report,None,original,s,f)
        with self.assertRaises(ValueError):build.validate_writable_hybrid(report,ELFFile(io.BytesIO(raw)),original,s,{'name':'fixture'})
        for field,value in (('original_sha256','0'*64),('sha256','0'*64),('input_sha256','0'*64),
                            ('alignment',True),('flags',2),('compiled_symbol','other'),('initializer_relocation_count',True)):
            changed=copy.deepcopy(report);changed['mapped_writable_data'][0][field]=value
            with self.subTest(field=field),self.assertRaises(ValueError):build.validate_writable_hybrid(changed,ELFFile(io.BytesIO(raw)),original,s,f)
        self.assertEqual(original,original_proof()[1])

if __name__=='__main__':unittest.main()
