#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/property_pack.h"
#include "property_pack_golden.h"

/* Reuse recovered bodies unchanged. Macro-only symbol adapters let the test
 * record calls and supply an algorithm-generated reflected CRC table. */
static u32 property_crc_table[256];
#define D_00445650 property_crc_table
#define func_0029C648 property_original_crc
#include "../../src/game/string_algorithms.c"
#undef func_0029C648
#undef D_00445650
#define func_0029A890 property_original_next
#include "../../src/game/property_records.c"
#undef func_0029A890

static union { u32 words[512];u8 bytes[2048]; } buffer;
static const struct PropertyPackGolden *fixture;
static u32 calls[5],events[64],event_count;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<16) printf("line%u routine%u count%u length%u mutation%u failure%u alias%u\n", \
      __LINE__,fixture->routine,fixture->count,fixture->length,fixture->mutation,fixture->failure,fixture->alias); \
    ++failures; } } while (0)
static u32 relative(const void *value)
{ return (u32)value-(u32)buffer.bytes; }
static void event(u32 value)
{ CHECK(event_count<64);if (event_count<64) events[event_count++]=value; }
static void store(u32 offset,u32 value,u32 size)
{ memcpy(buffer.bytes+offset,&value,size); }
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+sizeof buffer)
        return 0x20000+value-(u32)buffer.bytes;
    return value;
}
void *func_002AEC28(u32 size)
{
    void *result;
    ++calls[0];event(0);event(size);
    result=(fixture->failure==calls[0])?NULL:buffer.bytes+
      (!fixture->routine?0x400:calls[0]==1?0x600:0x700);
    if (!fixture->routine&&fixture->mutation==1&&calls[0]==1) {
        store(0,(u32)(buffer.bytes+0x1A0),4);store(0x1A0,(u32)(buffer.bytes+4),4);
    }
    if (fixture->routine&&fixture->mutation==7&&calls[0]==2) {
        store(0x688,0xFEDCBA98,4);store(0x68C,1,4);
    }
    return result;
}
u32 func_0029C648(const signed char *text)
{
    u32 result;
    ++calls[1];event(1);event(relative(text));
    result=property_original_crc(text);
    if (calls[1]==1) {
        if (fixture->mutation==2) store(0x100,(u32)(buffer.bytes+4),4);
        if (fixture->mutation==3) {
            store(0x188,0x12345678,4);store(0x18C,4,4);store(0x190,(u32)(buffer.bytes+0x350),4);
        }
    }
    return result;
}
char *func_00393B74(char *output,const char *input)
{
    ++calls[2];event(2);event(relative(output));event(relative(input));
    CHECK(relative(output)+strlen(input)+1<=sizeof buffer);
    strcpy(output,input);
    if (fixture->mutation==6) {
        store(0x600,(u32)(buffer.bytes+0x340),4);store(0x604,(u32)(buffer.bytes+0x300),4);
        store(0x688,0xCAFEBABE,4);store(0x68C,99,4);store(0x690,(u32)(buffer.bytes+0x340),4);
    }
    return output;
}
void *func_003934F8(void *output,const void *input,u32 length)
{
    u32 dest=relative(output),source=relative(input);
    ++calls[3];event(3);event(dest);event(source);event(length);
    CHECK(length<=128&&dest+length<=sizeof buffer&&source+length<=sizeof buffer);
    CHECK(length==0||dest>=source+length||source>=dest+length);
    if (length<=128&&dest+length<=sizeof buffer&&source+length<=sizeof buffer) memcpy(output,input,length);
    if (!fixture->routine&&calls[3]==1) {
        if (fixture->mutation==4) store(dest-2,24,2);
        if (fixture->mutation==5) { store(dest-3,0x80,1);store(0x1A0,0,4); }
    }
    if (fixture->routine&&fixture->mutation==8) store(0x690,(u32)(buffer.bytes+0x340),4);
    return output;
}
GeorgePropertyRecord *func_0029A890(const GeorgePropertyRecord *record)
{
    ++calls[4];event(4);event(relative(record));
    return property_original_next(record);
}
/* Other entries in the unchanged source adapters must never be called. */
float func_0029C168(float value)
{ (void)value;abort();return 0; }
u32 func_002B2448(u32 value)
{ (void)value;abort();return 0; }

int main(void)
{
    unsigned c,i,bit;u32 result;
    for (i=0;i<256;++i) {
        u32 value=i;
        for (bit=0;bit<8;++bit) value=(value>>1)^((value&1)?0xEDB88320u:0);
        property_crc_table[i]=value;
    }
    for (c=0;c<sizeof property_pack_golden/sizeof property_pack_golden[0];++c) {
        fixture=&property_pack_golden[c];memcpy(buffer.words,fixture->initial,sizeof buffer);
        for (i=0;i<512;++i) if (buffer.words[i]>=0x20000&&buffer.words[i]<0x20800)
            buffer.words[i]=(u32)buffer.bytes+buffer.words[i]-0x20000;
        memset(calls,0,sizeof calls);event_count=0;
        if (fixture->routine) result=(u32)func_002BA070((signed char *)buffer.bytes+0x300,
                      fixture->kind,fixture->length,buffer.bytes+0x340);
        else {
            static const u32 offsets[4]={16,0x18C,0x228,4};
            result=(u32)func_002B9F38((const GeorgeList *)&buffer,
                                    (u32 *)(buffer.bytes+offsets[fixture->alias]));
        }
        CHECK((result?result-(u32)buffer.bytes:0)==fixture->result);
        for (i=0;i<512;++i) CHECK(canonical(buffer.words[i])==fixture->expected[i]);
        for (i=0;i<5;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("property_pack: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
