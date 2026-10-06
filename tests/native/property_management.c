/* Authored call observations and retained storage, not a heap/VU emulator. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/property_management.h"
#include "property_management_golden.h"

/* Reuse reviewed tracked C unchanged, with macro-only observation adapters. */
static u32 management_crc_table[256];
#define D_00445650 management_crc_table
#define func_00295050 management_original_length
#include "../../src/game/string_algorithms.c"
#undef func_00295050
#undef D_00445650
const char D_00447040[]="unused",D_00447058[]="unused",D_004470B0[]="unused";
const char D_004470E0[]="unused",D_00447110[]="unused",D_00447140[]="unused",D_00447160[]="unused";
#define func_002AD9A8 management_original_initialize
#include "../../src/game/list.c"
#undef func_002AD9A8
#define func_002AD9C0 management_original_append
#define func_002ADAE0 management_original_remove
#include "../../src/game/list_aros.c"
#undef func_002AD9C0
#undef func_002ADAE0

static union { u32 words[640];u8 bytes[2560]; } buffer;
static const struct ManagementGolden *fixture;
static u32 calls[9],events[128],event_count;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<16) printf("line %u routine %u failure %u mutation %u name %u\n", \
      __LINE__,fixture?fixture->routine:0,fixture?fixture->failure:0,fixture?fixture->mutation:0,fixture?fixture->name:0); \
    ++failures; } } while (0)
static void *pointer(u32 value) { return buffer.bytes+value-0x20000; }
static u32 native_value(u32 value)
{
    if (value>=0x20000&&value<0x20A00)return (u32)pointer(value);
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+sizeof buffer)
        return 0x20000+value-(u32)buffer.bytes;
    return value;
}
static u32 load(u32 offset) { return canonical(buffer.words[offset/4]); }
static void store(u32 offset,u32 value) { buffer.words[offset/4]=native_value(value); }
static void event(u32 value)
{ CHECK(event_count<128);if (event_count<128)events[event_count++]=value; }
static void call(u32 index) { ++calls[index];event(index); }
static float single(u32 bits) { union { u32 u;float f; } value;value.u=bits;return value.f; }

void *func_002AEB60(u32 size,u32 alignment)
{
    call(0);event(size);event(alignment);CHECK(size==0xC0&&alignment==4);
    if (fixture->mutation==1)buffer.bytes[0x941]='M';
    return fixture->failure==1?NULL:pointer(0x20100);
}
void *func_002AEC28(u32 size)
{
    call(1);event(size);event(load(0x1B0));
    if (fixture->routine==3) {
        CHECK(size==12);
        return fixture->failure==1?NULL:pointer(0x20700);
    }
    CHECK(size<=64);
    if (fixture->mutation==3) { store(0x1B0,0x20981);buffer.bytes[0x941]='N'; }
    return fixture->failure==2?NULL:pointer(0x20801);
}
void func_002AD9A8(GeorgeList *list)
{
    call(2);event(canonical((u32)list));event(load(0x100));event(load(0x104));event(load(0x108));
    management_original_initialize(list);
    /* Explicit authored hook beyond the pure helper's observed effects. */
    if (fixture->mutation==5) { store(0x108,0x20500);store(0x148,0x41424344); }
}
void func_002A1C30(GeorgeRotationMatrix *matrix)
{
    const u32 offsets[]={0x12C,0x130,0x134,0x138,0x13C,0x140,0x144,0x148};
    const u32 rows[]={3,0,1,2};u32 i,col;
    GeorgePropertyOwnedNode *node=(GeorgePropertyOwnedNode *)pointer(0x20100);
    call(3);event(canonical((u32)matrix));event(node->flags);
    for (i=0;i<sizeof offsets/sizeof offsets[0];++i)event(load(offsets[i]));
    for (i=0;i<4;++i)for (col=0;col<4;++col)matrix->element[rows[i]*4+col]=rows[i]==col?1.0f:0.0f;
}
void func_002AEE40(void *memory)
{
    u32 address=canonical((u32)memory);
    call(4);event(address);event(load(0x1B0));event(load(0x590));
    CHECK(address>=0x20000&&address<0x20A00);
    if (fixture->mutation==2&&address==0x20901) { store(0x1B0,0x20981);buffer.bytes[0x941]='Q'; }
    if (fixture->mutation==6&&address!=0x20500)store(0x590,0x20981);
}
u32 func_00295050(const signed char *text)
{
    u32 value;
    call(5);event(canonical((u32)text));event(load(0x1B0));
    value=management_original_length(text);
    event(5);event(value);
    return value;
}
void *func_00393B74(void *destination,const void *source)
{
    const u8 *input=(const u8 *)source;u8 *output=(u8 *)destination;u32 i;
    call(6);event(canonical((u32)destination));event(canonical((u32)source));event(load(0x1B0));
    /* Forward byte contract for the original misaligned scalar branch only.
     * No host libc or aligned MMI implementation is substituted. */
    CHECK((((u32)destination|(u32)source)&7)!=0);
    for (i=0;i<64;++i) { output[i]=input[i];if (input[i]==0)break; }
    CHECK(i<64);
    return destination;
}
void func_002ADAE0(GeorgeListNode *node)
{
    call(7);event(canonical((u32)node));event(canonical((u32)node->next));event(canonical((u32)node->previous));event(load(0x590));
    management_original_remove(node);
    /* Explicit post-helper mutation tests the destructor's fresh payload load. */
    if (fixture->mutation==4)store(0x590,0x20981);
}
void func_002AD9C0(GeorgeList *list,GeorgeListNode *node)
{
    call(8);event(canonical((u32)list));event(canonical((u32)node));event(load(0x700));event(load(0x708));
    management_original_append(list,node);
}

int main(void)
{
    const u32 names[]={0,0x20941,0x20901,0x20902,0x20148,0x20961};u32 n,i,bit;
    CHECK(sizeof(void *)==4&&sizeof(GeorgePropertyOwnedNode)==0xB8);
    for (i=0;i<256;++i) {
        u32 value=i;
        for (bit=0;bit<8;++bit)value=(value>>1)^(value&1?0xEDB88320u:0);
        management_crc_table[i]=value;
    }
    for (n=0;n<sizeof management_golden/sizeof management_golden[0];++n) {
        GeorgePropertyOwnedNode *node=(GeorgePropertyOwnedNode *)pointer(0x20100);
        u32 result=0;
        fixture=&management_golden[n];
        for (i=0;i<640;++i)buffer.words[i]=native_value(fixture->initial[i]);
        memset(calls,0,sizeof calls);event_count=0;
        switch (fixture->routine) {
        case 0:result=canonical((u32)func_002B9530(names[fixture->name]?(const signed char *)pointer(names[fixture->name]):NULL,
               single(fixture->float_words[0]),single(fixture->float_words[1]),single(fixture->float_words[2])));break;
        case 1:func_002B95B8(node,names[fixture->name]?(const signed char *)pointer(names[fixture->name]):NULL);break;
        case 2:func_002BA110((GeorgePropertyTextNode *)pointer(0x20500));break;
        case 3:result=canonical((u32)func_002BA170());break;
        case 4:func_002BA1B0((GeorgeList *)pointer(0x20700),&node->link);break;
        default:result=func_002BA268((GeorgeList *)pointer(0x20700));break;
        }
        for (i=0;i<640;++i)CHECK(canonical(buffer.words[i])==fixture->expected[i]);
        CHECK(result==fixture->result);
        for (i=0;i<9;++i)CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i)CHECK(events[i]==fixture->events[i]);
    }
    printf("property_management: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
