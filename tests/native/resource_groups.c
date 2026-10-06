/* Authored engine/call substitutes. The five connected helpers execute C. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_groups.h"
#include "resource_groups_golden.h"

static void native_group_destroy(void *,u32);
static void native_child_release(void *,u32);
/* Original table geometry only; no original table bytes are copied. The two
 * header words are unobserved, and the observed destroy pair has adjustment 0. */
const struct {
    u32 unobserved_header[2];
    GeorgeGoalVirtualWord destroy;
} D_0043A458={{0,0},{0,0,native_group_destroy}};
void *D_004682DC;

static u8 *buffer;
static const struct ResourceGroupGolden *fixture;
static u32 calls[13],events[512],event_count;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<20)printf("line %u routine %u mutation %u\n",__LINE__,fixture?fixture->routine:0,fixture?fixture->mutation:0); \
    ++failures; } } while (0)
static void *pointer(u32 value) { return buffer+value-0x20000; }
static u32 native_value(u32 value)
{
    if (value>=0x20000&&value<0x20B00)return (u32)pointer(value);
    if (value==0xF00000C0)return (u32)native_child_release;
    if (value==0x43A458)return (u32)&D_0043A458;
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)buffer&&value<(u32)buffer+0xB00)return 0x20000+value-(u32)buffer;
    if (value==(u32)native_child_release)return 0xF00000C0;
    if (value==(u32)&D_0043A458)return 0x43A458;
    if (value==(u32)func_0020F440)return 0x20F440;
    return value;
}
static int pointer_field(u32 offset)
{
    const u32 holders[]={0x200,0x240},children[]={0x300,0x340,0x380,0x3C0};
    const u32 primary[]={0x700,0x800},secondary[]={0x900,0x940};u32 i,j;
    if (offset==0x10||offset==0x20||(offset>=0x28&&offset<0xA8)||offset==0x514)return 1;
    for (i=0;i<2;++i) {
        if (offset==holders[i]+8||offset==holders[i]+0x20||offset==holders[i]+0x24)return 1;
        for (j=0;j<4;++j)if (offset==primary[i]+j*0x24||offset==secondary[i]+j*8+4)return 1;
    }
    for (i=0;i<4;++i)if (offset==children[i]+0x10||offset==children[i]+0x20||offset==0x600+i*16||offset==0x604+i*16)return 1;
    return 0;
}
static u32 load(u32 offset) { u32 value;memcpy(&value,buffer+offset,4);return canonical(value); }
static u16 half(u32 offset) { u16 value;memcpy(&value,buffer+offset,2);return value; }
static void store(u32 offset,u32 value) { value=native_value(value);memcpy(buffer+offset,&value,4); }
static void store_half(u32 offset,u16 value) { memcpy(buffer+offset,&value,2); }
static void event(u32 value) { CHECK(event_count<512);if (event_count<512)events[event_count++]=value; }
static int child_address(u32 address) { return address==0x20300||address==0x20340||address==0x20380||address==0x203C0; }
static void mutate(u32 index)
{
    u32 m=fixture->mutation;
    if (calls[index]!=1)return;
    if (m==1&&index==0) { store(0x10,0x20240);store(0x1C,0x13579BDF);store_half(0x24,1);store_half(0x26,1); }
    if (m==2&&index==1) { store(0x28,0x203C0);buffer[2]|=0x20;store_half(0x24,1);store_half(0x26,1); }
    if (m==3&&index==2) { store(0x10,0x20240);store_half(0x24,1);store_half(0x26,1); }
    if (m==4&&index==4) { store(0x10,0x20240);store_half(0x26,0);buffer[2]^=4; }
    if (m==5&&index==5) { buffer[2]|=0x20;store_half(0x24,1);store_half(0x26,1); }
    if (m==6&&index==6) { buffer[2]|=0x20;store(0x28,0x203C0);store(0x68,0x203C0);store_half(0x24,1);store_half(0x26,1); }
    if (m==7&&index==7) { store_half(0x24,1);store_half(0x26,1);store(0x10,0x20240); }
    if (m==8&&index==11) { store(0x604,0);buffer[2]^=0x80; }
}
static void observe(u32 index,u32 a,u32 b,u32 c,u32 d,u32 e)
{
    u32 values[5],i;
    a=canonical(a);b=canonical(b);c=canonical(c);d=canonical(d);e=canonical(e);
    if (index==0)CHECK((a==0x20600||a==0x20610||a==0x20620||a==0x20630)&&b==0&&(c==0x1234ABCD||c==0x13579BDF));
    if (index==1)CHECK(child_address(a)&&b==0x20F440&&c==0x20000&&d==0x20000);
    if (index==2)CHECK(child_address(a)||a==0x20000||a==0x20100);
    if (index==3||index==5)CHECK(a==0x20200||a==0x20240);
    if (index==4)CHECK(a==0x20000);
    if (index==6)CHECK(child_address(a-fixture->adjustment)&&b==0);
    if (index==7)CHECK((a==0||child_address(a))&&b==0x20000);
    if (index==8)CHECK(a==0x20600&&b==2);
    if (index==9)CHECK(a==0xE8);
    if (index==10)CHECK(a==0x20100&&b==0x20600&&c==2&&(d==0||d==1||d==0xFEDCBA98)&&e==0x1234ABCD);
    if (index==11)CHECK(a==1&&b==0&&(c==0x20000||c==0x20100));
    if (index==12)CHECK(a==0x20100&&b==3);
    ++calls[index];event(index);
    values[0]=a;values[1]=b;values[2]=c;values[3]=d;values[4]=e;
    for (i=0;i<5;++i)event(values[i]);
    event(buffer[2]);event(half(0x24));event(half(0x26));event(load(0x10));event(load(0x28));
    mutate(index);
}

GeorgeResourceRecord *func_0020FD48(const void *key,u32 mode,u32 word)
{
    GeorgeResourceRecord *result=*(GeorgeResourceRecord *const *)key;
    observe(0,(u32)key,mode,word,0,0);return result;
}
void func_00226E38(GeorgeResourceRecord *child,void (*callback)(u32,GeorgeResourceGroup *),GeorgeResourceGroup *context,GeorgeResourceGroup *owner)
{ observe(1,(u32)child,(u32)callback,(u32)context,(u32)owner,0); }
void func_00226D78(GeorgeResourceRecord *child) { observe(2,(u32)child,0,0,0,0); }
void func_00227B68(void *holder) { observe(3,(u32)holder,0,0,0,0); }
void func_00226F40(GeorgeResourceRecord *group) { observe(4,(u32)group,0,0,0,0); }
void func_00227B98(void *holder) { observe(5,(u32)holder,0,0,0,0); }
static void native_child_release(void *child,u32 mode) { observe(6,(u32)child,mode,0,0,0); }
void func_00226ED8(GeorgeResourceRecord *child,GeorgeResourceRecord *group) { observe(7,(u32)child,(u32)group,0,0,0); }
GeorgeResourceRecord *func_00225E80(const void *key,u32 kind)
{
    GeorgeResourceRecord *result=*(GeorgeResourceRecord *const *)((const u8 *)key+4);
    observe(8,(u32)key,kind,0,0,0);return result;
}
void *func_002AEE60(u32 size) { observe(9,size,0,0,0,0);return pointer(0x20100); }
void func_00226BC0(GeorgeResourceRecord *group,const void *key,u32 kind,u32 mode,u32 word)
{
    /* Event publication precedes authored constructor writes and mutations,
     * exactly as in the trace's controlled constructor contract. */
    u32 values[5]={canonical((u32)group),canonical((u32)key),kind,mode,word},i;
    CHECK(values[0]==0x20100&&values[1]==0x20600&&kind==2&&(mode==0||mode==1||mode==0xFEDCBA98)&&word==0x1234ABCD);
    ++calls[10];event(10);for (i=0;i<5;++i)event(values[i]);
    event(buffer[2]);event(half(0x24));event(half(0x26));event(load(0x10));event(load(0x28));
    store_half(0x100,1);store(0x110,0x20200);store(0x11C,word);store(0x120,0x20500);store_half(0x124,3);store_half(0x126,2);
    mutate(10);
}
void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *group) { observe(11,mode,word,(u32)group,0,0); }
static void native_group_destroy(void *group,u32 mode) { observe(12,(u32)group,mode,0,0,0); }

int main(void)
{
    u8 *allocation=(u8 *)malloc(0xB00+65535);u32 n,i;
    if (allocation==NULL)return 2;
    /* Preserve the original low16 pointer bits when an output aliases the
     * holder's halfword count. This is synthetic test storage, not a game heap. */
    buffer=(u8 *)(((u32)allocation+65535)&0xFFFF0000u);
    D_004682DC=pointer(0x204C0);
    CHECK(sizeof(void *)==4&&sizeof(GeorgeResourceGroup)==0xA8);
    CHECK(offsetof(__typeof__(D_0043A458),destroy)==8&&sizeof(D_0043A458)==16);
    for (n=0;n<sizeof resource_group_golden/sizeof resource_group_golden[0];++n) {
        void *a,*b,*c;u32 result=0;
        fixture=&resource_group_golden[n];
        /* Packed count24=2/count26=2 is numerically 0x00020002, inside the
         * synthetic pointer window. Translate only proven pointer fields. */
        for (i=0;i<704;++i) {
            u32 value=fixture->initial[i];
            if (pointer_field(i*4))value=native_value(value);
            memcpy(buffer+i*4,&value,4);
        }
        memset(calls,0,sizeof calls);event_count=0;
        a=(void *)native_value(fixture->args[0]);b=(void *)native_value(fixture->args[1]);c=(void *)native_value(fixture->args[2]);
        switch (fixture->routine) {
        case 0:func_0020F128((GeorgeResourceGroup *)a,fixture->args[1]);break;
        case 1:func_0020F340((GeorgeResourceGroup *)a);break;
        case 2:func_0020F440(fixture->args[0],(GeorgeResourceGroup *)b);break;
        case 3:func_0020F5D8((GeorgeResourceGroup *)a);break;
        case 4:result=canonical((u32)func_0020F6F8(a,fixture->args[1],fixture->args[2]));break;
        case 5:func_002B6DC0(a);break;
        case 6:result=func_002B7088(a,fixture->args[1],(void **)c);break;
        case 7:result=func_002B70E0(a,(void **)b);break;
        case 8:func_002B6F28(a,fixture->args[1],c);break;
        default:func_002B6F48(a,fixture->args[1],c);break;
        }
        for (i=0;i<704;++i)CHECK(load(i*4)==fixture->expected[i]);
        for (i=0;i<13;++i)CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i)CHECK(events[i]==fixture->events[i]);
        if (fixture->routine==4||fixture->routine==6||fixture->routine==7)CHECK(result==fixture->result);
    }
    free(allocation);
    printf("resource_groups: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
