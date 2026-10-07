#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "george/geometry_frustum.h"
#include "geometry_frustum_golden.h"

static unsigned checks, fixture, calls;
static u32 events[20];
static float *arena_base;
static u32 bits(float value) { union { float f;u32 u; } v;v.f=value;return v.u; }
static float value(u32 word) { union { float f;u32 u; } v;v.u=word;return v.f; }
static void check(int condition,const char *reason,unsigned index)
{
    ++checks;
    if(!condition) {
        fprintf(stderr,"frustum fixture%u word%u: %s\n",fixture,index,reason);
        exit(1);
    }
}
/* Native-only naming bridge observes arguments, then executes the unchanged
 * vector_math.c normalizer in its own TU; no normalization substitute. */
extern float frustum_native_normalize(GeorgeMathVec3 *);
float func_002A3538(GeorgeMathVec3 *normal)
{
    unsigned base=calls*4;
    check(calls<5,"five call event bound",calls);
    events[base]=((u32)normal-(u32)arena_base)/4;
    events[base+1]=bits(normal->x);events[base+2]=bits(normal->y);events[base+3]=bits(normal->z);
    ++calls;
    return frustum_native_normalize(normal);
}
static float unexpected(void) { check(0,"unselected trig body executed",0);return 0; }
float func_0029B940(float y,float x) { (void)y;(void)x;return unexpected(); }
float func_0029C090(float v) { (void)v;return unexpected(); }
float func_0029C168(float v) { (void)v;return unexpected(); }
float func_0029C230(float v) { (void)v;return unexpected(); }

static void golden_checks(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(frustum_golden)/sizeof(frustum_golden[0]);++i) {
        const struct FrustumGolden *g=&frustum_golden[i];
        union { u32 word[128];float scalar[128]; } memory;
        fixture=i;memcpy(memory.word,g->initial,sizeof(memory.word));
        calls=0;memset(events,0,sizeof(events));arena_base=memory.scalar;
        func_002A3DF8((GeorgeGeometrySixFaces *)(memory.scalar+g->output),
                    (GeorgeGeometryFrame *)(memory.scalar+g->frame),
                    value(g->dimensions[0]),value(g->dimensions[1]),
                    value(g->dimensions[2]),value(g->dimensions[3]));
        for(j=0;j<128;++j) check(memory.word[j]==g->expected[j],"full arena differs from original trace",j);
        check(calls==5,"all five real normalizers executed",128);
        for(j=0;j<20;++j) check(events[j]==g->events[j],"normalizer arguments/order differ",j);
    }
}
static void independent_invariants(void)
{
    GeorgeGeometryFrame frame={{{1,0,0,0},{0,1,0,0},{0,0,1,0}},{0,0,0,0}};
    GeorgeGeometrySixFaces faces;
    unsigned i,j;
    const float corners[4][3]={{-2,-3,-4},{2,-3,-4},{2,3,-4},{-2,3,-4}};
    fixture=100000;calls=0;arena_base=(float *)&faces;
    func_002A3DF8(&faces,&frame,1,4,2,3);
    check(calls==5,"five real calls in axis invariant",0);
    for(i=0;i<4;++i) {
        const float *v=&faces.faces[i+1].vertex.x;
        for(j=0;j<3;++j) check(v[j]==corners[i][j],"independent axis corner construction",i*3+j);
    }
    check(faces.faces[5].vertex.x==0 && faces.faces[5].vertex.y==0 && faces.faces[5].vertex.z==-1,
          "independent near vertex",0);
    check(faces.faces[4].normal.z==-1 && faces.faces[4].distance==4 &&
          faces.faces[5].normal.z==1 && faces.faces[5].distance==-1,"opposed axis depth planes",1);
    for(i=0;i<3;++i) {
        const float *n=&faces.faces[4].normal.x,*opposite=&faces.faces[5].normal.x;
        check(bits(opposite[i])==(bits(n[i])^0x80000000u),"opposite normal exact signed-zero negation",i);
    }
    for(i=0;i<6;++i) {
        const GeorgeGeometryFace *f=&faces.faces[i];
        double norm=(double)f->normal.x*f->normal.x+(double)f->normal.y*f->normal.y+(double)f->normal.z*f->normal.z;
        double dot=(double)f->normal.x*f->vertex.x+(double)f->normal.y*f->vertex.y+(double)f->normal.z*f->vertex.z;
        check(fabs(norm-1)<0.000001,"independent unit normal",i);
        /* Each side vertex is the apex; both depth planes use their own point. */
        if(i>=4 || i==0) check(fabs(dot-f->distance)<0.000001,"own plane point equation",i);
    }
    /* Zero-length original fallback is observable and need not produce an
     * orthodox geometric normal. Its opposite still toggles every sign bit. */
    calls=0;func_002A3DF8(&faces,&frame,0,0,0,0);
    check(calls==5,"degenerate five calls",0);
    for(i=0;i<5;++i) check(faces.faces[i].normal.x==1 && faces.faces[i].normal.y==0 && faces.faces[i].normal.z==0,
                         "real zero-length normalization fallback",i);
    for(i=0;i<3;++i) check(bits((&faces.faces[5].normal.x)[i])==(bits((&faces.faces[4].normal.x)[i])^0x80000000u),
                           "degenerate opposite signed zeros",i);
}
int main(void)
{
    golden_checks();independent_invariants();
    printf("geometry_frustum: %u checks passed\n",checks);return 0;
}
