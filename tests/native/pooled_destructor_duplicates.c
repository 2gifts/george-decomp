/* New five source instantiations on initialized carriers. Inherited full
 * native controller TU supplies controlled base/release callbacks only;
 * its historical1488 checks are not counted or run here. */
#define main inherited_controller_main
#include "actor_controller.c"
#undef main
#include "george/pooled_destructor_duplicates.h"
#include "pooled_destructor_duplicates_golden.h"
static u32 active_mutation,active_replacement;
static union { long long align;u8 bytes[64]; } carrier;
static u8 manager_old[16],manager_new[16];
static void *active_object;
static void controlled_mutation(Event *e)
{
 if(e->address!=0x003064F0U)return;
 CHECK(e->argument[0]==(u32)active_object&&e->argument[1]==0);
 if(active_mutation&1U)FIELD(active_object,4,u16)=(u16)active_replacement;
 if(active_mutation&2U)D_004961F4=manager_new;
 if(active_mutation&4U)FIELD(active_object,0,u32)=0xD5A30102U;
}
static u32 translated(u32 x)
{
 if(x==0x20100U)return(u32)carrier.bytes;
 if(x==0x20110U)return(u32)(carrier.bytes+16);
 if(x==0x20300U)return(u32)manager_old;
 if(x==0x20400U)return(u32)manager_new;
 return x;
}
int main(void)
{
 void (*functions[5])(void *,u32)={func_0019A3D0,func_0019D3F0,func_001F0120,func_0022CB40,func_00276C60};
 u32 i,j,k,branches[5][2]={{0}};CHECK(sizeof(void *)==4);CHECK((GeorgeActorBits64)(u32)carrier.bytes+64U<0x80000000ULL);
 CHECK(((u32)carrier.bytes&3U)==0);CHECK(sizeof(GeorgePooledDestructorConsumedPrefix)==6);
 for(i=0;i<sizeof(pooled_golden)/sizeof(pooled_golden[0]);++i){
  const struct PooledGolden *g=&pooled_golden[i];
  event_count=reply_count=0;hook=controlled_mutation;active_mutation=g->mutation;active_replacement=g->replacement;
  for(j=0;j<64;++j)carrier.bytes[j]=(u8)(0xA5U+17U*j);
  active_object=carrier.bytes+g->offset;FIELD(active_object,4,u16)=(u16)g->size;
  D_004961F4=g->manager==0?0:g->manager==1?manager_old:manager_new;
  functions[g->routine](active_object,g->mode);++branches[g->routine][g->mode&1U];
  CHECK(event_count==(int)g->event_count);
  for(j=0;j<16;++j)CHECK(FIELD(carrier.bytes,j*4,u32)==g->expected[j]);
  CHECK((u32)D_004961F4==translated(g->expected[16]));
  for(j=0;j<g->event_count;++j){
   CHECK(events[j].address==g->events[j][0]);
   for(k=0;k<4;++k){u32 x=g->events[j][k+1];
    if((j==0&&k==0)||(j==1&&(k==0||k==1)))x=translated(x);
    CHECK(events[j].argument[k]==x);
   }
  }
 }
 for(i=0;i<5;++i)CHECK(branches[i][0]&&branches[i][1]);
 printf("pooled_destructor_duplicates: %d checks passed (%lu actual five-function fixtures)\n",checks,(unsigned long)(sizeof(pooled_golden)/sizeof(pooled_golden[0])));return 0;
}
