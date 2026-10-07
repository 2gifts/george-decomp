#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/record_collision.h"
#include "record_collision_golden.h"

#define BUFFER 0x20000u
#define WORDS (0x1600u / 4u)
typedef unsigned long long Bits64;
static unsigned checks, current_fixture, event_count;
static u32 events[512];
static union { u32 words[WORDS]; float values[WORDS]; } arena __attribute__((aligned(16)));
static const unsigned pointer_cells[] = {0x388,0x38C,0x390,0x394,0x398,0x3D8,0x3DC,0x3E0,0x3E4};

static void check(int condition, const char *message, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr, "record collision fixture %u index %u: %s\n", current_fixture,index,message);
        exit(1);
    }
}
static u32 bits(float value)
{
    union { float scalar; u32 bits; } data;
    data.scalar=value;
    return data.bits;
}
static void event(u32 value)
{
    check(event_count < sizeof(events)/sizeof(events[0]),"event bound",event_count);
    events[event_count++]=value;
}
static void *pointer(u32 guest)
{
    check(guest>=BUFFER && guest<BUFFER+WORDS*4 && !(guest&3),"guest pointer scope",guest);
    return (char *)&arena+guest-BUFFER;
}

/* Finite normal/zero models of the four already reviewed soft ABI paths. */
static void soft_event(unsigned kind,Bits64 a,Bits64 b,float value)
{
    event(kind);event((u32)a);event((u32)(a>>32));
    event((u32)b);event((u32)(b>>32));event(bits(value));
}
Bits64 func_00374848(float value)
{
    union { double scalar; Bits64 bits; } data;
    soft_event(0,0,0,value);data.scalar=(double)value;return data.bits;
}
s32 func_00373250(Bits64 left,Bits64 right)
{
    union { double scalar; Bits64 bits; } a,b;
    soft_event(1,left,right,0.0f);a.bits=left;b.bits=right;
    return (a.scalar>b.scalar)-(a.scalar<b.scalar);
}
Bits64 func_00372CC0(Bits64 left,Bits64 right)
{
    union { double scalar; Bits64 bits; } a,b;
    soft_event(2,left,right,0.0f);a.bits=left;b.bits=right;
    a.scalar-=b.scalar;return a.bits;
}
float func_003734F8(Bits64 value)
{
    union { double scalar; Bits64 bits; } data;
    soft_event(3,value,0,0.0f);data.bits=value;return (float)data.scalar;
}

/* Explicit finite VU observation contracts. The original fixture decoder
 * executes the entire VU/MMI bodies. These are unawarded native test models. */
static void transform(const GeorgeRotationMatrix *matrix,const GeorgeMathVec3 *input,
                      GeorgeMathVec3 *output,int translate)
{
    float x=input->x,y=input->y,z=input->z,result[3];
    unsigned i;
    event(translate?4:5);
    for(i=0;i<16;++i)event(bits(matrix->element[i]));
    event(bits(x));event(bits(y));event(bits(z));
    for(i=0;i<3;++i) {
        float product=matrix->element[i]*x;
        float next=matrix->element[i+4]*y;
        product=product+next;
        next=matrix->element[i+8]*z;
        product=product+next;
        if(translate)product=product+matrix->element[i+12]*1.0f;
        result[i]=product;
    }
    output->z=result[2];output->x=result[0];output->y=result[1];
}
void func_002A1C60(const void *matrix,const GeorgeMathVec3 *input,GeorgeMathVec3 *output)
{ transform((const GeorgeRotationMatrix *)matrix,input,output,1); }
void func_002A1D78(const void *matrix,const GeorgeMathVec3 *input,GeorgeMathVec3 *output)
{ transform((const GeorgeRotationMatrix *)matrix,input,output,0); }

/* Unused call closure for the published vector_math separate TU. */
float func_0029B940(float y,float x) { (void)y;(void)x;check(0,"unused atan",0);return 0; }
float func_0029C090(float x) { (void)x;check(0,"unused sine",0);return 0; }
float func_0029C168(float x) { (void)x;check(0,"unused cosine",0);return 0; }
float func_0029C230(float x) { (void)x;check(0,"unused arccosine",0);return 0; }

static void golden_checks(void)
{
    unsigned i,j,k;
    for(i=0;i<sizeof(record_golden)/sizeof(record_golden[0]);++i) {
        const struct RecordGolden *g=&record_golden[i];
        u32 expected[WORDS],actual,result=0;
        current_fixture=i;event_count=0;
        memset(&arena,0,sizeof(arena));memset(expected,0,sizeof(expected));
        for(j=0;j<g->initial_count;++j) {
            unsigned index=g->initial[j].index;
            check(index<WORDS,"sparse input scope",index);
            arena.words[index]=expected[index]=g->initial[j].value;
        }
        for(j=0;j<g->changed_count;++j) {
            unsigned index=g->changed[j].index;
            check(index<WORDS,"sparse expected scope",index);
            expected[index]=g->changed[j].value;
        }
        for(j=0;j<sizeof(pointer_cells)/sizeof(pointer_cells[0]);++j) {
            unsigned index=pointer_cells[j]/4;
            if(arena.words[index])arena.words[index]=(u32)pointer(arena.words[index]);
        }
        if(g->routine==0)
            result=func_00270A00(pointer(g->args[0]),pointer(g->args[1]),
                    pointer(g->args[2]),pointer(g->args[3]),pointer(g->args[4]),
                    pointer(g->args[5]),pointer(g->args[6]));
        else
            func_00270C90(pointer(g->args[0]),pointer(g->args[1]),g->args[2]);
        check(result==g->result,"original return",WORDS);
        for(j=0;j<WORDS;++j) {
            actual=arena.words[j];
            for(k=0;k<sizeof(pointer_cells)/sizeof(pointer_cells[0]);++k)
                if(j==pointer_cells[k]/4 && actual>=(u32)&arena && actual<(u32)&arena+sizeof(arena))
                    actual=BUFFER+actual-(u32)&arena;
            check(actual==expected[j],"original complete memory",j);
        }
        check(event_count==g->event_count,"original call event extent",WORDS+1);
        for(j=0;j<event_count;++j)check(events[j]==g->events[j],"original used call operands",j);
    }
}

static void independent_checks(void)
{
    GeorgeRecordCollision object;
    struct { u32 count; GeorgeCollisionRecord record; } records;
    GeorgeRotationMatrix basis[1];
    GeorgeRotationMatrix frame;
    s16 map[1]={0};
    GeorgeMathVec3 output, first={-2,-1,0}, second={2,-1,0};
    float fraction;
    u32 word;
    unsigned i;
    memset(&object,0,sizeof(object));memset(basis,0,sizeof(basis));
    records.count=1;records.record.field00=0;records.record.field04=0xFEDCBA98u;
    records.record.field08=6.0f;records.record.field10=-1.0f;
    object.field398=(GeorgeCollisionRecords *)&records;
    object.field0C=3;object.field388[2]=map;object.field3D8[2]=basis;
    basis[0].element[8]=2;basis[0].element[9]=-3;basis[0].element[10]=4;
    basis[0].element[12]=5;basis[0].element[13]=6;basis[0].element[14]=7;
    current_fixture=10000;
    func_00270C90(&object,&output,0xFEDCBA98u);
    check(output.x==9 && output.y==0 && output.z==15,"independent midpoint/full key/mode3",0);
    for(i=0;i<3;++i) {
        map[0]=-1;output.x=output.y=output.z=99;
        func_00270C90(&object,&output,records.record.field04);
        check(output.x==0 && output.y==0 && output.z==0,"negative-map zero output",i);
    }
    object.field398=0;output.x=output.y=output.z=99;
    func_00270C90(&object,&output,1);
    check(output.x==0 && output.y==0 && output.z==0,"absent table zero output",0);

    memset(&frame,0,sizeof(frame));memset(basis,0,sizeof(basis));
    frame.element[0]=frame.element[5]=frame.element[10]=frame.element[15]=1;
    basis[0].element[10]=1;basis[0].element[13]=-1;basis[0].element[14]=-1;
    map[0]=0;object.field398=(GeorgeCollisionRecords *)&records;
    records.record.field04=1;records.record.field08=3;
    records.record.field0C=0;records.record.field10=0;
    event_count=0;
    check(func_00270A00(&object,&frame,&first,&second,&fraction,&word,&output)==1,
          "independent crossing hit",0);
    check(fraction==0.5f && word==1,"independent crossing fraction/key",0);
    check(output.x==1 && output.y==0 && output.z==0,"zero residual genuine normalization fallback",0);
    event_count=0;
    check(func_00270A00(&object,&frame,&first,&second,
                       (float *)&records.record.field04,&word,&output)==1,
          "aliased fraction hit",0);
    check(word==bits(0.5f),"record word reload follows fraction publication",0);
}

int main(void)
{
    golden_checks();independent_checks();
    printf("record collision native checks: %u\n",checks);
    return 0;
}
