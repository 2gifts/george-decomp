#include <stdio.h>
#include <string.h>
#include "george/geometry_bounds.h"

static unsigned checks,failures,normalize_calls;
static u32 allocated_size;
static GeorgePlaneArray *allocation_result;
static GeorgeGeometryFrame *watched_frame;
static GeorgeMathVec3 *changed_lower,*changed_upper;
static GeorgeMathVec4 seen_axes[3];
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; printf("failure line %d\n",__LINE__); } } while(0)
static float scalar(u32 word) { union { u32 w; float f; } v;v.w=word;return v.f; }
static u32 bits(float value) { union { u32 w; float f; } v;v.f=value;return v.w; }

void *func_002AEC28(u32 size) { allocated_size=size;return allocation_result; }
float func_002A3538(GeorgeMathVec3 *vector)
{
    unsigned index=normalize_calls++;
    CHECK(index<3);
    if(index<3) seen_axes[index]=*(GeorgeMathVec4 *)vector;
    if(watched_frame) CHECK(vector==(GeorgeMathVec3 *)&watched_frame->axis[index%3]);
    if(index==0 && changed_lower) {
        changed_lower->x=changed_lower->y=changed_lower->z=900;
        changed_upper->x=changed_upper->y=changed_upper->z=-900;
    }
    vector->x=10.0f+index;vector->y=20.0f+index;vector->z=30.0f+index;
    return 99.0f;
}

#include "geometry_bounds_golden.h"
static void golden_cases(void)
{
    unsigned case_index,i;
    for(case_index=0;case_index<sizeof(bounds_golden)/sizeof(bounds_golden[0]);case_index++) {
        const struct BoundsGolden *g=&bounds_golden[case_index];
        union { u32 words[64]; float values[64]; } buffer;
        float *a=buffer.values+g->a,*b=buffer.values+g->b;
        u32 result=0;
        memcpy(buffer.words,g->initial,sizeof(buffer.words));
        switch(g->routine) {
        case 0:func_002A0048((GeorgeBounds *)a,g->parameter);break;
        case 1:result=func_002A00A0((GeorgeBounds *)a,(GeorgeMathVec3 *)b);break;
        case 2:result=func_002A0138((GeorgeBounds *)a,(GeorgeBounds *)b);break;
        case 3:result=func_002A0248((GeorgePlaneArray *)a,g->b,(GeorgeMathVec3 *)(buffer.values+g->c));break;
        case 4:result=func_002A02E0((GeorgePlaneArray *)a,(GeorgeMathVec4 *)b);break;
        case 5:result=func_002A0390((GeorgeMathVec3 *)a,(GeorgeMathVec4 *)b,g->c);break;
        case 6:func_002A0E20((GeorgeRotationMatrix *)a,(GeorgeRotationMatrix *)b);break;
        default:CHECK(0);break;
        }
        if(g->routine!=0 && g->routine!=6) CHECK(result==g->result);
        for(i=0;i<64;i++) if(buffer.words[i]!=g->expected[i]) {
            ++checks;++failures;
            printf("golden %u routine %d word %u: %08x != %08x\n",case_index,g->routine,i,buffer.words[i],g->expected[i]);
        } else ++checks;
    }
}

static void gates(void)
{
    GeorgeBounds box={{-1,-2,-3},{1,2,3}},other=box;
    GeorgeMathVec3 point={1,2,3};
    struct { u32 count; GeorgeMathVec4 planes[4]; } block={1,{{1,0,0,-1}}};
    GeorgeMathVec4 sphere={0,0,0,0.5f};
    GeorgeMathVec4 plane={1,0,0,0};
    float closest;
    CHECK(func_002A00A0(&box,&point)==1);
    point.x=scalar(0x3F800001);CHECK(func_002A00A0(&box,&point)==0);
    point.x=scalar(0x7FC00000);CHECK(func_002A00A0(&box,&point)==1);
    other.lower.x=1;CHECK(func_002A0138(&box,&other)==1);
    other.lower.x=scalar(0x3F800001);CHECK(func_002A0138(&box,&other)==0);
    other.lower.x=scalar(0x7FC00000);CHECK(func_002A0138(&box,&other)==1);
    CHECK(func_002A0248(NULL,0,NULL)==1);
    block.count=0;CHECK(func_002A0248((GeorgePlaneArray *)&block,3,NULL)==1);
    CHECK(func_002A02E0((GeorgePlaneArray *)&block,NULL)==1);
    block.count=1;point.x=0;point.y=point.z=0;
    CHECK(func_002A0248((GeorgePlaneArray *)&block,1,&point)==1);
    point.x=1;CHECK(func_002A0248((GeorgePlaneArray *)&block,1,&point)==0);
    point.x=scalar(0x7FC00000);CHECK(func_002A0248((GeorgePlaneArray *)&block,1,&point)==1);
    CHECK(func_002A02E0((GeorgePlaneArray *)&block,&sphere)==1);
    sphere.x=0.5f;CHECK(func_002A02E0((GeorgePlaneArray *)&block,&sphere)==3);
    sphere.x=1.5f;CHECK(func_002A02E0((GeorgePlaneArray *)&block,&sphere)==0);
    sphere.x=scalar(0x7FC00000);CHECK(func_002A02E0((GeorgePlaneArray *)&block,&sphere)==1);
    CHECK(func_002A0390(NULL,NULL,0)==0);
    CHECK(func_002A0390(NULL,NULL,-2)==1);
    point.x=1.0e-5f;CHECK(func_002A0390(&point,&plane,1)==1);
    point.x=scalar(bits(1.0e-5f)+1);CHECK(func_002A0390(&point,&plane,1)==0);
    point.x=scalar(0x7FC00000);CHECK(func_002A0390(&point,&plane,1)==1);
    CHECK(func_002A0410(NULL,NULL,0,NULL,1)==0);
    closest=9;CHECK(func_002A0410(NULL,NULL,-2,&closest,0.5f)==1 && closest==0.5f);
    point.x=0.5f;closest=9;CHECK(func_002A0410(&point,&plane,1,&closest,0.5f)==1 && closest==0.5f);
    point.x=scalar(0x3F000001);CHECK(func_002A0410(&point,&plane,1,&closest,0.5f)==0 && closest==0.5f);
    point.x=-0.25f;CHECK(func_002A0410(&point,&plane,1,&closest,0.5f)==1 && closest==0.25f);
    point.x=scalar(0x7FC00000);CHECK(func_002A0410(&point,&plane,1,&closest,0.5f)==1 && closest==0.5f);
    point.x=point.y=point.z=scalar(0x80000000);plane.x=plane.y=plane.z=1;plane.w=0;
    CHECK(func_002A0410(&point,&plane,1,&closest,0.5f)==1 && bits(closest)==0x80000000);
}

static void output_aliases(void)
{
    GeorgeMathVec3 point={0,0,0};
    GeorgeMathVec4 planes[2]={{1,0,0,0.75f},{2,0,0,0}};
    CHECK(func_002A0410(&point,planes,2,&point.x,0.5f)==1 && point.x==0.25f);
    point.x=2;planes[0].x=0;planes[0].w=0.25f;planes[1].x=9;
    CHECK(func_002A0410(&point,planes,2,&planes[1].x,0.5f)==1 && planes[1].x==0.25f);
}

static void allocation_cases(void)
{
    GeorgePlaneArray result;
    const u32 counts[]={0,1,3,0x10000000u,0xFFFFFFFFu};
    unsigned i;
    allocation_result=&result;
    for(i=0;i<sizeof(counts)/sizeof(counts[0]);i++) {
        result.count=77;
        CHECK(func_002A01E0(counts[i])==&result);
        CHECK(allocated_size==((counts[i]<<4)|4u) && result.count==counts[i]);
    }
    allocation_result=NULL;CHECK(func_002A01E0(7)==NULL && allocated_size==116);
    CHECK((u32)func_002A0238((GeorgePlaneArray *)0x10000,0)==0x10004);
    CHECK((u32)func_002A0238((GeorgePlaneArray *)0x10000,3)==0x10034);
    CHECK((u32)func_002A0238((GeorgePlaneArray *)0x10000,0x10000000u)==0x10004);
    CHECK((u32)func_002A0238((GeorgePlaneArray *)0x10000,0xFFFFFFFFu)==0xFFF4);
    CHECK(func_002A01D8()==0);
}

static void frame_cases(void)
{
    GeorgeGeometryFrame frame;
    GeorgeBounds bounds={{1,2,3},{7,6,5}};
    unsigned i;
    memset(&frame,0,sizeof(frame));frame.position.w=123;
    normalize_calls=0;watched_frame=&frame;
    changed_lower=&bounds.lower;changed_upper=&bounds.upper;
    func_002A0370(&frame,&bounds);
    CHECK(normalize_calls==3);
    CHECK(frame.position.x==4 && frame.position.y==4 && frame.position.z==4 && frame.position.w==123);
    CHECK(seen_axes[0].x==6 && seen_axes[0].y==0 && seen_axes[0].z==0 && seen_axes[0].w==3);
    CHECK(seen_axes[1].x==0 && seen_axes[1].y==4 && seen_axes[1].z==0 && seen_axes[1].w==2);
    CHECK(seen_axes[2].x==0 && seen_axes[2].y==0 && seen_axes[2].z==2 && seen_axes[2].w==1);
    for(i=0;i<3;i++) CHECK(frame.axis[i].x==10.0f+i && frame.axis[i].y==20.0f+i && frame.axis[i].z==30.0f+i);
    changed_lower=changed_upper=NULL;
    memset(&frame,0,sizeof(frame));frame.axis[0].x=1;frame.axis[0].y=2;frame.axis[0].z=3;
    frame.axis[1].x=7;frame.axis[1].y=6;frame.axis[1].z=5;
    normalize_calls=0;
    func_0029E7F0(&frame,(GeorgeMathVec3 *)&frame.axis[0],(GeorgeMathVec3 *)&frame.axis[1]);
    CHECK(frame.position.x==4 && frame.position.y==4 && frame.position.z==4);
    CHECK(seen_axes[1].y==4 && seen_axes[2].z==2);
    watched_frame=NULL;
}

int main(void)
{
    golden_cases();gates();output_aliases();allocation_cases();frame_cases();
    printf("geometry_bounds: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
