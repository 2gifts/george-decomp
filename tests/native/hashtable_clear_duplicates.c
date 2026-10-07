#include "george/hashtable_clear_duplicates.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable_clear_duplicates_golden.h"

/* These complete carriers belong only to this initialized test. Neither their
 * sixteen-byte stride nor their tails establish any original node class. */
struct NodeCarrier { GeorgeClearNodePrefix prefix; u32 tail[3]; };
struct TableCarrier { GeorgeClearTablePrefix prefix; u32 tail[3]; };
static struct TableCarrier table;
static struct NodeCarrier nodes[12],pool[4];
static void *buckets[16];
void *D_003F21B8[16];
static u32 checks;
static void (*const methods[5])(GeorgeClearTablePrefix *)={
    func_0024B7D0,func_0024BF58,func_0024C320,func_0024C6E8,func_0024CE78
};
typedef char test_node_stride[(sizeof(struct NodeCarrier)==16)?1:-1];
typedef char test_table_stride[(sizeof(struct TableCarrier)==32)?1:-1];

static void require(int ok,u32 fixture,u32 word)
{
    ++checks;
    if (!ok) {fprintf(stderr,"duplicate clear fixture %u word %u failed\n",fixture,word);exit(1);}
}

static u32 encoded_node(void *pointer)
{
    u32 a=(u32)pointer,base;
    if (!pointer) return 0;
    base=(u32)nodes;
    if (a>=base && a<base+sizeof(nodes) && (a-base)%16==0) return 0x20400U+a-base;
    base=(u32)pool;
    if (a>=base && a<base+sizeof(pool) && (a-base)%16==0) return 0x20600U+a-base;
    fprintf(stderr,"unknown duplicate clear node representation\n");exit(2);
}

static u32 encoded_bucket(void *pointer)
{
    u32 a=(u32)pointer,base=(u32)buckets;
    if (a>=base && a<=base+sizeof(buckets) && (a-base)%4==0) return 0x20200U+a-base;
    base=(u32)D_003F21B8;
    if (a>=base && a<=base+sizeof(D_003F21B8) && (a-base)%4==0) return 0x3F21B8U+a-base;
    fprintf(stderr,"unknown duplicate clear bucket representation\n");exit(2);
}

static u32 membership(u32 i,const struct DuplicateClearGolden *g)
{
    if (g->distribution==0) return 0;
    if (g->distribution==1) return g->buckets-1;
    if (g->distribution==2) return i%g->buckets;
    return (i*7+1)%g->buckets;
}

static void setup(const struct DuplicateClearGolden *g)
{
    u32 i,j;void **active;
    memset(&table,0,sizeof(table));memset(nodes,0,sizeof(nodes));
    memset(pool,0,sizeof(pool));memset(buckets,0,sizeof(buckets));memset(D_003F21B8,0,sizeof(D_003F21B8));
    active=g->alias==0 ? buckets : D_003F21B8+(g->alias==1 ? 4 : 1);
    table.prefix.unknown00=0xA5310201U;
    table.prefix.field04.field00=active;table.prefix.field04.field04=active+g->buckets;
    table.prefix.field04.unknown08=0xBD310201U;table.prefix.field10=g->nodes;
    for (j=0;j<3;++j) table.tail[j]=0xBCF10000U+j;
    for (i=0;i<12;++i)
        for (j=0;j<3;++j) nodes[i].tail[j]=0x80000000U+i*0x010101U+j*0x112233U;
    for (i=g->nodes;i!=0;) {
        u32 b;--i;b=membership(i,g);
        nodes[i].prefix.field00=(GeorgeClearNodePrefix *)active[b];active[b]=&nodes[i].prefix;
    }
    for (i=0;i<4;++i) {
        pool[i].prefix.field00=i+1<g->pool ? &pool[i+1].prefix : 0;
        for (j=0;j<3;++j) pool[i].tail[j]=0xE5010000U+i+j*0x334455U;
    }
    D_003F21B8[1]=g->pool ? &pool[0].prefix : 0;
}

/* Membership-only model: never follows the production next links. The true
 * new functions execute separately, preserving all untouched carrier tails. */
static void oracle(const struct DuplicateClearGolden *g,u32 f)
{
    GeorgeClearNodePrefix *head=g->pool ? &pool[0].prefix : 0,*next[12];u32 b,i;
    for (i=0;i<12;++i) next[i]=0;
    for (b=0;b<g->buckets;++b)
        for (i=0;i<g->nodes;++i)
            if (membership(i,g)==b) {next[i]=head;head=&nodes[i].prefix;}
    require(D_003F21B8[1]==head,f,1000);
    for (i=0;i<12;++i) require(nodes[i].prefix.field00==next[i],f,1001+i);
    for (i=0;i<g->buckets;++i) require(table.prefix.field04.field00[i]==0,f,1020+i);
    require(table.prefix.field10==0,f,1040);
}

int main(void)
{
    u32 f;
    for (f=0;f<sizeof(duplicate_clear_golden)/sizeof(duplicate_clear_golden[0]);++f) {
        const struct DuplicateClearGolden *g=duplicate_clear_golden+f;u32 output[104],i,j,k=0;
#ifdef CLEAR_ONLY_FIXTURE
        if (f!=CLEAR_ONLY_FIXTURE) continue;
#endif
        setup(g);methods[g->routine](&table.prefix);oracle(g,f);
        output[k++]=table.prefix.unknown00;
        output[k++]=encoded_bucket(table.prefix.field04.field00);output[k++]=encoded_bucket(table.prefix.field04.field04);
        output[k++]=table.prefix.field04.unknown08;output[k++]=table.prefix.field10;
        for (i=0;i<3;++i) output[k++]=table.tail[i];
        for (i=0;i<16;++i) output[k++]=encoded_node(buckets[i]);
        for (i=0;i<12;++i) {
            output[k++]=encoded_node(nodes[i].prefix.field00);
            for (j=0;j<3;++j) output[k++]=nodes[i].tail[j];
        }
        for (i=0;i<4;++i) {
            output[k++]=encoded_node(pool[i].prefix.field00);
            for (j=0;j<3;++j) output[k++]=pool[i].tail[j];
        }
        for (i=0;i<16;++i) output[k++]=encoded_node(D_003F21B8[i]);
        require(k==104,f,1050);
        for (i=0;i<104;++i) require(output[i]==g->expected[i],f,i);
    }
    printf("hashtable_clear_duplicates: %u checks\n",checks);return 0;
}
