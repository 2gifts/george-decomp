/* Independent corner and support-radius models; no game assets/instructions. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/geometry_classify.h"

typedef unsigned long long Bits64;
static unsigned checks, conversion_count, event_count;
static float conversions[32];
static unsigned events[128];
static GeorgeGeometryFrame *mutate_frame;
static GeorgeGeometryFace *mutate_face;
static void check(int value, int line)
{ ++checks; if (!value) { fprintf(stderr,"geometry_classify:%d failed\n",line); exit(1); } }
#define CHECK(value) check((value),__LINE__)
static Bits64 pack(double value) { union { double d; Bits64 u; } v; v.d=value; return v.u; }
static double unpack(Bits64 value) { union { double d; Bits64 u; } v; v.u=value; return v.d; }
static void event(unsigned kind) { CHECK(event_count<128); events[event_count++]=kind; }
Bits64 func_00374848(float value)
{
    event(1); CHECK(conversion_count<32); conversions[conversion_count++]=value;
    if (conversion_count==1 && mutate_frame) {
        mutate_frame->position.x=1000;
        mutate_frame->axis[1].w=6000;
        mutate_face->distance=-1000;
    }
    return pack(value);
}
s32 func_00373250(Bits64 left, Bits64 right)
{ double a=unpack(left),b=unpack(right); event(2); return a<b?-1:a>b?1:0; }
Bits64 func_00372CC0(Bits64 left, Bits64 right)
{ event(3); return pack(unpack(left)-unpack(right)); }
Bits64 func_00372C68(Bits64 left, Bits64 right)
{ event(4); return pack(unpack(left)+unpack(right)); }
float func_003734F8(Bits64 value) { event(5); return (float)unpack(value); }
static void reset_calls(void)
{ conversion_count=event_count=0;mutate_frame=0;mutate_face=0; }

static u32 box_model(const GeorgeGeometryFace *faces, const GeorgeBounds *box, unsigned count)
{
    unsigned face, corner; int crossing=0;
    for (face=0;face<count;++face) {
        double low=1e100,high=-1e100;
        for (corner=0;corner<8;++corner) {
            double x=(corner&1)?box->upper.x:box->lower.x;
            double y=(corner&2)?box->upper.y:box->lower.y;
            double z=(corner&4)?box->upper.z:box->lower.z;
            double value=x*faces[face].normal.x+y*faces[face].normal.y+z*faces[face].normal.z;
            if(value<low)low=value;if(value>high)high=value;
        }
        if(low>faces[face].distance)return 0;
        if(face+1<count && high>=faces[face].distance)crossing=1;
    }
    return crossing?2:1;
}
static u32 sphere_model(const GeorgeGeometryFace *faces, const GeorgeMathVec4 *sphere)
{
    unsigned face;int crossing=0;
    for(face=0;face<5;++face) {
        double d=sphere->x*faces[face].normal.x+sphere->y*faces[face].normal.y+
                 sphere->z*faces[face].normal.z-faces[face].distance;
        if(d>sphere->w)return 0;
        if(face<4 && d>-sphere->w)crossing=1;
    }
    return crossing?2:1;
}
static unsigned seed=0x87296413u;
static float integer(void) { seed=seed*1664525u+1013904223u;return (float)((int)(seed%9)-4); }
static void box_and_sphere(void)
{
    GeorgeGeometryFace faces[6];GeorgeBounds box;GeorgeMathVec4 sphere;
    unsigned i,j,k;memset(faces,0,sizeof faces);
    for(i=0;i<4096;++i) {
        box.lower.x=integer();box.lower.y=integer();box.lower.z=integer();
        box.upper.x=box.lower.x+fabsf(integer());box.upper.y=box.lower.y+fabsf(integer());
        box.upper.z=box.lower.z+fabsf(integer());
        sphere.x=integer();sphere.y=integer();sphere.z=integer();sphere.w=fabsf(integer());
        for(j=0;j<6;++j) {
            faces[j].normal.x=integer();faces[j].normal.y=integer();faces[j].normal.z=integer();
            faces[j].distance=integer()*4;faces[j].vertex.x=integer();
        }
        CHECK(func_002A3CA0(faces,&box)==box_model(faces,&box,5));
        CHECK(func_002A4520(faces,&box)==box_model(faces,&box,6));
        CHECK(func_002A4760(faces,&sphere)==sphere_model(faces,&sphere));
    }
    memset(&box,0,sizeof box);memset(&sphere,0,sizeof sphere);
    for(k=5;k<=6;++k)for(i=0;i<k;++i)for(j=0;j<3;++j) {
        unsigned f;for(f=0;f<6;++f){faces[f].normal.x=faces[f].normal.y=faces[f].normal.z=0;faces[f].distance=1;}
        faces[i].distance=(float)((int)j-1);
        CHECK((k==5?func_002A3CA0(faces,&box):func_002A4520(faces,&box))==
              (j==0?0u:(j==1 && i+1<k)?2u:1u));
    }
    for(i=0;i<5;++i) {
        memset(faces,0,sizeof faces);for(j=0;j<5;++j)faces[j].distance=1;
        faces[i].distance=0;
        CHECK(func_002A4760(faces,&sphere)==1); /* Strict negative-radius test. */
        sphere.w=1;CHECK(func_002A4760(faces,&sphere)==(i<4?2u:1u));sphere.w=0;
    }
}
static double dot(const GeorgeMathVec4 *axis,const GeorgeMathVec3 *normal)
{ return axis->x*normal->x+axis->y*normal->y+axis->z*normal->z; }
static u32 frame_model(const GeorgeGeometryFrame *frame,const GeorgeGeometryFace *face)
{
    double distance=dot(&frame->position,&face->normal)-face->distance;
    double radius=fabs(dot(&frame->axis[0],&face->normal)*frame->axis[0].w)+
                  fabs(dot(&frame->axis[1],&face->normal)*frame->axis[1].w)+
                  fabs(dot(&frame->axis[2],&face->normal)*frame->axis[2].w);
    return fabs(distance)<=radius?0:distance<0?1:2;
}
static void frames(void)
{
    GeorgeGeometryFrame frame;GeorgeGeometryFace faces[5];unsigned i,j;
    for(i=0;i<512;++i) {
        for(j=0;j<3;++j){frame.axis[j].x=integer();frame.axis[j].y=integer();
            frame.axis[j].z=integer();frame.axis[j].w=integer();}
        frame.position.x=integer();frame.position.y=integer();frame.position.z=integer();
        for(j=0;j<5;++j){faces[j].normal.x=integer();faces[j].normal.y=integer();
            faces[j].normal.z=integer();faces[j].distance=integer()*32;}
        for(j=0;j<5;++j){reset_calls();CHECK(func_0029E098(&frame,faces+j)==frame_model(&frame,faces+j));}
        { unsigned f;u32 expected=1;for(f=0;f<5;++f){u32 result=frame_model(&frame,faces+f);
            if(result==2){expected=0;break;}if(result==0 && f<4)expected=2;}
          reset_calls();CHECK(func_002A4678(faces,&frame)==expected); }
    }
    memset(&frame,0,sizeof frame);memset(faces,0,sizeof faces);
    frame.axis[0].x=-2;frame.axis[0].w=1;
    frame.axis[1].x=4;frame.axis[1].w=1;
    frame.axis[2].x=-3;frame.axis[2].w=1;
    frame.position.x=6;faces[0].normal.x=1;
    reset_calls();mutate_frame=&frame;mutate_face=faces;
    CHECK(func_0029E098(&frame,faces)==0);
    CHECK(conversion_count==5);
    CHECK(conversions[0]==-2 && conversions[1]==4 && conversions[2]==-3);
    CHECK(conversions[3]==6 && conversions[4]==9);
    CHECK(frame.position.x==1000 && frame.axis[1].w==6000 && faces[0].distance==-1000);
    { const unsigned expected[]={1,2,3,1,2,4,1,2,3,4,5,1,2,1,2};
      CHECK(event_count==sizeof expected/sizeof expected[0]);
      for(j=0;j<event_count;++j)CHECK(events[j]==expected[j]); }
    for(i=0;i<5;++i) {
        memset(&frame,0,sizeof frame);memset(faces,0,sizeof faces);
        for(j=0;j<5;++j)faces[j].distance=1;
        faces[i].distance=0;reset_calls();CHECK(func_002A4678(faces,&frame)==(i<4?2u:1u));
        CHECK(conversion_count==25);
    }
}
int main(void)
{
    CHECK(sizeof(void *)==4 && sizeof(Bits64)==8);
    box_and_sphere();frames();
    printf("geometry_classify: %u checks passed\n",checks);return 0;
}
