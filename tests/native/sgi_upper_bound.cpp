/* Authored initialized typed fixtures. The selected method is in a separate
 * TU: the complete unchanged genuine SGI template explicit instantiation. */
#include <stdio.h>
#include <stddef.h>
#include "george/sgi_upper_bound.h"
#include "sgi_upper_bound_golden.h"
struct Arena {
 void *array[16]; unsigned reserved[16]; int payload[17];
 unsigned gap[14]; void *slot; unsigned tail[32];
};
static Arena arena;
static void **slot;
static unsigned parameters[8],events[32][6],calls,checks;
static int failure;
static unsigned canonical(const void *p) {
 for (unsigned i=0;i<17;++i) if (p==&arena.payload[i]) return 32+i;
 printf("Invalid typed payload %p calls=%u n=%u mode=%u mutation=%u alias=%u\n",p,calls,parameters[1],parameters[5],parameters[6],parameters[7]);
 failure=1;return ~0u;
}
static int compare(const void *a,const void *b) {
 if(calls>=32){failure=1;return 0;}
 unsigned ai=canonical(a),bi=canonical(b);
 if(failure)return 0;
 int av=*(const int*)a,bv=*(const int*)b,result=av<bv;
 unsigned mode=parameters[5],mutation=parameters[6],middle=0;
 if((mutation&4)&&!calls){
  for(middle=0;middle<16;++middle)if(arena.array[middle]==b)break;
  if(middle==16){puts("Missing initial typed middle");failure=1;return 0;}
 }
 if(mode==1)result=result?-1:0;
 else if(mode==2)result=result?(-2147483647-1):0;
 else if(mode==3)result=(calls&1)?1:0;
 if((mutation&1)&&!calls)*slot=&arena.payload[16];
 if((mutation&2)&&!calls)arena.payload[16]=17;
 if((mutation&4)&&!calls)arena.array[middle]=&arena.payload[16];
 if(mutation&8)*(int*)b=(int)((unsigned)bv+7u);
 unsigned *e=events[calls++];e[0]=ai;e[1]=bi;e[2]=(unsigned)av;
 e[3]=(unsigned)bv;e[4]=(unsigned)result;e[5]=canonical(*slot);
 return result;
}
static void check(unsigned a,unsigned b,unsigned fixture,const char *field,unsigned index) {
 ++checks;
 if(a!=b && !failure){printf("FAIL %u %s[%u]: %08x != %08x\n",fixture,field,index,a,b);failure=1;}
}
int main() {
 if(sizeof(void*)!=4||sizeof(void**)!=4||sizeof(int)!=4||sizeof(int*)!=4||sizeof(GeorgeSGIUpperCompare)!=4||sizeof(size_t)!=4||sizeof(ptrdiff_t)!=4||sizeof(Arena)!=384||offsetof(Arena,payload)!=128||offsetof(Arena,slot)!=252){puts("ABI FAIL");return 2;}
 unsigned suite=sizeof(upper_golden)/sizeof(upper_golden[0]),executions=0;
 for(unsigned routine=0;routine<59;++routine)for(unsigned f=0;f<suite;++f){
  const UpperGolden &g=upper_golden[f];unsigned *words=(unsigned*)&arena;
  for(unsigned i=0;i<8;++i)parameters[i]=g.parameters[i];parameters[0]=routine;
  for(unsigned i=0;i<96;++i)words[i]=0xA5070201u^(i*0x10203u);
  for(unsigned i=0;i<16;++i){
   arena.array[i]=&arena.payload[i];
   arena.payload[i]=parameters[4]==0?(int)(2*i)-12:parameters[4]==1?12-(int)(2*i):parameters[4]==2?3:(i&1)?(-2147483647-1):2147483647;
  }
  arena.payload[16]=(int)parameters[3];if(!parameters[7])arena.slot=&arena.payload[16];
  slot=parameters[7]==0?&arena.slot:parameters[7]==1?&arena.array[0]:&arena.array[7];
  calls=0;failure=0;
  unsigned start=parameters[2],n=parameters[1];
  void **answer=__upper_bound<void**,void*,GeorgeSGIUpperCompare,int>(arena.array+start,arena.array+start+n,*slot,compare,(int*)0);
  check((unsigned)(answer-arena.array),g.result,f,"return",0);
  check(calls,g.event_count,f,"calls",0);
  for(unsigned i=0;i<96;++i){
   unsigned v=i<16?canonical(arena.array[i]):i==63&&!parameters[7]?canonical(arena.slot):words[i];
   check(v,g.memory[i],f,"memory",i);
  }
  for(unsigned i=0;i<calls && i<32;++i)for(unsigned j=0;j<6;++j)check(events[i][j],g.events[i][j],f,"events",i*6+j);
  if(failure)return 1;++executions;
 }
 printf("SGI upper-bound PASS %u checks, %u native executions, %u-case suite x59; old native counts excluded\n",checks,executions,suite);
 return 0;
}
