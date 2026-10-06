/* Authored callback/list observations. Retained storage is not a heap model;
 * the preserved VU call has only an identity-quaternion output contract. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/property_updates.h"
#include "property_updates_golden.h"

/* Reuse reviewed list C with observation wrappers, retaining AROS provenance.
 * These authored diagnostics are for unrelated entries that never execute. */
const char D_00447040[]="unused",D_00447058[]="unused",D_004470B0[]="unused";
const char D_004470E0[]="unused",D_00447110[]="unused",D_00447140[]="unused",D_00447160[]="unused";
#define func_002AD9A8 native_list_initialize
#include "../../src/game/list.c"
#undef func_002AD9A8
#define func_002ADAE0 native_list_remove
#include "../../src/game/list_aros.c"
#undef func_002ADAE0

static union { u32 words[640];u8 bytes[2560]; } buffer;
static const struct PropertyUpdateGolden *fixture;
static u32 calls[9],events[256],event_count;
static unsigned checks,failures;
#define CHECK(value) do { ++checks;if (!(value)) { \
    if (failures<16) printf("line%u routine%u flags%04X graph%u mutation%u\n", \
      __LINE__,fixture?fixture->routine:0,fixture?fixture->flags:0,fixture?fixture->graph:0,fixture?fixture->mutation:0); \
    ++failures; } } while (0)
static void destroy_primary(GeorgePropertyOwnedNode *node);
static void destroy_secondary(GeorgePropertyOwnedNode *node);
static void update_primary(GeorgePropertyOwnedNode *node);
static void update_secondary(GeorgePropertyOwnedNode *node);
static void *pointer(u32 value) { return buffer.bytes+value-0x20000; }
static u32 native_value(u32 value)
{
    if (value>=0x20000&&value<0x20A00) return (u32)pointer(value);
    if (value==0xF00000A0) return (u32)destroy_primary;
    if (value==0xF00000B0) return (u32)destroy_secondary;
    if (value==0xF00000C0) return (u32)update_primary;
    if (value==0xF00000D0) return (u32)update_secondary;
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+sizeof buffer)
        return 0x20000+value-(u32)buffer.bytes;
    if (value==(u32)destroy_primary) return 0xF00000A0;
    if (value==(u32)destroy_secondary) return 0xF00000B0;
    if (value==(u32)update_primary) return 0xF00000C0;
    if (value==(u32)update_secondary) return 0xF00000D0;
    return value;
}
static u32 load(u32 offset) { return canonical(buffer.words[offset/4]); }
static void event(u32 value)
{ CHECK(event_count<256);if (event_count<256) events[event_count++]=value; }
static void call(u32 index,const void *argument)
{ ++calls[index];event(index);event(canonical((u32)argument)); }
static float single(u32 bits) { union { u32 u;float f; } v;v.u=bits;return v.f; }

void func_002AD9A8(GeorgeList *list)
{
    /* Whole lifecycle TU needs this symbol, but no update fixture constructs. */
    CHECK(0);native_list_initialize(list);
}
void func_002A1C30(GeorgeRotationMatrix *matrix)
{
    /* Whole lifecycle TU needs this symbol, but no update fixture constructs. */
    CHECK(0);(void)matrix;
}
void func_002ADAE0(GeorgeListNode *node)
{
    call(2,node);event(canonical((u32)node->next));event(canonical((u32)node->previous));
    native_list_remove(node);
}
void func_002AEE40(void *memory) { call(3,memory); }
static void destroy(u32 index,GeorgePropertyOwnedNode *node)
{
    const u32 offsets[]={8,0xB0,0xB4,0,4,0x60};u32 i,address=canonical((u32)node);
    call(index,node);
    for (i=0;i<sizeof offsets/sizeof offsets[0];++i) event(load(address-0x20000+offsets[i]));
}
static void destroy_primary(GeorgePropertyOwnedNode *node) { destroy(4,node); }
static void destroy_secondary(GeorgePropertyOwnedNode *node) { destroy(5,node); }

void func_002A1F18(GeorgeRotationMatrix *output,const GeorgeMathVec4 *rotation,
                  const GeorgeMathVec3 *position)
{
    u32 i,col,address=canonical((u32)output)-0x70;
    const u32 rows[]={3,0,1,2};float z;
    CHECK(address==0x20100||address==0x20200||address==0x20300);
    CHECK(canonical((u32)rotation)==address+0x38);
    CHECK(canonical((u32)position)==address+0x2C);
    for (i=0;i<4;++i) CHECK(load(address-0x20000+0x38+i*4)==(i==3?0x3F800000:0));
    call(6,output);event(canonical((u32)rotation));event(canonical((u32)position));
    for (i=0;i<4;++i) event(load(address-0x20000+0x38+i*4));
    for (i=0;i<3;++i) event(load(address-0x20000+0x2C+i*4));
    for (i=0;i<4;++i) for (col=0;col<4;++col) output->element[rows[i]*4+col]=rows[i]==col?1.0f:0.0f;
    output->element[12]=position->x;output->element[13]=position->y;
    z=position->z;output->element[15]=1.0f;output->element[14]=z;
}
static void update(u32 index,GeorgePropertyOwnedNode *node)
{
    GeorgePropertyOwnedNode *root=(GeorgePropertyOwnedNode *)pointer(0x20100);
    GeorgePropertyOwnedNode *first=(GeorgePropertyOwnedNode *)pointer(0x20200);
    GeorgePropertyOwnedNode *second=(GeorgePropertyOwnedNode *)pointer(0x20300);
    u32 m=fixture->mutation,address=canonical((u32)node);
    CHECK(node==root||node==first||node==second);
    call(index,node);event(load(address-0x20000+0x18));event(node->flags);
    event(canonical((u32)node->field20));event(canonical((u32)node->children.head));
    if (node==root) {
        if (m==1||m==13) func_002B9628(node);
        if (m==2) node->flags=0x4020;
        if (m==3||m==13) node->flags=0x8010;
        if (m==4) first->field20=NULL;
        if (m==5) func_002ADAE0(&first->link);
        if (m==8) node->field18=99.0f;
        if (m==9) func_002B9748(first,99.0f);
        if (m==11) {
            node->position.x=7.0f;node->position.y=-8.0f;node->position.z=9.0f;
            node->flags=0x10;
        }
        if (m==14) node->flags=0xFFFF;
    }
    if (node==first) {
        if (m==6) func_002ADAE0(&first->link);
        if (m==7) func_002ADAE0(&second->link);
        if (m==12) second->field20=(void *)update_secondary;
    }
    /* Actual callback declaration has one node argument; incidental f12 is
     * intentionally absent from these C observations. The trace clobbers it. */
}
static void update_primary(GeorgePropertyOwnedNode *node) { update(7,node); }
static void update_secondary(GeorgePropertyOwnedNode *node) { update(8,node); }

int main(void)
{
    u32 n,i;
    CHECK(sizeof(void *)==4&&sizeof(GeorgePropertyOwnedNode)==0xB8);
    for (n=0;n<sizeof property_update_golden/sizeof property_update_golden[0];++n) {
        GeorgePropertyOwnedNode *root=(GeorgePropertyOwnedNode *)pointer(0x20100);
        fixture=&property_update_golden[n];
        for (i=0;i<640;++i) buffer.words[i]=native_value(fixture->initial[i]);
        memset(calls,0,sizeof calls);event_count=0;
        switch (fixture->routine) {
        case 0:func_002B9628(root);break;
        case 1:func_002B9748(root,single(fixture->time_bits));break;
        case 2:func_002B9898(root);break;
        case 3:func_002B9C08((GeorgeList *)pointer(0x20000),single(fixture->time_bits));break;
        default:func_002B9D28((GeorgeList *)pointer(0x20000));break;
        }
        for (i=0;i<640;++i) CHECK(canonical(buffer.words[i])==fixture->expected[i]);
        for (i=0;i<9;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("property_updates: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
