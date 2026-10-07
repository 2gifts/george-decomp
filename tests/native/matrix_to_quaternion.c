#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "george/matrix_to_quaternion.h"
#include "matrix_to_quaternion_golden.h"

/* The whole production source/header are consistently renamed for this host
 * contract. This initialized test word array is not recovered retail data.
 */
s32 matrix_quaternion_observer_table[3];
static float arena[64] __attribute__((aligned(16)));
static unsigned checks, fixture;

static void check(int condition, const char *what, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr,"quaternion fixture %u word %u: %s\n",fixture,index,what);
        exit(1);
    }
}

static u32 raw(float value)
{
    u32 bits;
    memcpy(&bits,&value,4);
    return bits;
}

int main(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(matrix_quaternion_golden)/sizeof(matrix_quaternion_golden[0]);++i) {
        const struct MatrixQuaternionGolden *g=&matrix_quaternion_golden[i];
        fixture=i;
        check(sizeof(void *)==4 && sizeof(float)==4 && sizeof(s32)==4,
              "native scalar/pointer ABI",0);
        check(sizeof(GeorgeMathVec4)==16 && sizeof(GeorgeRotationMatrix)==64,
              "native consumed prefixes",0);
        check(g->output_word+4<=64 && g->matrix_word+16<=64,
              "initialized typed arena",0);
        for(j=0;j<64;++j)memcpy(&arena[j],&g->initial[j],4);
        for(j=0;j<3;++j) {
            check(g->table[j]>=0 && g->table[j]<=2,"bounded initialized table",j);
            matrix_quaternion_observer_table[j]=g->table[j];
        }
        george_matrix_to_quaternion((GeorgeMathVec4 *)(arena+g->output_word),
             (const GeorgeRotationMatrix *)(arena+g->matrix_word));
        for(j=0;j<64;++j)check(raw(arena[j])==g->expected[j],"full original-observed arena",j);
        for(j=0;j<3;++j)check(matrix_quaternion_observer_table[j]==g->table[j],"table unchanged",j);

        /* A geometric invariant is used only for disjoint proper cube
         * rotations (the first 144 cases, six placements per rotation).
         * Shifted aliases and arbitrary writable-table variants are excluded.
         */
        if(i<144 && i%6==0) {
            const float *q=arena+g->output_word;
            float norm=((q[0]*q[0]+q[1]*q[1])+q[2]*q[2])+q[3]*q[3];
            float xx=q[0]*q[0], yy=q[1]*q[1], zz=q[2]*q[2];
            float xy=q[0]*q[1], xz=q[0]*q[2], yz=q[1]*q[2];
            float wx=q[3]*q[0], wy=q[3]*q[1], wz=q[3]*q[2];
            float rotation[9];
            rotation[0]=1.0f-2.0f*(yy+zz); rotation[1]=2.0f*(xy-wz);
            rotation[2]=2.0f*(xz+wy); rotation[3]=2.0f*(xy+wz);
            rotation[4]=1.0f-2.0f*(xx+zz); rotation[5]=2.0f*(yz-wx);
            rotation[6]=2.0f*(xz-wy); rotation[7]=2.0f*(yz+wx);
            rotation[8]=1.0f-2.0f*(xx+yy);
            check(fabsf(norm-1.0f)<0.000001f,"disjoint proper rotation norm",0);
            for(j=0;j<9;++j) {
                float expected;
                unsigned row=j/3, column=j%3;
                memcpy(&expected,&g->initial[g->matrix_word+row*4+column],4);
                check(fabsf(rotation[j]-expected)<0.000001f,"independent rotation reconstruction",j);
            }
        }
    }
    printf("matrix-to-quaternion: %u checks across %u initialized fixtures\n",checks,i);
    return 0;
}
