/* Authored observations of real selected close/copy and published stdio C.
 * Host callbacks model only write/close results and the documented mutations.
 * The FILE seek callback is never called, so no host fpos_t-width claim follows.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "george/stdio_close.h"
#include "stdio_close_golden.h"

#define BASE 0x20000u
#define CTX0 0x21000u
#define CTX1 0x21400u
#define ALT 0x20700u
static u32 arena[STDIO_WORDS], expected[STDIO_WORDS];
static const struct StdioGolden *current;
static FILE *selected;
static u32 events[140], event_count, checks;
struct _reent *D_00405694;
extern int func_003941D8(FILE *);
extern size_t stdio_real_fread(void *,size_t,size_t,FILE *);
extern void stdio_cleanup_r(struct _reent *);

static void fail(const char *message,u32 a,u32 b)
{
    printf("stdio check %u: %s: %08x != %08x\n",checks,message,a,b);
    exit(1);
}
static void equal(const char *message,u32 a,u32 b)
{
    ++checks;
    if(a!=b)fail(message,a,b);
}
static void *pointer(u32 address)
{
    if(!address)return NULL;
    if(address<BASE || address>=BASE+sizeof(arena))fail("unowned guest pointer",address,BASE);
    return (unsigned char *)arena+(address-BASE);
}
static u32 address(const void *p)
{
    if(!p)return 0;
    if((const unsigned char *)p<(const unsigned char *)arena ||
       (const unsigned char *)p>=(const unsigned char *)(arena+STDIO_WORDS))
        fail("unowned host pointer",(u32)p,(u32)arena);
    return BASE+(u32)((const unsigned char *)p-(const unsigned char *)arena);
}
static int writer(void *,const char *,int);
static int closer(void *);
static int alternate(void *);
int __sclose(void *);
int __sread(void *cookie,char *buffer,int count)
{
    (void)cookie;(void)buffer;(void)count;
    fail("unexecuted read hook",1,0);return -1;
}
int __swrite(void *cookie,const char *buffer,int count)
{
    (void)cookie;(void)buffer;(void)count;
    fail("unexecuted standard write hook",1,0);return -1;
}
fpos_t __sseek(void *cookie,fpos_t offset,int whence)
{
    (void)cookie;(void)offset;(void)whence;
    fail("unexecuted seek hook",1,0);return -1;
}
static void record(u32 kind,void *cookie,const char *buffer,int count)
{
    if(event_count+7>140)fail("event bound",event_count,140);
    events[event_count++]=kind;
    events[event_count++]=address(cookie);
    events[event_count++]=address(buffer);
    events[event_count++]=(u32)count;
    events[event_count++]=(unsigned short)selected->_flags;
    /* The close function is translated through its explicitly typed field. */
    events[event_count++]=selected->_close==closer?0x60000004u:
        selected->_close==alternate?0x60000008u:selected->_close==__sclose?0x395538u:0;
    events[event_count++]=address(selected->_data);
}
static int writer(void *cookie,const char *buffer,int count)
{
    record(0,cookie,buffer,count);
    if(current->mutation&1){selected->_close=alternate;selected->_cookie=pointer(BASE+0x60);}
    if(current->mutation&2)selected->_close=NULL;
    if(current->mutation&4){selected->_data=pointer(CTX1);selected->_flags|=0x40;}
    if(current->mutation&32)D_00405694=pointer(CTX1);
    return (s32)current->writeret;
}
static int close_value(u32 kind,void *cookie)
{
    record(kind,cookie,NULL,0);
    if(current->mutation&8){
        selected->_bf._base=pointer(ALT);selected->_ub._base=pointer(ALT);selected->_lb._base=pointer(ALT);
        selected->_flags=0x7FFF;selected->_data=pointer(CTX1);
    }
    if(current->mutation&16)D_00405694=pointer(CTX1);
    return (s32)current->closeret;
}
static int closer(void *cookie){return close_value(1,cookie);}
static int alternate(void *cookie){return close_value(3,cookie);}
int __sclose(void *cookie){return close_value(2,cookie);}

/* Authentic memcpy interface for unchanged fread only. Both original fread
 * call sites discard this result. The selected copy has a void convention;
 * NULL here does not assert any retail memcpy return value.
 */
void *stdio_observe_fread_copy(void *out,const void *in,size_t size)
{
    func_003947D8(out,in,(u32)size);return NULL;
}
int __srefill(FILE *f)
{
    (void)f;fail("unexecuted refill path",1,0);return -1;
}
void *_malloc_r(struct _reent *r,size_t size)
{
    (void)r;(void)size;fail("unexecuted allocator path",1,0);return NULL;
}
void _free_r(struct _reent *r,void *p)
{
    (void)r;(void)p;fail("unexpected restored cleanup",1,0);
}

static u32 host_word(u32 value)
{
    switch(value){
    case 0x60000000u:return (u32)writer;
    case 0x60000004u:return (u32)closer;
    case 0x60000008u:return (u32)alternate;
    case 0x3953E8u:return (u32)__sread;
    case 0x395450u:return (u32)__swrite;
    case 0x3954D0u:return (u32)__sseek;
    case 0x395538u:return (u32)__sclose;
    case 0x394748u:return (u32)stdio_cleanup_r;
    default:return (u32)pointer(value);
    }
}
static u32 guest_word(u32 value)
{
    if(value==(u32)writer)return 0x60000000u;
    if(value==(u32)closer)return 0x60000004u;
    if(value==(u32)alternate)return 0x60000008u;
    if(value==(u32)__sread)return 0x3953E8u;
    if(value==(u32)__swrite)return 0x395450u;
    if(value==(u32)__sseek)return 0x3954D0u;
    if(value==(u32)__sclose)return 0x395538u;
    if(value==(u32)stdio_cleanup_r)return 0x394748u;
    return address((void *)value);
}
static int typed_pointer(u32 index)
{
    u32 i;
    for(i=0;i<sizeof(stdio_pointer_cells)/sizeof(*stdio_pointer_cells);++i)
        if(stdio_pointer_cells[i]==BASE+index*4)return 1;
    return 0;
}
int main(void)
{
    u32 n,i,j,result;
    equal("pointer width",sizeof(void *),4);
    equal("native int width",sizeof(int),4);
    equal("native long width",sizeof(long),4);
    equal("native fpos width",sizeof(fpos_t),4); /* unused seek ABI differs from target8. */
    equal("native context size",sizeof(struct _reent),748);
    equal("FILE size",sizeof(FILE),88);
    equal("FILE flags",offsetof(FILE,_flags),12);
    equal("FILE cookie",offsetof(FILE,_cookie),28);
    equal("FILE close",offsetof(FILE,_close),44);
    equal("FILE data",offsetof(FILE,_data),84);
    equal("context init",offsetof(struct _reent,__sdidinit),56);
    equal("context cleanup",offsetof(struct _reent,__cleanup),60);
    equal("context glue",offsetof(struct _reent,__sglue),472);
    equal("context embedded FILE",offsetof(struct _reent,__sf),484);
    for(n=0;n<sizeof(stdio_golden)/sizeof(*stdio_golden);++n){
        current=&stdio_golden[n];event_count=0;
        for(i=0;i<STDIO_WORDS;++i)arena[i]=expected[i]=0;
        for(j=0;j<current->initial_count;++j)arena[current->initial[j].index]=current->initial[j].value;
        for(j=0;j<current->expected_count;++j)expected[current->expected[j].index]=current->expected[j].value;
        for(i=0;i<STDIO_WORDS;++i)if(typed_pointer(i))arena[i]=host_word(arena[i]);
        D_00405694=pointer(CTX0);selected=pointer(current->selected_file);
        if(current->routine==0)result=(u32)func_003941D8(selected);
        else if(current->routine==1){func_003947D8(pointer(current->destination),pointer(current->source),current->count);result=0;}
        else result=(u32)stdio_real_fread(pointer(current->destination),1,current->count,selected);
        equal("result",result,current->result);
        equal("context global",address(D_00405694),current->impure);
        equal("event count",event_count,current->event_count);
        for(j=0;j<event_count;++j)equal("event",events[j],current->events[j]);
        for(i=0;i<STDIO_WORDS;++i)equal("arena",typed_pointer(i)?guest_word(arena[i]):arena[i],expected[i]);
    }
    printf("%u stdio_close checks passed\n",checks);
    return 0;
}
