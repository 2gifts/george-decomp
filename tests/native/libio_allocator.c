/* Entire unchanged primary TU; only malloc/free are controlled observations. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
static void *libio_allocator_allocate(size_t);
static void libio_allocator_release(void *);
#define malloc libio_allocator_allocate
#define free libio_allocator_release
#include <floatconv.c>
#undef malloc
#undef free
#include "libio_allocator_golden.h"

typedef char pointer32[sizeof(void *)==4?1:-1];
typedef char integer32[sizeof(int)==4?1:-1];
typedef char size32[sizeof(size_t)==4?1:-1];
typedef char limb32[sizeof(unsigned32)==4?1:-1];
typedef char short16[sizeof(short)==2?1:-1];
typedef char header20[BIGINT_HEADER_SIZE==20?1:-1];

static unsigned32 *arena;
static unsigned fixture,event_n,events[1][3],mutation;
static unsigned long checks;

static unsigned initial_word(unsigned i,unsigned seed)
{ return 0x6a31c29du^(i*0x10203u)^(seed*0x443u); }

static unsigned offset(const void *p)
{
  unsigned value=(unsigned)p,start=(unsigned)arena;
  if(value<start||value>=start+LIBIO_ALLOCATOR_WORDS*4u) exit(2);
  return value-start;
}

static void check(unsigned actual,unsigned expected,const char *label,unsigned i)
{
  ++checks;
  if(actual!=expected) {
    fprintf(stderr,"fixture%u %s[%u] actual%08x expected%08x\n",fixture,label,i,actual,expected);
    exit(1);
  }
}

static void *libio_allocator_allocate(size_t size)
{
  Bigint *b=(Bigint *)((unsigned char *)arena+0x1000u);
  if((size!=52&&size!=84&&size!=148)||event_n) exit(2);
  events[0][0]=1;events[0][1]=(unsigned)size;events[0][2]=offset(b);event_n=1;
  if(mutation) {
    b->k=0x76543210;b->maxwds=0x12345678;
    b->on_stack=(short)0x1234;b->sign=(short)0xabcd;b->wds=7;
  }
  return b;
}

static void libio_allocator_release(void *pointer)
{
  Bigint *b=(Bigint *)pointer;
  if(event_n) exit(2);
  events[0][0]=2;events[0][1]=offset(b);events[0][2]=(unsigned short)b->on_stack;event_n=1;
  if(mutation) { b->sign=(short)((unsigned short)b->sign^0x55aau);b->wds=3; }
}

static void initialize(Bigint *b,unsigned k,unsigned n,unsigned stack,unsigned sign,const unsigned *limbs)
{
  unsigned i;b->k=(int)k;b->maxwds=(int)(1u<<k);b->wds=(int)n;
  b->on_stack=(short)stack;b->sign=(short)sign;
  for(i=0;i<n;++i)b->x[i]=limbs[i];
}

int main(void)
{
  unsigned count=sizeof(libio_allocator_fixtures)/sizeof(libio_allocator_fixtures[0]);
  arena=(unsigned32 *)malloc(LIBIO_ALLOCATOR_WORDS*sizeof(unsigned32));
  if(!arena)return 2;
  for(fixture=0;fixture<count;++fixture) {
    const LibioAllocatorFixture *f=&libio_allocator_fixtures[fixture];
    Bigint *a=(Bigint *)((unsigned char *)arena+0x100u);
    Bigint *b=(Bigint *)((unsigned char *)arena+0x400u);
    unsigned expected[LIBIO_ALLOCATOR_WORDS],i,j,result=0;
#ifdef LIBIO_ALLOCATOR_ONLY_FIXTURE
    if(fixture!=LIBIO_ALLOCATOR_ONLY_FIXTURE)continue;
#endif
    for(i=0;i<LIBIO_ALLOCATOR_WORDS;++i)expected[i]=arena[i]=initial_word(i,f->seed);
    initialize(a,f->k,f->n,f->stack,f->sign,f->a);
    initialize(b,f->k,f->n,f->stack,f->sign^0x55aau,f->b);
    mutation=f->mutation;event_n=0;
    if(f->method==0)result=offset(Balloc((int)f->argument));
    else if(f->method==1)Bfree(f->is_null?NULL:a);
    else {
      Bigint *dest=f->flip?b:a,*src=f->flip?a:b;
      if(f->selfcopy)src=dest;
      Bcopy(dest,src);
    }
    check(result,f->result,"pointer_return",0);
    check(event_n,f->event_n,"event_count",0);
    for(i=0;i<event_n;++i)for(j=0;j<3;++j)check(events[i][j],f->events[i][j],"event",j);
    for(i=0;i<f->delta_n;++i) {
      if(f->deltas[i][0]>=LIBIO_ALLOCATOR_WORDS||(i&&f->deltas[i-1][0]>=f->deltas[i][0]))return 3;
      expected[f->deltas[i][0]]=f->deltas[i][1];
    }
    for(i=0;i<LIBIO_ALLOCATOR_WORDS;++i)check(arena[i],expected[i],"arena",i);
  }
  free(arena);
  printf("libio allocator: %lu checks across %u complete original fixtures\n",checks,count);
  return 0;
}
