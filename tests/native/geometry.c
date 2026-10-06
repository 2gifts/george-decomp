#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/geometry.h"
#include "geometry_golden.h"

static unsigned checks,failures;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n",__LINE__); } } while (0)
static float scalar(u32 value) { union { float scalar;u32 word; } v;v.word=value;return v.scalar; }
static u32 word(float value) { union { float scalar;u32 word; } v;v.scalar=value;return v.word; }
static void close_value(float actual,float expected)
{
    if(expected==0.0f) CHECK(word(actual)==word(expected));
    else CHECK(fabsf(actual-expected)<=0.000002f*(1.0f+fabsf(expected)));
}
/* The constructor only calls the recovered normalizer; accidental trig calls
 * are failures. The source is linked directly, without original game files. */
float func_0029B940(float y,float x) { (void)y;(void)x;CHECK(0);return 0; }
float func_0029C090(float x) { (void)x;CHECK(0);return 0; }
float func_0029C168(float x) { (void)x;CHECK(0);return 0; }
float func_0029C230(float x) { (void)x;CHECK(0);return 0; }

static void instruction_golden(void)
{
    const float matrix[16]={1,0,0,90,0,1,0,91,0,0,1,92,3,-2,5,93};
    unsigned i,j;
    for(i=0;i<sizeof geometry_golden/sizeof geometry_golden[0];++i) {
        union { GeorgeGeometryPyramid aligned;float words[56]; } memory;
        GeorgeGeometryFrame separate,*frame;
        for(j=0;j<56;++j) memory.words[j]=scalar(geometry_golden[i].initial[j]);
        memcpy(&separate,matrix,sizeof separate);
        frame=geometry_golden[i].frame_offset<0?&separate:
            (GeorgeGeometryFrame *)((u8 *)memory.words+geometry_golden[i].frame_offset);
        func_002A3640(&memory.aligned,frame,4,2,1);
        for(j=0;j<35;++j) close_value(memory.words[j],scalar(geometry_golden[i].expected[j]));
        for(j=35;j<56;++j) CHECK(word(memory.words[j])==geometry_golden[i].initial[j]);
    }
}

static void outward_planes(void)
{
    GeorgeGeometryFrame frame={{{1,0,0,0},{0,1,0,0},{0,0,1,0}},{3,-2,5,0}};
    GeorgeGeometryPyramid shape;
    const GeorgeMathVec3 expected[5]={{3,-2,5},{1,-3,1},{5,-3,1},{5,-1,1},{1,-1,1}};
    GeorgeMathVec3 inside={3,-2,3};unsigned i,j;
    func_002A3640(&shape,&frame,4,2,1);
    for(i=0;i<5;++i) {
        GeorgeGeometryFace *face=&shape.faces[i];
        float length=(face->normal.x*face->normal.x+face->normal.y*face->normal.y)+face->normal.z*face->normal.z;
        CHECK(fabsf(length-1)<0.000001f);
        CHECK(face->vertex.x==expected[i].x && face->vertex.y==expected[i].y && face->vertex.z==expected[i].z);
        CHECK((face->normal.x*inside.x+face->normal.y*inside.y)+face->normal.z*inside.z<=face->distance);
        for(j=0;j<5;++j) {
            float projection=(face->normal.x*expected[j].x+face->normal.y*expected[j].y)+face->normal.z*expected[j].z;
            CHECK(projection<=face->distance+0.000002f);
        }
    }
    func_002A3640(&shape,&frame,0,0,0);
    for(i=0;i<5;++i) {
        CHECK(shape.faces[i].normal.x==1 && shape.faces[i].normal.y==0 && shape.faces[i].normal.z==0);
        CHECK(shape.faces[i].distance==3);
        CHECK(shape.faces[i].vertex.x==3 && shape.faces[i].vertex.y==-2 && shape.faces[i].vertex.z==5);
    }
}

int main(void)
{
    instruction_golden();outward_planes();
    printf("geometry: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
