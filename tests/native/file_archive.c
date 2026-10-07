/* Separate production TUs execute resolver/CRC/map/close. These hooks describe
 * observable external calls, not heap, cache, SDK-close or kernel behavior. */
#include "george/file_archive.h"
#include "george/string_registry.h"
#include "george/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "file_archive_golden.h"
#define BASE 0x20000u
#define SLOT 0x20000u
#define INPUT 0x20101u
#define CONTEXT 0x20200u
#define HEAP 0x20300u
#define TABLE 0x20400u
#define NEW_TABLE 0x20700u
#define MAP 0x20800u
#define ALT_MAP 0x20820u
#define BUCKETS 0x20900u
#define ALT_BUCKETS 0x20940u
#define NODE 0x20A00u
#define ALT_NODE 0x20A40u
static u32 arena[FILE_ARCHIVE_WORDS],expected[FILE_ARCHIVE_WORDS],events[16][8],event_count,checks;
static const struct ArchiveGolden *current;
u32 D_003FD23C,D_003FD240,D_003FD1D8,D_003FD1DC;
GeorgeStringRegistryPair *D_003FD1E0;
GeorgeHeap *D_003FD204;
char D_00469A00[128],D_00469A80[128],D_00469B00[128];
const char D_00447238[]="authored allocation diagnostic observer";
extern void *archive_real_memcpy(void *,const void *,size_t);
extern char *archive_real_strcpy(char *,const char *);
extern char *archive_real_strcat(char *,const char *);
extern int archive_real_strcmp(const char *,const char *);
extern char *archive_real_strlwr(char *);
extern char *archive_real_strstr(const char *,const char *);
static void equal(u32 a,u32 b,const char *label)
{
    ++checks;if(a!=b){printf("archive fixture %u check %u %s: %08X != %08X\n",(u32)(current-archive_golden),checks,label,a,b);exit(1);}
}
static void *host(u32 v)
{
    if(!v)return 0;
    if(v<BASE || v-BASE>=sizeof(arena)){printf("unowned archive native pointer\n");exit(1);}
    return (u8 *)arena+v-BASE;
}
static u32 guest(const void *p){return p?BASE+((u32)p-(u32)arena):0;}
static u32 value(u32 v){return v>=BASE && v-BASE<sizeof(arena)?(u32)host(v):v;}
static void event(u32 kind,u32 a,u32 b,u32 c)
{
    GeorgeFileSlot *slot=host(SLOT);u32 *row;
    if(event_count>=16){printf("archive native event bound\n");exit(1);}
    row=events[event_count++];row[0]=kind;row[1]=a;row[2]=b;row[3]=c;
    row[4]=D_003FD23C;row[5]=D_003FD240;row[6]=slot->field04;row[7]=slot->field10;
}
s32 func_00363AE0(s32 operation)
{
    GeorgeFileSlot *slot=host(SLOT);equal((u32)operation,0,"cache argument");event(1,0,0,0);
    if(current->mutation&1){D_003FD23C=0x12345678u;D_003FD240=0x80000000u;slot->field10=0xBEEFF00Du;}
    return (s32)0x80000001u;
}
s32 func_00368C40(s32 descriptor)
{
    GeorgeFileSlot *slot=host(SLOT);event(2,(u32)descriptor,0,0);
    if(current->mutation&2){D_003FD23C=0xFFFFFFFFu;slot->field04=0xAAAAAAAAu;slot->field10=0x87654321u;}
    return (s32)0x80000001u;
}
void *func_002ADF60(GeorgeHeap *heap,u32 bytes,u32 alignment)
{
    equal(guest(heap),HEAP,"allocation heap");equal(bytes,80,"allocation bytes");equal(alignment,3,"allocation alignment");event(3,HEAP,bytes,alignment);
    if(current->mutation&4)((GeorgeArchiveMapView *)host(CONTEXT))->field50=host(ALT_MAP);
    return current->failure?0:host(NEW_TABLE);
}
s32 func_00394F68(const char *format,...)
{
    va_list args;u32 cursor,bytes,alignment;
    equal(format==D_00447238,1,"authored diagnostic pointer");
    va_start(args,format);cursor=va_arg(args,u32);bytes=va_arg(args,u32);alignment=va_arg(args,u32);va_end(args);
    event(4,cursor?guest((void *)cursor):0,bytes,alignment);return (s32)0x80000001u;
}
void *func_003934F8(void *d,const void *s,u32 n){return archive_real_memcpy(d,s,n);}
char *func_00393B74(char *d,const char *s){return archive_real_strcpy(d,s);}
char *func_00393758(char *d,const char *s){return archive_real_strcat(d,s);}
s32 func_00393A28(const char *a,const char *b){return archive_real_strcmp(a,b);}
char *func_003984D8(char *s){return archive_real_strlwr(s);}
char *func_00398628(const char *s,const char *n){return archive_real_strstr(s,n);}
static int pointer_cell(u32 index)
{
    u32 a=BASE+index*4,t;
    static const u32 maps[]={MAP,ALT_MAP},buckets[]={BUCKETS,ALT_BUCKETS},nodes[]={NODE,ALT_NODE};
    if(a==CONTEXT+0x50 || a==TABLE || a==TABLE+4 || a==HEAP+0x18)return 1;
    for(t=0;t<2;++t)if(a==maps[t]+12 || (a>=buckets[t] && a<buckets[t]+32) || a==nodes[t] || a==nodes[t]+8 || a==nodes[t]+16 || a==nodes[t]+24)return 1;
    return 0;
}
int main(void)
{
    u32 c,i,j,result;char *texts[]={D_00469A00,D_00469A80,D_00469B00};
    for(c=0;c<sizeof(archive_golden)/sizeof(archive_golden[0]);++c){
        current=&archive_golden[c];event_count=0;
        for(i=0;i<FILE_ARCHIVE_WORDS;++i)expected[i]=0xAD4B0301u^(i*0x10203u);
        for(i=0;i<current->initial_count;++i)expected[current->initial[i].index]=current->initial[i].value;
        for(i=0;i<FILE_ARCHIVE_WORDS;++i)arena[i]=pointer_cell(i)?value(expected[i]):expected[i];
        for(i=0;i<3;++i)for(j=0;j<128;++j)texts[i][j]=(char)current->texts_initial[i][j];
        D_003FD23C=current->globals_initial[0];D_003FD240=current->globals_initial[1];D_003FD1D8=current->globals_initial[2];D_003FD1DC=current->globals_initial[3];
        D_003FD1E0=host(current->globals_initial[4]);D_003FD204=host(current->globals_initial[5]);
        if(current->routine)result=guest(func_002B1BA8(host(CONTEXT),host(INPUT)));
        else{func_002B0998(host(SLOT));result=0;}
        equal(result,current->result,"result");
        for(i=0;i<current->change_count;++i)expected[current->changes[i].index]=current->changes[i].value;
        for(i=0;i<FILE_ARCHIVE_WORDS;++i){u32 a=arena[i];if(pointer_cell(i) && a>=(u32)arena && a-(u32)arena<sizeof(arena))a=guest((void *)a);equal(a,expected[i],"whole arena");}
        {u32 g[]={D_003FD23C,D_003FD240,D_003FD1D8,D_003FD1DC,guest(D_003FD1E0),guest(D_003FD204)};for(i=0;i<6;++i)equal(g[i],current->globals_expected[i],"global");}
        for(i=0;i<3;++i)for(j=0;j<128;++j)equal((u8)texts[i][j],current->texts_expected[i][j],"text global");
        equal(event_count,current->event_count,"event count");
        for(i=0;i<event_count;++i)for(j=0;j<8;++j)equal(events[i][j],current->events[i][j],"event");
    }
    printf("file_archive: %u checks passed\n",checks);return 0;
}
