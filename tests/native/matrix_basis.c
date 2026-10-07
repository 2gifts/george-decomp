#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/matrix_basis.h"
#include "matrix_basis_golden.h"

static float arena[128] __attribute__((aligned(16)));
static unsigned checks,fixture;
static void check(int ok,const char *what,unsigned index)
{
    ++checks;
    if (!ok) {
        fprintf(stderr,"basis fixture %u word %u: %s\n",fixture,index,what);
        exit(1);
    }
}
static unsigned raw(float value)
{
    unsigned bits;
    memcpy(&bits,&value,4);
    return bits;
}
static void observe(unsigned index)
{
    const struct BasisGolden *q=&basis_golden[index];
    unsigned i;
    fixture=index;
    check(q->out+16<=128 && q->coeff+4<=128 && q->position+3<=128,
          "initialized full arena bounds",0);
    memcpy(arena,q->initial,sizeof(arena));
    if(q->routine==0)
        func_002A1E78((GeorgeRotationMatrix *)(arena+q->out),
                     (const GeorgeMathVec4 *)(arena+q->coeff));
    else {
        check(q->routine==1,"actual selected routine",0);
        func_002A1F18((GeorgeRotationMatrix *)(arena+q->out),
                     (const GeorgeMathVec4 *)(arena+q->coeff),
                     (const GeorgeMathVec3 *)(arena+q->position));
    }
    for(i=0;i<128;++i)
        check(raw(arena[i])==q->expected[i],"full original nominal arena",i);
}
static void typed_objects(void)
{
    struct MatrixWithTail { GeorgeRotationMatrix matrix; unsigned tail[4]; } output;
    GeorgeMathVec4 coefficients={1.0f/16,2.0f/16,3.0f/16,4.0f/16};
    GeorgeMathVec3 position={0.0f,17.0f/128,-23.0f/128};
    const int numerators[12]={115,-10,11,0,14,118,2,0,-5,10,123,0};
    unsigned i;
    fixture=0xFFFFFFFFu;
    for(i=0;i<4;++i)output.tail[i]=0xA5B6C7D8u+i;
    func_002A1E78(&output.matrix,&coefficients);
    for(i=0;i<12;++i)check(output.matrix.element[i]==(float)numerators[i]/128,
                          "independent asymmetric integer polynomial",i);
    for(i=0;i<4;++i)check(output.tail[i]==0xA5B6C7D8u+i,"typed output tail",i);
    { unsigned zero=0x80000000u; memcpy(&position.x,&zero,4); }
    func_002A1F18(&output.matrix,&coefficients,&position);
    check(raw(output.matrix.element[12])==0x80000000u,"translation signed zero raw move",12);
    check(output.matrix.element[13]==position.y && output.matrix.element[14]==position.z &&
          output.matrix.element[15]==1.0f,"typed live position",13);
    for(i=0;i<4;++i)check(output.tail[i]==0xA5B6C7D8u+i,"translated typed tail",i);
}
int main(int argc,char **argv)
{
    unsigned i,count=(unsigned)(sizeof(basis_golden)/sizeof(basis_golden[0]));
    check(sizeof(float)==4 && sizeof(unsigned)==4 && sizeof(void*)==4,
          "actual native widths",0);
    if(argc==2) {
        char *end;
        unsigned index=(unsigned)strtoul(argv[1],&end,10);
        check(*end=='\0' && index<count,"selected genuine negative fixture",0);
        observe(index);
    } else {
        check(argc==1,"default full positive observer",0);
        for(i=0;i<count;++i)observe(i);
        typed_objects();
    }
    printf("matrix basis: %u checks passed / %u unique original fixtures\n",checks,count);
    return 0;
}
