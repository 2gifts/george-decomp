#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/property_records.h"
#include "property_records_golden.h"

/* Reuse the already reviewed byte-order implementation unchanged. Only this
 * native translation unit renames its scalar entry so a wrapper records calls. */
#define func_002B2448 property_original_byte_swap
#include "../../src/game/byte_order.c"
#undef func_002B2448

static union { u32 words[128];float scalars[128];u8 bytes[512]; } buffer;
static const struct PropertyGolden *fixture;
static u32 calls[4],events[32],event_count;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<20) printf("line%u routine%u kind%u mutation%u alias%u count%u search%u\n", \
      __LINE__,fixture->routine,fixture->kind,fixture->mutation,fixture->alias,fixture->count,fixture->search); \
    ++failures; } } while (0)
static u32 word(float value)
{ union {float value;u32 bits;} v;v.value=value;return v.bits; }
static float scalar(u32 bits)
{ union {float value;u32 bits;} v;v.bits=bits;return v.value; }
static void event(u32 value)
{ CHECK(event_count<32);if (event_count<32) events[event_count++]=value; }
static void mutate(void)
{
    if (fixture->mutation==1) buffer.words[98]=64;
    else if (fixture->mutation==2) buffer.bytes[261]=0x80;
    else if (fixture->mutation==3) { buffer.bytes[262]=80;buffer.bytes[263]=0; }
}
static u32 convert(const void *payload)
{
    const u8 *p=(const u8 *)payload;
    CHECK(p==buffer.bytes+264||p==buffer.bytes+304||p==buffer.bytes+344);
    ++calls[2];event(2);event(*(const u32 *)p);mutate();return 0x12345678;
}
char *func_00393B74(char *output,const char *input)
{
    ++calls[0];event(0);event((u32)((u8 *)output-buffer.bytes));
    event((u32)((const u8 *)input-buffer.bytes));
    CHECK((u8 *)output>=buffer.bytes&&(u8 *)output<buffer.bytes+512);
    CHECK((const u8 *)input>=buffer.bytes&&(const u8 *)input<buffer.bytes+512);
    strcpy(output,input);mutate();return output;
}
float func_0029C168(float angle)
{
    float result=(float)(0.8-(double)angle*0.05);
    ++calls[1];event(1);event(word(angle));mutate();return result;
}
u32 func_002B2448(u32 value)
{ ++calls[3];event(3);event(value);return property_original_byte_swap(value); }
static u32 canonical(u32 value)
{
    if (value==(u32)convert) return 0xF0000080;
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+512)
        return 0x20000+value-(u32)buffer.bytes;
    return value;
}
static int float_equal(u32 actual,u32 expected)
{
    float a=scalar(actual),e=scalar(expected);
    if (actual==expected) return 1;
    if (!isfinite(a)||!isfinite(e)||e==0.0f) return 0;
    return fabsf(a-e)<=3e-6f*(1.0f+fabsf(e));
}
int main(void)
{
    unsigned c,i;u32 result;
    GeorgePropertyRecord *record=(GeorgePropertyRecord *)(buffer.bytes+256);
    const GeorgePropertyDescriptor *descriptors=(const GeorgePropertyDescriptor *)(buffer.bytes+384);
    for (c=0;c<sizeof(property_golden)/sizeof(property_golden[0]);++c) {
        fixture=&property_golden[c];memcpy(buffer.words,fixture->initial,sizeof(buffer));
        for (i=0;i<4;++i) buffer.words[100+i*6]=(u32)convert;
        memset(calls,0,sizeof(calls));event_count=0;
        if (fixture->routine==0) func_0029A508(&buffer,fixture->empty==1?NULL:record,fixture->count,descriptors);
        else if (fixture->routine==1) {
            GeorgePropertyRecord *next=func_0029A890(record);
            result=next?(u32)next-(u32)buffer.bytes:0;CHECK(result==fixture->result);
        } else { result=func_0029A8B8(record);CHECK(result==fixture->result); }
        for (i=0;i<128;++i) {
            u32 a=canonical(buffer.words[i]),e=fixture->expected[i];
            if (fixture->kind>=11&&fixture->kind<=15&&fixture->routine==0&&e!=fixture->initial[i]&&e!=0)
                CHECK(float_equal(a,e));
            else CHECK(a==e);
        }
        for (i=0;i<4;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("property_records: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
