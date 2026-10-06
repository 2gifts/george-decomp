/* Authored ownership/call observations; retained storage is not a heap model. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/property_lifecycle.h"
#include "property_lifecycle_golden.h"

/* Reuse actual reviewed list C. The unrelated diagnostic entries use authored
 * stand-ins and are never called. AROS provenance stays in its source file. */
const char D_00447040[]="unused",D_00447058[]="unused",D_004470B0[]="unused";
const char D_004470E0[]="unused",D_00447110[]="unused",D_00447140[]="unused",D_00447160[]="unused";
#define func_002AD9A8 native_list_initialize
#include "../../src/game/list.c"
#undef func_002AD9A8
#define func_002ADAE0 native_list_remove
#include "../../src/game/list_aros.c"
#undef func_002ADAE0

static union { u32 words[640];u8 bytes[2560]; } buffer;
static const struct LifecycleGolden *fixture;
static u32 calls[6],events[128],event_count;
static unsigned checks,failures;
#define CHECK(value) do { ++checks;if (!(value)) { \
    if (failures<16) printf("line%u routine%u mode%u mutation%u replacement%u\n", \
      __LINE__,fixture?fixture->routine:0,fixture?fixture->mode:0,fixture?fixture->mutation:0,fixture?fixture->replacement:0); \
    ++failures; } } while (0)
static void destroy_primary(GeorgePropertyOwnedNode *node);
static void destroy_secondary(GeorgePropertyOwnedNode *node);
static void *pointer(u32 value)
{ return buffer.bytes+value-0x20000; }
static u32 native_value(u32 value)
{
    if (value>=0x20000&&value<0x20A00) return (u32)pointer(value);
    if (value==0xF00000A0) return (u32)destroy_primary;
    if (value==0xF00000B0) return (u32)destroy_secondary;
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+sizeof buffer)
        return 0x20000+value-(u32)buffer.bytes;
    if (value==(u32)destroy_primary) return 0xF00000A0;
    if (value==(u32)destroy_secondary) return 0xF00000B0;
    return value;
}
static u32 load(u32 offset)
{ return canonical(buffer.words[offset/4]); }
static void store(u32 offset,u32 value)
{ buffer.words[offset/4]=native_value(value); }
static void half(u32 offset,u16 value)
{ memcpy(buffer.bytes+offset,&value,2); }
static void empty(u32 address)
{ native_list_initialize((GeorgeList *)pointer(address)); }
static void event(u32 value)
{ CHECK(event_count<128);if (event_count<128) events[event_count++]=value; }
static void call(u32 index,const void *argument)
{ ++calls[index];event(index);event(canonical((u32)argument)); }
static void mutate(u32 index,u32 address)
{
    u32 m=fixture->mutation;
    if (index==0&&m==1) {
        store(0x100,0x20004);store(0x160,0x20300);store(0x12C,0x42DE0000);
        store(0x10C,777);half(0x11C,0xBEEF);store(0x148,0x41A80000);
    } else if (index==1&&m==2) {
        half(0x11C,0xF123);store(0x12C,0xC29A0000);store(0x10C,1234);
        store(0x148,0x43A68000);store(0x1B0,0x20920);store(0x1B4,0x20700);
    } else if (index==2) {
        if (m==7&&address==0x20100) store(0x124,0xF00000B0);
        if (m==8&&address==0x20500) store(0x590,0x20840);
        if (m==13&&address==0x20100) store(0x108,0x20300);
    } else if (index==3) {
        if (m==3&&address==0x20800) {
            store(0x1B0,0x20920);empty(0x20160);
            store(0x200,0);store(0x204,0);store(0x300,0);store(0x304,0);
        }
        if (m==4&&address==0x20900) {
            store(0x160,0x20300);store(0x304,0x20160);store(0x200,0);store(0x204,0);
        }
        if (m==9&&address==0x20400) store(0x1B4,0x20700);
        if (m==10&&address==0x20800) { empty(0x20400);store(0x600,0);store(0x604,0); }
        if (m==11&&address==0x20900) store(0x1B4,0x20700);
        if (m==12&&address==0x20500) store(0x1B4,0x20700);
    } else if (index>=4&&address==0x20200) {
        if (m==5) { empty(0x20160);store(0x300,0);store(0x304,0); }
        if (m==6) store(0x200,0x20264);
    }
}
void func_002AD9A8(GeorgeList *list)
{
    call(0,list);event(load(0x100));event(load(0x104));event(load(0x108));
    native_list_initialize(list);mutate(0,canonical((u32)list));
}
void func_002ADAE0(GeorgeListNode *node)
{
    call(2,node);event(canonical((u32)node->next));event(canonical((u32)node->previous));
    native_list_remove(node);mutate(2,canonical((u32)node));
}
void func_002A1C30(GeorgeRotationMatrix *matrix)
{
    const u32 offsets[]={0x12C,0x130,0x134,0x138,0x13C,0x140,0x144,0x148,0x1B4,0x1B0};
    const u32 rows[]={3,0,1,2};u32 i,col;
    GeorgePropertyOwnedNode *root=(GeorgePropertyOwnedNode *)pointer(0x20100);
    call(1,matrix);event(root->flags);
    for (i=0;i<sizeof offsets/sizeof offsets[0];++i) event(load(offsets[i]));
    for (i=0;i<4;++i) for (col=0;col<4;++col) matrix->element[rows[i]*4+col]=rows[i]==col?1.0f:0.0f;
    mutate(1,canonical((u32)matrix));
}
void func_002AEE40(void *memory)
{ call(3,memory);mutate(3,canonical((u32)memory)); }
static void destroy(u32 index,GeorgePropertyOwnedNode *node)
{
    const u32 offsets[]={8,0xB0,0xB4,0,4,0x60};u32 i,address=canonical((u32)node);
    call(index,node);
    for (i=0;i<sizeof offsets/sizeof offsets[0];++i) event(load(address-0x20000+offsets[i]));
    mutate(index,address);
}
static void destroy_primary(GeorgePropertyOwnedNode *node) { destroy(4,node); }
static void destroy_secondary(GeorgePropertyOwnedNode *node) { destroy(5,node); }
static float single(u32 bits) { union { u32 u;float f; } v;v.u=bits;return v.f; }

int main(void)
{
    u32 n,i;
    CHECK(sizeof(void *)==4&&sizeof(GeorgePropertyOwnedNode)==0xB8);
    for (n=0;n<sizeof lifecycle_golden/sizeof lifecycle_golden[0];++n) {
        GeorgePropertyOwnedNode *root=(GeorgePropertyOwnedNode *)pointer(0x20100);
        GeorgeList *replacement;
        fixture=&lifecycle_golden[n];
        for (i=0;i<640;++i) buffer.words[i]=native_value(fixture->initial[i]);
        memset(calls,0,sizeof calls);event_count=0;
        replacement=fixture->replacement==2?NULL:(GeorgeList *)pointer(fixture->replacement==1?0x20400:0x20700);
        switch (fixture->routine) {
        case 0:func_002B9440(root,single(fixture->float_words[0]),single(fixture->float_words[1]),single(fixture->float_words[2]));break;
        case 1:func_002B9660(root);break;
        case 2:func_002B9720(root);break;
        case 3:func_002B9AF8(root,replacement);break;
        case 4:func_002B9B30(root);break;
        default:func_002BA1D0((GeorgeList *)pointer(0x20400));break;
        }
        for (i=0;i<640;++i) CHECK(canonical(buffer.words[i])==fixture->expected[i]);
        for (i=0;i<6;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("property_lifecycle: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
