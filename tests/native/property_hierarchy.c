#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/property_hierarchy.h"
#include "property_hierarchy_golden.h"

/* Reuse the unchanged pinned newlib1.8.1 source imported by the reuse agent.
 * Only symbol/header macros adapt
 * this exact public file to the native caller-observation wrapper. */
#define _CONST const
#define _AND ,
#define _DEFUN(name,args,decls) name(decls)
#define strstr property_original_strstr
#include "../../src/runtime/strstr.c"
#undef strstr
#undef _DEFUN
#undef _AND
#undef _CONST

static union { u32 words[640];u8 bytes[2560]; } buffer;
static const struct HierarchyGolden *fixture;
static u32 calls[3],events[32],event_count;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<20) printf("line%u routine%u key%u shape%u mutation%u alias%u flags%u remaining%u stop%u\n", \
      __LINE__,fixture->routine,fixture->key,fixture->shape,fixture->mutation,fixture->alias,fixture->flags,fixture->remaining,fixture->stop); \
    ++failures; } } while (0)
static u32 relative(const void *value)
{ return (u32)value-(u32)buffer.bytes; }
static void event(u32 value)
{ CHECK(event_count<32);if (event_count<32) events[event_count++]=value; }
static void store(u32 offset,u32 value)
{ buffer.words[offset/4]=value; }
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer.bytes&&value<(u32)buffer.bytes+sizeof buffer)
        return 0x20000+value-(u32)buffer.bytes;
    return value;
}
static void mutate(u32 index,int callback)
{
    if (callback) {
        if (fixture->mutation==1&&index==0) store(0x160,(u32)(buffer.bytes+0x300));
        if (fixture->mutation==2&&index==1) store(0x200,0);
        if (fixture->mutation==3&&index==1) store(0x300,0);
    } else {
        if (fixture->mutation==4&&index==0) store(0x160,(u32)(buffer.bytes+0x300));
        if (fixture->mutation==5&&index==1) store(0x200,0);
    }
}
s32 func_00393A28(const char *first,const char *second)
{
    s32 result;
    ++calls[0];event(0);event(relative(first));event(relative(second));
    result=strcmp(first,second);mutate((relative(first)-0x700)/32,0);
    return result;
}
char *func_00398628(const char *first,const char *second)
{
    char *result;
    ++calls[1];event(1);event(relative(first));event(relative(second));
    result=property_original_strstr(first,second);mutate((relative(first)-0x700)/32,0);
    return result;
}
static s32 callback(GeorgePropertyHierarchyNode *node,void *data)
{
    u32 index=(relative(node)-0x100)/256;
    ++calls[2];event(2);event(relative(node));event(relative(data));
    CHECK(index<5&&relative(data)==0x900);mutate(index,1);
    return index==fixture->stop?0:(s32)fixture->callback_value;
}
int main(void)
{
    unsigned c,i;u32 result;
    GeorgePropertyHierarchyNode *root=(GeorgePropertyHierarchyNode *)(buffer.bytes+0x100);
    for (c=0;c<sizeof hierarchy_golden/sizeof hierarchy_golden[0];++c) {
        static const u32 offsets[4]={0x600,0x30C,0x168,0x10C};
        GeorgePropertyHierarchyNode **output;
        const signed char *key;
        fixture=&hierarchy_golden[c];memcpy(buffer.words,fixture->initial,sizeof buffer);
        for (i=0;i<640;++i) if (buffer.words[i]>=0x20000&&buffer.words[i]<0x20A00)
            buffer.words[i]=(u32)buffer.bytes+buffer.words[i]-0x20000;
        memset(calls,0,sizeof calls);event_count=0;
        output=(GeorgePropertyHierarchyNode **)(buffer.bytes+offsets[fixture->alias]);
        key=NULL;
        if (fixture->routine==0||fixture->routine==1||fixture->routine==4)
            key=(const signed char *)(buffer.bytes+0x800+fixture->key*16);
        switch (fixture->routine) {
        case 0:result=(u32)func_002B9928(root,key);break;
        case 1:result=func_002B99B0(root,key,output);break;
        case 2:result=func_002B9A58(root,fixture->key,fixture->remaining,output);break;
        case 3:result=(u32)func_002B9B68(root,fixture->flags,callback,buffer.bytes+0x900);break;
        case 4:result=(u32)func_002B9D88((const GeorgeList *)&buffer,key);break;
        default:result=func_002B9E38((const GeorgeList *)&buffer,fixture->key,fixture->remaining,output);break;
        }
        if ((fixture->routine==0||fixture->routine==4)&&result) result-=(u32)buffer.bytes;
        CHECK(result==fixture->result);
        for (i=0;i<640;++i) CHECK(canonical(buffer.words[i])==fixture->expected[i]);
        for (i=0;i<3;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("property_hierarchy: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
