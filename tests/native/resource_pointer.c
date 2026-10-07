#include <stdio.h>
#include <stdlib.h>
#include "george/resource_pointer.h"

#define WORDS 768u
#define LARGE_WORDS 18432u
typedef char resource_native_pointer32[(sizeof(void *)==4)?1:-1];
typedef char resource_native_result64[(sizeof(GeorgeResourcePointerBits)==8)?1:-1];
#include "resource_pointer_golden.h"

static u32 arena_words;
static unsigned long checks;

static u32 initial_word(u32 i,u32 salt)
{
    return 0x692F1301u^(i*0x1020305u)^salt;
}

/* Independent numerical arena, never dereference a synthetic address. */
static u32 read_cell(const u32 *cells,u32 base,u32 address,u32 width)
{
    u32 offset=address-base;
    if ((width!=1u&&width!=2u&&width!=4u)||address%width||
        offset>arena_words*4u-width) abort();
    return (cells[offset/4u]>>((offset%4u)*8u))&
        (width==4u?0xFFFFFFFFu:(1u<<(width*8u))-1u);
}

static void write_cell(u32 *cells,u32 base,u32 address,u32 value,u32 width)
{
    u32 offset=address-base,shift=(offset%4u)*8u;
    u32 mask=width==4u?0xFFFFFFFFu:((1u<<(width*8u))-1u)<<shift;
    if ((width!=1u&&width!=2u&&width!=4u)||address%width||
        offset>arena_words*4u-width) abort();
    cells[offset/4u]=(cells[offset/4u]&~mask)|((value<<shift)&mask);
}

static void initialize(u32 *cells,u32 base,const struct ResourcePointerParams *p)
{
    u32 i,owner=base+4u*p->owner_word,payload=owner+p->payload_delta;
    u32 slot=owner+0x800u,table=owner+0x900u;
    for (i=0;i<arena_words;++i) cells[i]=initial_word(i,p->salt);
    write_cell(cells,base,owner,p->flags,2);
    write_cell(cells,base,owner+2u,p->byte2,1);
    write_cell(cells,base,owner+3u,p->byte3,1);
    write_cell(cells,base,owner+4u,owner+16u,4);
    write_cell(cells,base,owner+8u,payload,4);
    write_cell(cells,base,owner+12u,slot,4);
    write_cell(cells,base,payload,0,4);
    write_cell(cells,base,payload+28u,p->count,2);
    write_cell(cells,base,slot,table,4);
    if (p->routine==0) write_cell(cells,base,payload,p->value,4);
    else if (p->routine==1)
        write_cell(cells,base,owner+16u+4u*p->byte2,p->value,4);
    else if (p->routine==2) {
        if (p->flavor==1) {
            table=payload+28u;
            write_cell(cells,base,slot,table,4);
            write_cell(cells,base,table,p->value,4);
            write_cell(cells,base,table+4u,0,4);
            write_cell(cells,base,table+8u,0x40u,4);
            write_cell(cells,base,table+12u,0x80000000u,4);
        } else {
            u32 groups=(p->flags&1u)?1u<<(p->byte2&31u):1u;
            for (i=0;i<groups*p->count;++i) {
                u32 value;
                if (p->pattern==0) value=0;
                else if (p->pattern==1) value=i%2u?0x40u+4u*i:0;
                else if (p->pattern==2) value=0x20u+4u*i;
                else value=0x80000000u+4u*i;
                write_cell(cells,base,table+4u*i,value,4);
            }
        }
    } else if (p->flavor==1) {
        write_cell(cells,base,owner+12u,payload+28u,4);
        write_cell(cells,base,payload+28u,1u,4);
    } else if (p->flavor==2)
        write_cell(cells,base,owner+12u,owner+12u,4);
    else write_cell(cells,base,slot,p->value,4);
}

static GeorgeResourcePointerBits integer_effects(u32 *cells,u32 base,
                                                const struct ResourcePointerParams *p)
{
    u32 owner=base+4u*p->owner_word;
    if (p->routine==0) {
        u32 payload=read_cell(cells,base,owner+8u,4);
        u32 mask=read_cell(cells,base,payload,4)&0x30000u;
        return mask==0x20000u||mask==0x30000u;
    }
    if (p->routine==1) {
        u32 byte=read_cell(cells,base,owner+2u,1),slot=owner+16u+4u*byte;
        u32 relative;
        write_cell(cells,base,owner+4u,owner+16u,4);
        write_cell(cells,base,owner+12u,slot,4);
        relative=read_cell(cells,base,slot,4);
        write_cell(cells,base,slot,relative+owner,4);
        return 0;
    }
    if (p->routine==2) {
        u32 groups=1u,outer,entry=0u,payload;
        if (read_cell(cells,base,owner,2)&1u)
            groups=1u<<(read_cell(cells,base,owner+2u,1)&31u);
        payload=read_cell(cells,base,owner+8u,4);
        for (outer=0;outer<groups;++outer) {
            u32 count=read_cell(cells,base,payload+28u,2),j=0;
            if (count) for (;;) {
                u32 slot=read_cell(cells,base,owner+12u,4);
                u32 table=read_cell(cells,base,slot,4),at=table+entry*4u;
                u32 relative=read_cell(cells,base,at,4);
                if (relative) write_cell(cells,base,at,relative+owner,4);
                payload=read_cell(cells,base,owner+8u,4);
                ++j;++entry;count=read_cell(cells,base,payload+28u,2);
                if (j>=count) break;
            }
            if (outer+1u<groups) payload=read_cell(cells,base,owner+8u,4);
        }
        return 0;
    }
    {
        u32 old=read_cell(cells,base,owner+3u,1),bit=1u<<(p->index&31u);
        u32 selected=p->mode?((old|bit)&255u):(old&~bit);
        u32 current=read_cell(cells,base,owner+3u,1);
        u32 payload,count,slot,word;
        if (selected==current) return current;
        payload=read_cell(cells,base,owner+8u,4);
        old=read_cell(cells,base,owner+3u,1);count=read_cell(cells,base,payload+28u,2);
        slot=read_cell(cells,base,owner+12u,4);word=read_cell(cells,base,slot,4);
        write_cell(cells,base,slot,word-4u*count*old,4);
        write_cell(cells,base,owner+3u,selected,1);
        payload=read_cell(cells,base,owner+8u,4);
        slot=read_cell(cells,base,owner+12u,4);count=read_cell(cells,base,payload+28u,2);
        word=read_cell(cells,base,slot,4)+4u*count*selected;
        write_cell(cells,base,slot,word,4);
        return (GeorgeResourcePointerBits)(signed long long)(s32)word;
    }
}

int main(void)
{
    static u32 synthetic[LARGE_WORDS],synthetic_expected[LARGE_WORDS];
    static u32 actual[LARGE_WORDS],expected[LARGE_WORDS];
    static const u32 words[]={0,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu};
    static const GeorgeResourcePointerBits bits[]={0,0x7FFFFFFFULL,
        0xFFFFFFFF80000000ULL,0xFFFFFFFFFFFFFFFFULL};
    u32 i,j;
    for (i=0;i<4;++i) {
        if ((GeorgeResourcePointerBits)(signed long long)(s32)words[i]!=bits[i]) return 2;
        ++checks;
    }
    for (i=0;i<sizeof(resource_pointer_golden)/sizeof(resource_pointer_golden[0]);++i) {
        const struct ResourcePointerGolden *g=&resource_pointer_golden[i];
        struct ResourcePointerParams p=g->p;
        u32 base=(u32)actual,owner;
        GeorgeResourcePointerBits result,wanted,original_result;
        arena_words=g->words;
        initialize(synthetic,0x20000u,&g->p);
        for (j=0;j<arena_words;++j) synthetic_expected[j]=synthetic[j];
        original_result=integer_effects(synthetic_expected,0x20000u,&g->p);
        for (j=0;j<g->changes;++j) synthetic[g->change[j].cell]=g->change[j].value;
        for (j=0;j<arena_words;++j) {
            if (synthetic[j]!=synthetic_expected[j]) {
                fprintf(stderr,"fixture %u original oracle cell %u\n",i,j);return 1;
            }
            ++checks;
        }
        if ((p.routine==0||p.routine==3)&&original_result!=g->original_result) {
            fprintf(stderr,"fixture %u original oracle result\n",i);return 1;
        }
        ++checks;
        if (p.routine==2&&p.flavor==1) {
            u32 offset=(0xFFFCu-(base&0xFFFFu))&0xFFFFu;
            if (offset%4u||offset>65532u||LARGE_WORDS*4u-offset<8196u) return 2;
            p.owner_word=offset/4u;
        }
        initialize(actual,base,&p);
        for (j=0;j<arena_words;++j) expected[j]=actual[j];
        wanted=integer_effects(expected,base,&p);owner=base+4u*p.owner_word;
        if (p.routine==0) result=george_resource_payload_is_kind2_or3((const void *)owner);
        else if (p.routine==1) {func_002B62E0((void *)owner);result=0;}
        else if (p.routine==2) {func_002B6308((void *)owner);result=0;}
        else result=george_resource_selector((void *)owner,p.index,p.mode);
        if (result!=wanted) {
            fprintf(stderr,"fixture %u lower64 %016llX != %016llX\n",i,result,wanted);return 1;
        }
        ++checks;
        for (j=0;j<arena_words;++j) {
            if (actual[j]!=expected[j]) {
                fprintf(stderr,"fixture %u cell %u %08X != %08X\n",i,j,actual[j],expected[j]);return 1;
            }
            ++checks;
        }
    }
    printf("resource pointer: %lu checks\n",checks);
    return 0;
}
