/* Whole unchanged generic-C comparison on owned initialized padded storage.
 * No game allocator, memory-fault, MMIO, overlapping strings or EE timing model.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "runtime_strings_golden.h"

extern void *runtime_strings_real_memchr(const void *,int,size_t);
extern void *runtime_strings_real_memset(void *,int,size_t);
extern char *runtime_strings_real_strncat(char *,const char *,size_t);
extern int runtime_strings_real_strncmp(const char *,const char *,size_t);
extern char *runtime_strings_real_strncpy(char *,const char *,size_t);

typedef union { unsigned long long alignment; unsigned char bytes[512]; } Owned;
static Owned storage[3] __attribute__((aligned(16)));
static unsigned char expected[1536];
static unsigned long checks,defined_checks,wrapping_checks;

static unsigned char payload(unsigned int pattern,unsigned int index)
{
    static const unsigned char mixed1[]={127,128,255,1};
    static const unsigned char mixed2[]={1,128,254,127};
    return pattern==0?65:pattern==1?255:pattern==2?128:
        pattern==3?mixed1[index&3]:mixed2[index&3];
}

static unsigned int canonical(const void *p)
{
    uintptr_t a=(uintptr_t)p;unsigned int buffer;
    if(!p)return 0;
    for(buffer=0;buffer<3;++buffer){
        uintptr_t begin=(uintptr_t)storage[buffer].bytes;
        if(begin<=a && a<begin+512)return 0x20000u+buffer*0x400u+(unsigned int)(a-begin);
    }
    fprintf(stderr,"returned pointer outside owned object\n");return 0xFFFFFFFFu;
}

static int defined_subtraction(const StringsGolden *g)
{
    const int *p=g->p;unsigned int left=(unsigned int)p[1],offset=(unsigned int)p[3];
    const unsigned long ones=0x01010101UL; /* native long4 is verified in main */
    if(p[0]!=4 || ((p[3]|p[5])&3))return 1;
    while(left>=4){
        unsigned long bits=0;unsigned int i;long value;
        for(i=0;i<4;++i)bits|=(unsigned long)storage[0].bytes[offset+i]<<(8*i);
        value=(long)bits;
        if(value<LONG_MIN+(long)ones)return 0;
        for(i=0;i<4;++i)if(storage[0].bytes[offset+i]==0)return 1;
        offset+=4;left-=4;
    }
    return 1;
}

int main(void)
{
    size_t fixture;unsigned int i,buffer;
    if(sizeof(void *)!=4 || sizeof(size_t)!=4 || sizeof(long)!=4 || CHAR_BIT!=8 || LONG_MAX!=2147483647L){
        fprintf(stderr,"unsupported native layout (requires actual MinGW32 long4)\n");return 2;
    }
    for(fixture=0;fixture<sizeof(strings_golden)/sizeof(strings_golden[0]);++fixture){
        const StringsGolden *g=&strings_golden[fixture];const int *p=g->p;unsigned int result=0;
        for(buffer=0;buffer<3;++buffer)for(i=0;i<512;++i)
            storage[buffer].bytes[i]=(unsigned char)((i*37+buffer*59)^0xA5);
        if(p[0]==0){
            unsigned char fill=((unsigned int)p[2]&255)==0xA5?0x5A:0xA5;
            for(i=0;i<288;++i)storage[0].bytes[p[3]+i]=fill;
            if(p[6]>=0)storage[0].bytes[p[3]+p[6]]=(unsigned char)p[2];
        }else if(p[0]>=2){
            for(i=0;i<288;++i){
                storage[0].bytes[p[3]+i]=payload(p[11],i);
                storage[1].bytes[p[4]+i]=payload(p[11],i);
            }
            if(p[6]>=0)storage[0].bytes[p[3]+p[6]]=0;
            if(p[7]>=0)storage[1].bytes[p[4]+p[7]]=0;
            if(p[9]>=0)storage[1].bytes[p[4]+p[9]]=(unsigned char)p[10];
            if(p[0]==2){
                for(i=0;i<(unsigned int)p[8];++i)storage[2].bytes[p[5]+i]=payload(p[11],i);
                storage[2].bytes[p[5]+p[8]]=0;
            }
        }
        for(buffer=0;buffer<3;++buffer)for(i=0;i<512;++i)expected[buffer*512+i]=storage[buffer].bytes[i];
        for(i=0;i<g->count;++i){
            const StringsRun *run=&strings_runs[g->start+i];unsigned int j;
            if((unsigned int)run->offset+run->length>1536){fprintf(stderr,"bad synthetic golden bound\n");return 3;}
            for(j=0;j<run->length;++j)expected[run->offset+j]=run->value;
        }
        if(defined_subtraction(g)!=(int)g->native_defined){fprintf(stderr,"arithmetic subset mismatch fixture %lu\n",(unsigned long)fixture);return 4;}
        ++checks;
        if(g->native_defined)++defined_checks;else ++wrapping_checks;
        if(p[0]==0)result=canonical(runtime_strings_real_memchr(storage[0].bytes+p[3],p[2],p[1]));
        else if(p[0]==1)result=canonical(runtime_strings_real_memset(storage[2].bytes+p[5],p[2],p[1]));
        else if(p[0]==2)result=canonical(runtime_strings_real_strncat((char *)storage[2].bytes+p[5],(const char *)storage[1].bytes+p[4],p[1]));
        else if(p[0]==3)result=(unsigned int)runtime_strings_real_strncmp((const char *)storage[0].bytes+p[3],(const char *)storage[1].bytes+p[4],p[1]);
        else result=canonical(runtime_strings_real_strncpy((char *)storage[2].bytes+p[5],(const char *)storage[0].bytes+p[3],p[1]));
        if(result!=g->result){fprintf(stderr,"result mismatch fixture %lu got %08x expected %08x\n",(unsigned long)fixture,result,g->result);return 5;}
        ++checks;
        for(buffer=0;buffer<3;++buffer)for(i=0;i<512;++i){
            if(storage[buffer].bytes[i]!=expected[buffer*512+i]){fprintf(stderr,"byte mismatch fixture %lu buffer %u offset %u\n",(unsigned long)fixture,buffer,i);return 6;}
            ++checks;
        }
    }
    printf("runtime strings: %lu checks, %lu defined-subtraction fixtures, %lu explicit-wrap fixtures\n",checks,defined_checks,wrapping_checks);
    return 0;
}
