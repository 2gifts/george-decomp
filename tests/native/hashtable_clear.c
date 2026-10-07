#include "george/completion_resolver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable_clear_golden.h"

void *D_003F21B8[16];
static GeorgeCompletionTable table;
static void *buckets[16];
static GeorgeCompletionNode nodes[12],free_nodes[4];
static u32 checks;
extern unsigned clear_sgi_reference(void);

static void require(int condition,u32 fixture,u32 word)
{
    ++checks;
    if (!condition) {
        fprintf(stderr,"clear fixture %u word %u failed\n",fixture,word);
        exit(1);
    }
}

static u32 encoded_node(const void *p)
{
    u32 a=(u32)p,base;
    if (!p) return 0;
    base=(u32)nodes;
    if (a>=base && a<base+sizeof(nodes)) return 0x20400U+(a-base);
    base=(u32)free_nodes;
    if (a>=base && a<base+sizeof(free_nodes)) return 0x20600U+(a-base);
    fprintf(stderr,"unknown native clear pointer\n");exit(1);
}

static u32 encoded_bucket(const void *p)
{
    u32 a=(u32)p,base=(u32)buckets;
    if (!p) return 0;
    if (a>=base && a<=base+sizeof(buckets)) return 0x20200U+(a-base);
    fprintf(stderr,"unknown native clear bucket pointer\n");exit(1);
}

static u32 bucket_for(u32 i,const struct ClearGolden *g)
{
    if (g->distribution==0) return 0;
    if (g->distribution==1) return g->buckets-1;
    if (g->distribution==2) return i%g->buckets;
    return (i*7+1)%g->buckets;
}

static void setup(const struct ClearGolden *g)
{
    u32 i,unknown=0xA5310201U;
    memset(&table,0,sizeof(table));memset(buckets,0,sizeof(buckets));
    memset(nodes,0,sizeof(nodes));memset(free_nodes,0,sizeof(free_nodes));
    memset(D_003F21B8,0,sizeof(D_003F21B8));
    memcpy(table.unknown00,&unknown,4);
    table.field04.field00=buckets;table.field04.field04=buckets+g->buckets;
    table.field04.field08=buckets+16;table.field10=g->nodes;
    for (i=0;i<12;++i) {
        nodes[i].field04=0x80000000U+i*0x010101U;
        nodes[i].field08=buckets+(i%16);
    }
    for (i=g->nodes;i!=0;) {
        u32 b;--i;b=bucket_for(i,g);
        nodes[i].field00=(GeorgeCompletionNode *)buckets[b];buckets[b]=nodes+i;
    }
    for (i=0;i<4;++i) {
        free_nodes[i].field00=i+1<g->pool ? free_nodes+i+1 : 0;
        free_nodes[i].field04=0xE5010000U+i;
        free_nodes[i].field08=buckets+i;
    }
    D_003F21B8[1]=g->pool ? free_nodes : 0;
}

/* Independently determine output link order from membership, without reading
 * or following the production node chain. All objects/storage are separate
 * valid initialized C table/node/array objects in this native experiment. */
static void oracle(const struct ClearGolden *g,u32 fixture)
{
    GeorgeCompletionNode *head=g->pool ? free_nodes : 0;
    GeorgeCompletionNode *next[12];u32 b,i;
    for (i=0;i<12;++i) next[i]=0;
    for (b=0;b<g->buckets;++b)
        for (i=0;i<g->nodes;++i)
            if (bucket_for(i,g)==b) { next[i]=head;head=nodes+i; }
    require(D_003F21B8[1]==head,fixture,1000);
    for (i=0;i<12;++i) require(nodes[i].field00==next[i],fixture,1001+i);
    for (i=0;i<16;++i) require(buckets[i]==0,fixture,1020+i);
    require(table.field10==0,fixture,1040);
}

int main(void)
{
    u32 f;
    for (f=0;f<sizeof(clear_golden)/sizeof(clear_golden[0]);++f) {
#ifdef CLEAR_ONLY_FIXTURE
        if (f!=CLEAR_ONLY_FIXTURE) continue;
#endif
        const struct ClearGolden *g=clear_golden+f;
        u32 output[85],i,j=0,unknown;
        setup(g);func_002BF418(&table);oracle(g,f);
        memcpy(&unknown,table.unknown00,4);output[j++]=unknown;
        output[j++]=encoded_bucket(table.field04.field00);
        output[j++]=encoded_bucket(table.field04.field04);
        output[j++]=encoded_bucket(table.field04.field08);output[j++]=table.field10;
        for (i=0;i<16;++i) output[j++]=encoded_node(buckets[i]);
        for (i=0;i<12;++i) {
            output[j++]=encoded_node(nodes[i].field00);output[j++]=nodes[i].field04;
            output[j++]=encoded_bucket(nodes[i].field08);
        }
        for (i=0;i<4;++i) {
            output[j++]=encoded_node(free_nodes[i].field00);output[j++]=free_nodes[i].field04;
            output[j++]=encoded_bucket(free_nodes[i].field08);
        }
        for (i=0;i<16;++i) output[j++]=encoded_node(D_003F21B8[i]);
        require(j==85,f,1050);
        for (i=0;i<85;++i) require(output[i]==g->expected[i],f,i);
    }
    checks+=clear_sgi_reference();
    printf("hashtable_clear: %u checks\n",checks);
    return 0;
}
