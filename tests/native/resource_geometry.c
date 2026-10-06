/* Synthetic bounded resource-call model, separate from production source.
 * The virtual table is authored: only the observed second-pair ABI is modeled.
 * No allocator, engine classifier, original code or original table is copied.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_geometry.h"
#include "resource_geometry_golden.h"

#define BUFFER 0x20000u
#define END 0x20400u
#define RECORD 0x20040u
#define OTHER 0x20080u
#define OTHER2 0x200C0u
#define HOLDER 0x20100u
#define ALTHOLDER 0x20120u
#define PAYLOAD 0x20180u
#define PAYLOAD2 0x20200u
#define MATRIX 0x20280u
#define FACES 0x20300u
#define KEY 0x20380u
#define TABLE 0x43A408u

static union {u32 words[256];u8 bytes[1024];} arena __attribute__((aligned(16)));
static const struct ResourceGolden *fixture;
static u32 calls[17],events[256],event_count,checks;
static unsigned case_index;

static void fail(const char *message) {fprintf(stderr,"resource case %u: %s\n",case_index,message);exit(1);}
static void require(int condition,const char *message) {checks++;if (!condition) fail(message);}
static void *pointer(u32 address) {require(address>=BUFFER&&address<END,"model pointer outside arena");return arena.bytes+address-BUFFER;}
static u32 canonical(const void *value);
static u32 read_word(u32 address) {u32 value;memcpy(&value,pointer(address),4);return value;}
static void write_word(u32 address,u32 value) {memcpy(pointer(address),&value,4);}
static u8 read_byte(u32 address) {return *(u8 *)pointer(address);}
static void write_byte(u32 address,u8 value) {*(u8 *)pointer(address)=value;}
static u32 bits(float value) {u32 result;memcpy(&result,&value,4);return result;}
static void write_pointer(u32 address,u32 target) {write_word(address,(u32)pointer(target));}
static void log_start(u32 index) {require(index<17,"unknown model event");calls[index]++;events[event_count++]=index;}
static void log_word(u32 value) {require(event_count<256,"event bound");events[event_count++]=value;}
static void virtual_release(void *record,u32 word);
/* Actual +8 signed-adjustment/word3 interface, with an authored native target. */
const struct {GeorgeGoalVirtualWord first,second;} D_0043A408={{0,0,0},{0,0,virtual_release}};
static u32 canonical(const void *value) {
    u32 address=(u32)value,start=(u32)arena.bytes;
    if (address>=start&&address<start+sizeof(arena)) return BUFFER+address-start;
    if (value==&D_0043A408) return TABLE;
    return address;
}
static void holder_event(u32 index,void *holder) {
    u32 a=canonical(holder);require(a==HOLDER||a==ALTHOLDER,"holder ABI");
    log_start(index);log_word(a);log_word(canonical((void *)read_word(a+8)));log_word(read_byte(RECORD+2));
}

GeorgeResourceRecord *func_00225E80(const void *key,u32 kind) {
    require(canonical(key)==KEY&&kind==1,"lookup ABI");log_start(0);log_word(KEY);log_word(kind);
    return fixture->found?(GeorgeResourceRecord *)pointer(RECORD):0;
}
void *func_002AEE60(u32 size) {require(size==40,"allocate size");log_start(1);log_word(size);return pointer(RECORD);}
void func_00226BC0(GeorgeResourceRecord *record,const void *key,u32 kind,u32 mode,u32 word) {
    require(canonical(record)==RECORD&&canonical(key)==KEY&&kind==1,"constructor ABI");
    log_start(2);log_word(RECORD);log_word(KEY);log_word(kind);log_word(mode);log_word(word);record->flags=(u8)fixture->flags;
}
void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *record) {
    require(mode==1&&word==0&&canonical(record)==RECORD,"mode request ABI");log_start(3);log_word(mode);log_word(word);log_word(RECORD);
}
void func_00226D78(GeorgeResourceRecord *record) {require(canonical(record)==RECORD,"existing record callback");log_start(4);log_word(RECORD);}
void func_00226FB0(GeorgeResourceRecord *record) {
    require(canonical(record)==RECORD,"constructor callback");log_start(5);log_word(RECORD);
    if (fixture->mutation==5)record->flags=8;
}
void func_00226E38(GeorgeResourceRecord *other,void (*callback)(u32,GeorgeResourceRecord *),GeorgeResourceRecord *record,GeorgeResourceRecord *context) {
    require(canonical(other)==OTHER&&callback==func_0020EEC8&&canonical(record)==RECORD&&record==context,"deferred callback ABI");
    log_start(6);log_word(OTHER);log_word(0x20EEC8);log_word(RECORD);log_word(RECORD);
    if (fixture->mutation==8)callback(fixture->callback_result,record);
}
void func_002B62E0(void *holder) {
    holder_event(7,holder);
    if (fixture->mutation==1) {write_pointer(RECORD+0x10,ALTHOLDER);write_byte(RECORD+2,0x80);write_pointer(RECORD+0x24,OTHER2);}
}
void func_002B6308(void *holder) {
    u32 a=canonical(holder);holder_event(8,holder);
    if (fixture->mutation==2) {write_pointer(a+8,PAYLOAD2);write_byte(RECORD+2,1);write_pointer(RECORD+0x10,ALTHOLDER);write_pointer(RECORD+0x24,OTHER2);}
}
void func_002867E8(void *holder,u32 word) {
    u32 a=canonical(holder);require(a==HOLDER||a==ALTHOLDER,"payload handler holder");log_start(9);log_word(a);log_word(word);
    if (fixture->mutation==3) {write_byte(a+2,0);write_byte(RECORD+2,8);}
}
GeorgeResourceRecord *func_0020F6F8(void *payload,u32 mode,u32 word) {
    u32 a=canonical(payload);require((a==PAYLOAD||a==PAYLOAD2)&&mode==0&&word==0x12345678,"retained query ABI");
    log_start(10);log_word(a);log_word(mode);log_word(word);
    if (fixture->mutation==4)write_pointer(RECORD+0x10,ALTHOLDER);
    return fixture->query_null?0:(GeorgeResourceRecord *)pointer(OTHER);
}
void func_0021C508(void *holder) {holder_event(11,holder);}
static void virtual_release(void *record,u32 word) {require(canonical(record)==RECORD&&word==3,"virtual adjusted pair ABI");log_start(16);log_word(RECORD);log_word(word);}
void func_002A1C60(const GeorgeRotationMatrix *matrix,const GeorgeMathVec4 *input,GeorgeMathVec4 *output) {
    u32 i;float source[4];require(canonical(matrix)==MATRIX&&canonical(input)==PAYLOAD+0x30,"sphere transform ABI");
    memcpy(source,input,sizeof(source));log_start(12);log_word(MATRIX);log_word(PAYLOAD+0x30);for(i=0;i<4;i++)log_word(bits(source[i]));
    output->x=source[0]+10;output->y=source[1]+20;output->z=source[2]+30;
    if(fixture->mutation==6) {write_word(PAYLOAD+0x3C,bits(99));write_pointer(RECORD+0x10,ALTHOLDER);write_pointer(HOLDER+8,PAYLOAD2);}
}
u32 func_002A48F0(const GeorgeGeometryFace *faces,const GeorgeMathVec4 *sphere) {
    u32 i,values[4];require(canonical(faces)==FACES,"sphere classifier ABI");memcpy(values,sphere,sizeof(values));
    log_start(14);log_word(FACES);for(i=0;i<4;i++)log_word(values[i]);
    if (fixture->mutation==7) {write_pointer(RECORD+0x10,ALTHOLDER);write_pointer(HOLDER+8,PAYLOAD2);write_word(PAYLOAD+0x40,0x87654321);}
    return fixture->classification;
}
void func_0029FEF8(GeorgeGeometryFrame *frame,const void *input,const GeorgeRotationMatrix *matrix) {
    u32 i,values[16];require(canonical(input)==PAYLOAD+0x40&&canonical(matrix)==MATRIX,"retained frame input ABI");
    log_start(13);log_word(PAYLOAD+0x40);log_word(MATRIX);log_word(read_word(PAYLOAD+0x40));
    for(i=0;i<16;i++)values[i]=0x11000000+i;memcpy(frame,values,sizeof(values));
}
u32 func_002A4808(const GeorgeGeometryFace *faces,const GeorgeGeometryFrame *frame) {
    u32 i,values[16];require(canonical(faces)==FACES,"frame classifier ABI");memcpy(values,frame,sizeof(values));
    log_start(15);log_word(FACES);for(i=0;i<16;i++)log_word(values[i]);return fixture->result;
}

int main(void) {
    u32 i,result;
    for(case_index=0;case_index<sizeof(resource_golden)/sizeof(resource_golden[0]);case_index++) {
        fixture=&resource_golden[case_index];memset(calls,0,sizeof(calls));event_count=0;
        for(i=0;i<256;i++) {
            u32 value=fixture->initial[i];
            if (value>=BUFFER&&value<END)value=(u32)pointer(value);
            else if(value==TABLE)value=(u32)&D_0043A408;
            arena.words[i]=value;
        }
        result=0;
        switch(fixture->routine) {
        case 0:func_0020E920(pointer(RECORD),fixture->mode);break;
        case 1:result=canonical(func_0020EAA0(pointer(KEY),fixture->mode,0xCAFE0123));break;
        case 2:result=canonical(func_0020EC10(pointer(RECORD),pointer(KEY),fixture->mode,0xCAFE0123));break;
        case 3:result=func_0020EC78(pointer(RECORD));break;
        case 4:result=func_0020ECA0(pointer(RECORD),pointer(MATRIX),pointer(FACES));break;
        case 5:func_0020EEC8(fixture->callback_result,pointer(RECORD));break;
        default:fail("unsupported routine");
        }
        require(result==fixture->expected_result,"result differs from original trace");
        for(i=0;i<256;i++)require(canonical((void *)arena.words[i])==fixture->expected[i],"authored memory differs from original trace");
        for(i=0;i<17;i++)require(calls[i]==fixture->calls[i],"callback count differs");
        require(event_count==fixture->event_count,"event length differs");
        for(i=0;i<event_count;i++)require(events[i]==fixture->events[i],"callback order/arguments differ");
    }
    printf("resource geometry: %u checks across %u instruction-derived fixtures passed\n",checks,case_index);return 0;
}
