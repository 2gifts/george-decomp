#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "george/buffer_ui.h"
#include "george/buffer_records.h"
#include "george/deimos_tables.h"
#include "george/string_algorithms.h"
#define func_002AAF50 ui_real_array_get
#include "george/array_records.h"
#undef func_002AAF50
#include "buffer_ui_golden.h"

#define BUFFER 0x20000U
#define END (BUFFER+UI_WORDS*4U)
#define UI 0x20040U
#define ALT_UI 0x20440U
#define ALT_MANAGER 0x20940U
static u32 arena[UI_WORDS] __attribute__((aligned(256)));
static u32 expected[UI_WORDS],events[UI_EVENTS];
static unsigned checks,current,event_count,callback_count,renderer_count;
static u32 mutation,completion,parse;
GeorgeBufferUi *D_003F9408;
u8 *D_004683AC;
GeorgeGenericMap *D_00481760;
extern const signed char D_0043A750[],D_0043A788[],D_0043A798[];

static void check(int ok,const char *message,u32 index)
{++checks;if(!ok){fprintf(stderr,"buffer UI fixture %u: %s %u\n",current,message,index);exit(1);}}
static void *physical(u32 value)
{check(value>=BUFFER && value<END,"typed authored pointer",value);return (u8 *)arena+value-BUFFER;}
static u32 canonical(u32 value)
{
    if(value>=(u32)arena && value-(u32)arena<sizeof(arena))return BUFFER+value-(u32)arena;
    return value;
}
static u32 crc(const void *data,u32 length)
{
    const u8 *p=data;u32 result=0xFFFFFFFFU,i,j;
    for(i=0;i<length;++i){result^=p[i];for(j=0;j<8;++j)result=(result>>1)^(0xEDB88320U & (0U-(result&1U)));}
    return ~result;
}
static u32 text_crc(const signed char *text)
{return crc(text,(u32)strlen((const char *)text));}
static void event(u32 size,const u32 *items)
{
    u32 i;check(event_count+size+1<=UI_EVENTS,"event extent",event_count);
    events[event_count++]=size;for(i=0;i<size;++i)events[event_count++]=items[i];
}
static u32 bits(float value)
{union{float f;u32 u;}v;v.f=value;return v.u;}
static void single_event(u32 target,u32 value)
{u32 e[]={target,value};event(2,e);}
static void empty_event(u32 target)
{event(1,&target);}

/* Opaque supporting callbacks: authored mutation controls, no recovery award. */
void func_002162D0(u32 key,GeorgeDeimosValue *value,void *context)
{
    u32 e[]={0x2162D0,key,canonical((u32)value),canonical((u32)context)};
    ++callback_count;event(4,e);
    D_003F9408->field0C=(s32)((u32)D_003F9408->field0C+completion);
    strcpy((char *)D_003F9408+0x10,"alpha");D_003F9408->field210=3;
    if(callback_count==1){
        if(mutation==1)D_003F9408=physical(ALT_UI);
        else if(mutation==2)*(void **)physical(0x23508)=physical(0x23540);
        else if(mutation==3){
            u32 p=canonical((u32)value),node=p>=0x23808U && p<=0x23888U?p-8U:0x23820U;
            *(void **)physical(node)=physical(0x23880);
        }
        else if(mutation==4)*(void **)physical(0x23300)=physical(0x23340);
    }
}
s32 func_002CEB78(const signed char *source,u32 length,const signed char *name)
{
    u32 e[]={0x2CEB78,(u32)strlen((const char *)source),text_crc(source),length,0x43A720};
    extern const signed char D_0043A720[];
    check(name==D_0043A720,"parser named binding",0);event(5,e);
    if(mutation==5)D_003F9408=physical(ALT_UI);
    return (s32)parse;
}
void func_002D0258(u32 key,s32 first,s32 second)
{u32 e[]={0x2D0258,key,(u32)first,(u32)second};event(4,e);}
void func_002D04B0(const signed char *name,s32 value)
{
    extern const signed char D_0043A720[];u32 e[]={0x2D04B0,0x43A720,(u32)value};
    check(name==D_0043A720,"parser result binding",0);event(3,e);
}
void func_0023C908(float x,float y,float r,float g,float b,float a,u32 kind,const signed char *format,...)
{
    u32 e[13]={0x23C908,bits(x),bits(y),bits(r),bits(g),bits(b),bits(a),kind,0};
    u32 size;va_list args;va_start(args,format);
    if(format==D_0043A750){
        unsigned long long first=(unsigned long long)va_arg(args,signed long long);
        unsigned long long second=(unsigned long long)va_arg(args,signed long long);
        e[8]=0x43A750;e[9]=(u32)first;e[10]=(u32)(first>>32);e[11]=(u32)second;e[12]=(u32)(second>>32);size=13;
    }else if(format==D_0043A788){
        unsigned long long value=va_arg(args,unsigned long long);
        e[8]=0x43A788;e[9]=(u32)value;e[10]=(u32)(value>>32);size=11;
    }else if(format==D_0043A798){
        signed char *text=va_arg(args,signed char *);
        e[8]=0x43A798;e[9]=canonical((u32)text);e[10]=text_crc(text);size=11;
    }else {e[8]=canonical((u32)format);e[9]=text_crc(format);size=10;}
    va_end(args);event(size,e);++renderer_count;
    if(mutation==6 && renderer_count==1)D_003F9408=physical(ALT_UI);
    if(mutation==7 && renderer_count==2)*(void **)physical(ALT_MANAGER+0x0C)=physical(0x20C40);
}
void func_002BA680(void){empty_event(0x2BA680);}
void func_002BA708(void){empty_event(0x2BA708);}
void func_00290BF0(u32 value){single_event(0x290BF0,value);}
void func_00290CF8(u32 first,u32 second){u32 e[]={0x290CF8,first,second};event(3,e);}
void func_0028E8A0(u32 value){single_event(0x28E8A0,value);}
void func_0028E668(u32 value){single_event(0x28E668,value);}
void func_0028FE88(float r,float g,float b,float a){u32 e[]={0x28FE88,bits(r),bits(g),bits(b),bits(a)};event(5,e);}
void func_0028FBC0(u32 kind){single_event(0x28FBC0,kind);}
void func_0028FD30(float x,float y,float z){u32 e[]={0x28FD30,bits(x),bits(y),bits(z)};event(4,e);}
void func_0028C590(void){empty_event(0x28C590);}
void func_0023E0D0(u32 value){single_event(0x23E0D0,value);}

/* This bridge returns actual GNU soft-double result bits under the native ABI.
 * The unchanged conversion/unpack/make/pack graph is compiled separately. */
extern double ui_real_fptodp(float value);
unsigned long long func_00374848(float value)
{union{double d;unsigned long long bits;}v;v.d=ui_real_fptodp(value);return v.bits;}

/* Native-only compatible interfaces around unchanged licensed generic string
 * sources. They establish byte-result equivalence, not EE bulk-read identity. */
extern void *ui_generic_memset(void *,int,u32);
extern char *ui_generic_strcpy(char *,const char *);
extern char *ui_generic_strcat(char *,const char *);
extern char *ui_generic_strrchr(const char *,int);
extern char *ui_generic_strpbrk(const char *,const char *);
void *func_003936A0(void *d,s32 v,u32 n){return ui_generic_memset(d,v,n);}
signed char *func_00393B74(signed char *d,const signed char *s){return (signed char *)ui_generic_strcpy((char *)d,(const char *)s);}
signed char *func_00393758(signed char *d,const signed char *s){return (signed char *)ui_generic_strcat((char *)d,(const char *)s);}
signed char *func_003985D8(const signed char *s,s32 c){return (signed char *)ui_generic_strrchr((const char *)s,c);}
signed char *func_00398558(const signed char *s,const signed char *set){return (signed char *)ui_generic_strpbrk((const char *)s,(const char *)set);}
extern char *ui_generic_strncpy(char *,const char *,u32);
char *func_00394010(char *d,const char *s,u32 n){return ui_generic_strncpy(d,s,n);}
extern char *ui_generic_strstr(const char *,const char *);
char *func_00398628(const char *s,const char *p){return ui_generic_strstr(s,p);}

/* The exact recovered getter is native-renamed with its header declaration.
 * This typed adapter agrees with the frozen manager's u32* interface. Empty
 * command arrays do not exercise it in current UI fixtures; no helper award. */
u32 *func_002AAF50(void *array,s32 index)
{
    return (u32 *)ui_real_array_get((const GeorgeArrayRecords *)array,index);
}
static int pointer_cell(const struct UiGolden *g,u32 index)
{u32 i;for(i=0;i<g->pointer_count;++i)if(g->pointers[i]==index)return 1;return 0;}
static int text_word(const struct UiGolden *g,u32 index)
{u32 i;for(i=0;i<UI_HASHES;++i)if(g->hashes[i].address<=BUFFER+4*index && BUFFER+4*index<g->hashes[i].address+g->hashes[i].size)return 1;return 0;}
int main(void)
{
    unsigned n,i;check(sizeof(void *)==4,"native32 pointers",0);
    check(((u32)arena&255U)==0,"low pointer alias alignment",0);
    check(sizeof(float)==4 && sizeof(double)==8,"native scalar widths",0);
    for(n=0;n<sizeof(ui_golden)/sizeof(ui_golden[0]);++n){
        const struct UiGolden *g=ui_golden+n;union{u32 u;float f;}delta;
        current=n;mutation=g->mutation;completion=g->completion;parse=g->parse;
        event_count=callback_count=renderer_count=0;
        for(i=0;i<UI_WORDS;++i)arena[i]=expected[i]=0xA5030201U^(i*0x10203U);
        for(i=0;i<g->initial_count;++i){check(g->initial[i].index<UI_WORDS,"initial word",i);arena[g->initial[i].index]=expected[g->initial[i].index]=g->initial[i].value;}
        for(i=0;i<g->change_count;++i){check(g->changes[i].index<UI_WORDS,"change word",i);expected[g->changes[i].index]=g->changes[i].value;}
        for(i=0;i<g->pointer_count;++i){u32 p=g->pointers[i],v=arena[p];check(p<UI_WORDS,"typed pointer cell",i);if(v>=BUFFER && v<END)arena[p]=(u32)physical(v);else check(v==0,"null pointer cell",i);}
        D_003F9408=physical(g->globals[0]);D_004683AC=physical(g->globals[1]);D_00481760=physical(g->globals[2]);
        delta.u=g->delta_bits;
        if(g->routine==0)func_002163E8();else if(g->routine==1)func_002167D8(delta.f);else{check(g->routine==2,"selected routine",0);func_002169C0();}
        check(canonical((u32)D_003F9408)==g->expected_globals[0],"fresh UI global",0);
        check(canonical((u32)D_004683AC)==g->expected_globals[1],"keyboard global",0);
        check(canonical((u32)D_00481760)==g->expected_globals[2],"map global",0);
        for(i=0;i<UI_WORDS;++i)if(!text_word(g,i))check((pointer_cell(g,i)?canonical(arena[i]):arena[i])==expected[i],"whole typed arena",i);
        for(i=0;i<UI_HASHES;++i)check(crc(physical(g->hashes[i].address),g->hashes[i].size)==g->hashes[i].crc,"character memory hash",i);
        check(event_count==g->event_count,"controlled event count",event_count);
        for(i=0;i<event_count;++i)check(events[i]==g->events[i],"controlled event value",i);
    }
    printf("%u buffer UI checks passed\n",checks);return 0;
}
