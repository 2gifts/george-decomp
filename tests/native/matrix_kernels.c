#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/matrix_kernels.h"
#include "matrix_kernels_golden.h"

static unsigned checks, fixture;
static float arena[192] __attribute__((aligned(16)));
static void check(int ok, const char *what, unsigned index)
{
    ++checks;
    if (!ok) {
        fprintf(stderr, "matrix fixture %u word %u: %s\n", fixture, index, what);
        exit(1);
    }
}
static unsigned raw(float value)
{
    unsigned result;
    memcpy(&result, &value, sizeof(result));
    return result;
}
static void fixtures(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(matrix_golden)/sizeof(matrix_golden[0]);++i) {
        const struct MatrixGolden *q=&matrix_golden[i];
        fixture=i;
        check(q->a+16<=192 && q->b+(q->routine?16:4)<=192 &&
              q->out+(q->routine?16:4)<=192, "initialized typed arena bounds",0);
        memcpy(arena,q->initial,sizeof(arena));
        if(q->routine==0)
            func_002A1DF0(arena+q->a,arena+q->b,arena+q->out);
        else {
            check(q->routine==1,"actual selected routine",0);
            func_002A2200(arena+q->out,arena+q->a,arena+q->b);
        }
        for(j=0;j<192;++j)
            check(raw(arena[j])==q->expected[j],"complete original arena",j);
    }
}
static void independent(void)
{
    float identity[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    float matrix[16]={1,2,3,4,2,1,4,3,3,4,1,2,4,3,2,1};
    float input[4]={0,0,0,2}, out[16], before[16];
    unsigned i;
    fixture=0xFFFFFFFFu;
    func_002A1DF0(matrix,input,out);
    check(out[0]==8 && out[1]==6 && out[2]==4 && out[3]==2,
          "arbitrary homogeneous W contributes fourth row",0);
    func_002A1DF0(identity,input,input);
    check(input[0]==0 && input[1]==0 && input[2]==0 && input[3]==2,
          "identity and complete point alias",0);
    memcpy(before,matrix,sizeof(matrix));
    func_002A2200(matrix,matrix,identity);
    for(i=0;i<16;++i)check(raw(matrix[i])==raw(before[i]),"right identity with matrix output alias",i);
    func_002A2200(matrix,identity,matrix);
    for(i=0;i<16;++i)check(raw(matrix[i])==raw(before[i]),"left identity with matrix output alias",i);
    /* Independent matrix-order example: multiplying a diagonal left
     * scales complete right rows, while the opposite order scales columns. */
    for(i=0;i<16;++i)identity[i]=0;
    identity[0]=1;identity[5]=2;identity[10]=3;identity[15]=4;
    func_002A2200(out,identity,matrix);
    for(i=0;i<16;++i)check(out[i]==matrix[i]*(float)(i/4+1),"diagonal-left row scaling",i);
    func_002A2200(out,matrix,identity);
    for(i=0;i<16;++i)check(out[i]==matrix[i]*(float)(i%4+1),"diagonal-right column scaling",i);
}
int main(void)
{
    check(sizeof(float)==4 && sizeof(unsigned)==4 && sizeof(void*)==4,
          "actual native widths",0);
    fixtures();independent();
    printf("matrix kernels: %u checks passed / %u unique original fixtures\n",
           checks,(unsigned)(sizeof(matrix_golden)/sizeof(matrix_golden[0])));
    return 0;
}
