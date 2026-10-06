/* Authored finite call observations; real bounds and group helpers run in
 * separate translation units. No original instructions or tables are copied. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_bounds.h"
#include "resource_bounds_golden.h"

static void native_child_release(void *,u32);
static void native_payload_release(void *,u32);
/* Only the proven zero-adjustment destructor geometry is represented. These
 * dispatches execute production C, including the factory's failure release. */
const struct {
    u32 unobserved_header[2]; GeorgeGoalVirtualWord destroy;
} D_0043A458={{0,0},{0,0,(void (*)(void *,u32))func_0020F9B0}},
  D_0043A4B0={{0,0},{0,0,(void (*)(void *,u32))func_0020FCC0}};
void *D_004682DC;

static u8 *buffer;
static const struct ResourceBoundsGolden *fixture;
static u32 calls[11],events[512],event_count;
static unsigned checks,failures,case_index;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures<20) printf("line %u case %u routine %u mutation %u\n",__LINE__,case_index,fixture?fixture->routine:0,fixture?fixture->mutation:0); \
    ++failures; } } while (0)
static void *pointer(u32 value) { return buffer+value-0x20000; }
static u32 native_value(u32 value)
{
    if (value>=0x20000&&value<0x20B00)return (u32)pointer(value);
    if (value==0xF00000C0)return (u32)native_child_release;
    if (value==0xF00000D0)return (u32)native_payload_release;
    if (value==0x43A458)return (u32)&D_0043A458;
    if (value==0x43A4B0)return (u32)&D_0043A4B0;
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer&&value<(u32)buffer+0xB00)return 0x20000+value-(u32)buffer;
    if (value==(u32)native_child_release)return 0xF00000C0;
    if (value==(u32)native_payload_release)return 0xF00000D0;
    if (value==(u32)&D_0043A458)return 0x43A458;
    if (value==(u32)&D_0043A4B0)return 0x43A4B0;
    return value;
}
static int pointer_field(u32 offset)
{
    const u32 holders[]={0x200,0x240},children[]={0x300,0x340,0x380,0x3C0};
    const u32 primary[]={0x700,0x800};u32 i,j;
    int group=fixture->routine<=5||fixture->routine==13;
    if (offset==0x10||offset==0x20||offset==0x110||offset==0x120||offset==0x124)return 1;
    if (group&&offset>=0x28&&offset<0xA8)return 1;
    if (!group&&offset==0x24)return 1;
    for (i=0;i<2;++i) {
        if (offset==holders[i]+0x20)return 1;
        for (j=0;j<4;++j)if (offset==primary[i]+j*0x24)return 1;
    }
    for (i=0;i<4;++i)
        if (offset==children[i]+0x10||offset==children[i]+0x20||offset==0x400+i*0x40||offset==0x604+i*16)return 1;
    return offset==0x50C||offset==0x514;
}
static u32 load(u32 offset) { u32 value;memcpy(&value,buffer+offset,4);return canonical(value); }
static u16 half(u32 offset) { u16 value;memcpy(&value,buffer+offset,2);return value; }
static void store(u32 offset,u32 value) { value=native_value(value);memcpy(buffer+offset,&value,4); }
static void store_half(u32 offset,u16 value) { memcpy(buffer+offset,&value,2); }
static void event(u32 value) { CHECK(event_count<512);if (event_count<512)events[event_count++]=value; }
static int child(u32 address) { return address==0x20300||address==0x20340||address==0x20380||address==0x203C0; }
static int payload(u32 address) { return address==0x20400||address==0x20440||address==0x20480||address==0x204C0; }
static void mutate(u32 index)
{
    u32 m=fixture->mutation;
    if (calls[index]!=1)return;
    if (m==1&&index==9) { store(0x10,0x20240);store(0x24,0x20440);buffer[2]^=8;buffer[0x102]^=8; }
    if (m==2&&index==3) { store(0x604,0);buffer[2]^=0x80; }
    if (m==3&&index==5) { buffer[2]|=0x20;store_half(0x24,1);store_half(0x26,1); }
    if (m==4&&index==6) { store(0x28,0x203C0);store(0x68,0x203C0);store_half(0x24,1);store_half(0x26,1); }
    if (m==5&&index==7) { store_half(0x24,1);store_half(0x26,1); }
    if (m==6&&index==10)store(0x24,0x20440);
    if (m==7&&index==8)store(0x10,0x20240);
    if (m==8&&index==2) { buffer[2]^=0x10;buffer[0x102]^=0x10; }
}
static void observe(u32 index,u32 a,u32 b,u32 c,u32 d,u32 e)
{
    u32 values[5],i;
    a=canonical(a);b=canonical(b);c=canonical(c);d=canonical(d);e=canonical(e);
    if (index==0)CHECK(a==0x20600&&b==6);
    if (index==1)CHECK(a==0x40);
    if (index==2)CHECK((a==0x20000||a==0x20100)&&b==0x20600&&(c==2||c==6)&&(d==0||d==1||d==0xFEDCBA98)&&e==0x1234ABCD);
    if (index==3)CHECK(a==1&&b==0&&(c==0x20000||c==0x20100));
    if (index==4)CHECK(a==0x20000||a==0x20100);
    if (index==5)CHECK(a==0x20200||a==0x20240);
    if (index==6)CHECK(child(a-fixture->adjustment)&&b==0);
    if (index==7)CHECK((a==0||child(a))&&b==0x20000);
    if (index==8)CHECK((a==0x20000||a==0x20100)&&(b==0||b==3||b==0xFEDCBA98));
    if (index==9)CHECK((a==0x20200||a==0x20240)&&b==0xF00000E0&&c==0xF00000F0);
    if (index==10)CHECK(payload(a-fixture->adjustment)&&b==3);
    ++calls[index];event(index);
    values[0]=a;values[1]=b;values[2]=c;values[3]=d;values[4]=e;
    for (i=0;i<5;++i)event(values[i]);
    event(buffer[2]);event(load(0x20));event(load(0x10));event(load(0x28));event(half(0));
}

GeorgeResourceRecord *func_00225E80(const void *key,u32 kind)
{
    GeorgeResourceRecord *result=*(GeorgeResourceRecord *const *)((const u8 *)key+4);
    observe(0,(u32)key,kind,0,0,0);mutate(0);return result;
}
void *func_002AEE60(u32 size) { observe(1,size,0,0,0,0);mutate(1);return pointer(0x20100); }
void func_00226BC0(GeorgeResourceRecord *object,const void *key,u32 kind,u32 mode,u32 word)
{
    u32 offset=(u32)object-(u32)buffer;
    observe(2,(u32)object,(u32)key,kind,mode,word);
    store_half(offset,1);store(offset+0x10,0x20200);store(offset+0x1C,word);store(offset+0x20,0x20500);
    if (kind==2) { store_half(offset+0x24,3);store_half(offset+0x26,2); }
    else store(offset+0x24,0x20480);
    mutate(2);
}
void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *object) { observe(3,mode,word,(u32)object,0,0);mutate(3); }
void func_00226D78(GeorgeResourceRecord *object) { observe(4,(u32)object,0,0,0,0);mutate(4); }
void func_00227B98(void *holder) { observe(5,(u32)holder,0,0,0,0);mutate(5); }
static void native_child_release(void *object,u32 mode) { observe(6,(u32)object,mode,0,0,0);mutate(6); }
void func_00226ED8(GeorgeResourceRecord *object,GeorgeResourceRecord *owner) { observe(7,(u32)object,(u32)owner,0,0,0);mutate(7); }
void func_002267D0(GeorgeResourceRecord *object,u32 mode) { observe(8,(u32)object,mode,0,0,0);mutate(8); }
void *func_0022B360(void *holder,GeorgeMathVec4 *point,GeorgeRotationMatrix *frame)
{
    const u32 authored_point[]={0x80000000,0x40200000,0x40E00000,0x3F800000};
    const float authored_frame[]={1,2,3,4,5,6,7,8,9,10,11,12};
    CHECK((u32)point+16<=(u32)frame||(u32)frame+48<=(u32)point);
    observe(9,(u32)holder,0xF00000E0,0xF00000F0,0,0);
    memcpy(point,authored_point,sizeof authored_point);
    /* The original's live frame overlaps saved RA after its first 12 words.
     * This contract initializes 48 bytes, never the full 64-byte matrix. */
    memcpy(frame,authored_frame,sizeof authored_frame);
    mutate(9);return pointer(0x20400);
}
static void native_payload_release(void *object,u32 mode) { observe(10,(u32)object,mode,0,0,0);mutate(10); }

/* The entire real group translation unit is linked. Calls in its unexecuted
 * methods must fail if accidentally reached, rather than silently pass. */
GeorgeResourceRecord *func_0020FD48(const void *key,u32 mode,u32 word)
{ (void)key;(void)mode;(void)word;CHECK(0);return NULL; }
void func_00226E38(GeorgeResourceRecord *object,void (*callback)(u32,GeorgeResourceGroup *),GeorgeResourceGroup *context,GeorgeResourceGroup *owner)
{ (void)object;(void)callback;(void)context;(void)owner;CHECK(0); }
void func_00226F40(GeorgeResourceRecord *object) { (void)object;CHECK(0); }
void func_00227B68(void *holder) { (void)holder;CHECK(0); }

int main(void)
{
    u8 *allocation=(u8 *)malloc(0xB00+65535);u32 i;
    if (allocation==NULL)return 2;
    /* Preserve low16 pointer bits when payload+10 aliases the holder count;
     * only genuinely pointer-valued fields are translated on input. */
    buffer=(u8 *)(((u32)allocation+65535)&0xFFFF0000u);
    D_004682DC=pointer(0x204C0);
    CHECK(sizeof(void *)==4&&sizeof(GeorgeResourceBounds)==0x40&&sizeof(GeorgeResourceGroup)==0xA8);
    CHECK(offsetof(__typeof__(D_0043A4B0),destroy)==8&&sizeof(D_0043A4B0)==16);
    for (case_index=0;case_index<sizeof resource_bounds_golden/sizeof resource_bounds_golden[0];++case_index) {
        void *a,*b;u32 result=0;
        fixture=&resource_bounds_golden[case_index];
        for (i=0;i<704;++i) {
            u32 value=fixture->initial[i];
            if (pointer_field(i*4))value=native_value(value);
            memcpy(buffer+i*4,&value,4);
        }
        memset(calls,0,sizeof calls);event_count=0;
        a=(void *)native_value(fixture->args[0]);b=(void *)native_value(fixture->args[1]);
        switch (fixture->routine) {
        case 0:result=func_0020F858(a);break;
        case 1:result=canonical((u32)func_0020F860(a,fixture->args[1]));break;
        case 2:result=canonical((u32)func_0020F870(a,b,fixture->args[2],fixture->args[3]));break;
        case 3:result=func_0020F8D0(a,b,fixture->args[2]);break;
        case 4:func_0020F990(a);break;
        case 5:func_0020F9B0(a,fixture->args[1]);break;
        case 6:func_0020FA08(a,fixture->args[1]);break;
        case 7:result=canonical((u32)func_0020FA88(a,fixture->args[1],fixture->args[2]));break;
        case 8:result=canonical((u32)func_0020FC18(a));break;
        case 9:result=canonical((u32)func_0020FC20(a));break;
        case 10:result=canonical((u32)func_0020FC28(a));break;
        case 11:result=canonical((u32)func_0020FC30(a,b,fixture->args[2],fixture->args[3]));break;
        case 12:func_0020FCC0(a,fixture->args[1]);break;
        default:func_002B6F60(a,b,fixture->args[2]);break;
        }
        for (i=0;i<704;++i)CHECK(load(i*4)==fixture->expected[i]);
        for (i=0;i<11;++i)CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i)CHECK(events[i]==fixture->events[i]);
        if (fixture->routine<=3||fixture->routine==7||(fixture->routine>=8&&fixture->routine<=11))CHECK(result==fixture->result);
    }
    free(allocation);
    printf("resource_bounds: %u fixtures, %u checks, %u failures\n",case_index,checks,failures);
    return failures?1:0;
}
