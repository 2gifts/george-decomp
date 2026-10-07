/* Whole genuine string C on initialized padded disjoint ordinary objects. */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "runtime_copy_compare_golden.h"
extern int compare_real_strcmp(const char *,const char *);
extern char *compare_real_strcpy(char *,const char *);
typedef union { unsigned long long alignment; unsigned char bytes[512]; } Owned;
static Owned storage[3] __attribute__((aligned(16)));
static unsigned char expected[1536];
static unsigned long checks;
static unsigned char payload(unsigned int p,unsigned int i)
{
    static const unsigned char a[]={127,128,255,1},b[]={1,128,254,127};
    return p==0?65:p==1?255:p==2?128:p==3?a[i&3]:b[i&3];
}
static unsigned int canonical(const void *p)
{
    uintptr_t a=(uintptr_t)p;unsigned int j;
    if(!p)return 0;
    for(j=0;j<3;++j){uintptr_t b=(uintptr_t)storage[j].bytes;
        if(b<=a && a<b+512)return 0x20000u+j*0x400u+(unsigned int)(a-b);}
    return 0xFFFFFFFFu;
}
int main(void)
{
    size_t fixture;unsigned int i,j;
    if(sizeof(void *)!=4 || sizeof(unsigned long)!=4 || CHAR_BIT!=8 || LONG_MAX!=2147483647L)return 2;
    for(fixture=0;fixture<sizeof(copy_compare_golden)/sizeof(copy_compare_golden[0]);++fixture){
        const CopyCompareGolden *g=&copy_compare_golden[fixture];const int *p=g->p;unsigned int result;
        for(j=0;j<3;++j)for(i=0;i<512;++i)storage[j].bytes[i]=(unsigned char)((i*37+j*59)^0xA5);
        for(j=0;j<2;++j){unsigned int offset=(unsigned int)p[1+j];
            for(i=0;i<288;++i)storage[j].bytes[offset+i]=payload(p[6],i);
            storage[j].bytes[offset+p[4+j]]=0;}
        if(p[7]>=0)storage[1].bytes[p[2]+p[7]]=(unsigned char)p[8];
        for(j=0;j<3;++j)for(i=0;i<512;++i)expected[j*512+i]=storage[j].bytes[i];
        for(i=0;i<g->count;++i){const CopyCompareRun *r=&copy_compare_runs[g->start+i];unsigned int k;
            if((unsigned int)r->offset+r->length>1536)return 3;
            for(k=0;k<r->length;++k)expected[r->offset+k]=r->value;}
        result=p[0]?canonical(compare_real_strcpy((char *)storage[2].bytes+p[3],(const char *)storage[0].bytes+p[1])):
            (unsigned int)compare_real_strcmp((const char *)storage[0].bytes+p[1],(const char *)storage[1].bytes+p[2]);
        if(result!=g->result){fprintf(stderr,"result mismatch fixture %lu got %08x expected %08x\n",(unsigned long)fixture,result,g->result);return 4;}
        ++checks;
        for(j=0;j<3;++j)for(i=0;i<512;++i){
            if(storage[j].bytes[i]!=expected[j*512+i]){fprintf(stderr,"byte mismatch fixture %lu buffer %u offset %u\n",(unsigned long)fixture,j,i);return 5;}
            ++checks;}
    }
    printf("runtime copy/compare: %lu checks\n",checks);return 0;
}
