#include "george/geometry_bounds.h"
#include "george/heap.h"

void func_0029E7F0(GeorgeGeometryFrame *output, const GeorgeMathVec3 *lower,
                  const GeorgeMathVec3 *upper)
{
    float x=lower->x, z=lower->z, y=lower->y;
    float dx=upper->x-x, dz=upper->z-z, dy=upper->y-y;
    output->position.x=x+dx*0.5f;
    output->position.z=z+dz*0.5f;
    output->position.y=y+dy*0.5f;
    output->axis[0].y=0.0f;
    output->axis[0].z=0.0f;
    output->axis[0].x=dx;
    output->axis[0].w=dx*0.5f;
    func_002A3538((GeorgeMathVec3 *)&output->axis[0]);
    output->axis[1].x=0.0f;
    output->axis[1].z=0.0f;
    output->axis[1].y=dy;
    output->axis[1].w=dy*0.5f;
    func_002A3538((GeorgeMathVec3 *)&output->axis[1]);
    output->axis[2].x=0.0f;
    output->axis[2].y=0.0f;
    output->axis[2].z=dz;
    output->axis[2].w=dz*0.5f;
    func_002A3538((GeorgeMathVec3 *)&output->axis[2]);
}

void func_002A0048(GeorgeBounds *bounds, float amount)
{
    float negative=-amount;
    float x=bounds->lower.x+negative, y=bounds->lower.y+negative;
    float z=bounds->lower.z+negative, high_x=bounds->upper.x+amount;
    bounds->lower.x=x;
    bounds->lower.z=z;
    bounds->lower.y=y;
    bounds->upper.x=high_x;
    bounds->upper.y+=amount;
    bounds->upper.z+=amount;
}

u32 func_002A00A0(const GeorgeBounds *bounds, const GeorgeMathVec3 *point)
{
    if(point->x<bounds->lower.x || bounds->upper.x<point->x) return 0;
    if(point->y<bounds->lower.y || bounds->upper.y<point->y) return 0;
    if(point->z<bounds->lower.z || bounds->upper.z<point->z) return 0;
    return 1;
}

u32 func_002A0138(const GeorgeBounds *left, const GeorgeBounds *right)
{
    if(right->upper.x<left->lower.x || left->upper.x<right->lower.x) return 0;
    if(right->upper.y<left->lower.y || left->upper.y<right->lower.y) return 0;
    if(right->upper.z<left->lower.z || left->upper.z<right->lower.z) return 0;
    return 1;
}

u32 func_002A01D8(void) { return 0; }

GeorgePlaneArray *func_002A01E0(u32 count)
{
    GeorgePlaneArray *array=func_002AEC28((count<<4)|4u);
    if(array!=NULL) array->count=count;
    return array;
}

GeorgeMathVec4 *func_002A0238(GeorgePlaneArray *array, u32 index)
{
    /* Word shift/add wrap in the original address calculation is explicit. */
    return (GeorgeMathVec4 *)((u32)array+((index<<4)+4u));
}

u32 func_002A0248(const GeorgePlaneArray *planes, u32 count, const GeorgeMathVec3 *points)
{
    u32 point_index, plane_index, plane_count;
    if(count==0) return 1;
    plane_count=planes->count;
    for(point_index=0;point_index<count;point_index++) {
        if(plane_count!=0) {
            float x=points->x, y=points->y, z=points->z;
            const GeorgeMathVec4 *plane=planes->plane;
            for(plane_index=0;plane_index<plane_count;plane_index++,plane++) {
                float distance=((plane->x*x+plane->y*y)+plane->z*z)+plane->w;
                if(0.0f<=distance) return 0;
            }
        }
        points++;
    }
    return 1;
}

u32 func_002A02E0(const GeorgePlaneArray *planes, const GeorgeMathVec4 *sphere)
{
    u32 result=1, index, count=planes->count;
    if(count!=0) {
        float radius=sphere->w, x=sphere->x, y=sphere->y, z=sphere->z;
        const GeorgeMathVec4 *plane=planes->plane;
        for(index=0;index<count;index++,plane++) {
            float distance=((plane->x*x+plane->y*y)+plane->z*z)+plane->w;
            if(radius<=distance) return 0;
            if(-radius<=distance) result|=2u;
        }
    }
    return result;
}

void func_002A0370(GeorgeGeometryFrame *output, const GeorgeBounds *bounds)
{
    func_0029E7F0(output,&bounds->lower,&bounds->upper);
}

u32 func_002A0390(const GeorgeMathVec3 *point, const GeorgeMathVec4 *planes, s32 count)
{
    s32 index;
    if(count==0) return 0;
    if(count>0) {
        float x=point->x, y=point->y, z=point->z;
        for(index=0;index<count;index++,planes++) {
            float distance=((x*planes->x+y*planes->y)+z*planes->z)-planes->w;
            if(1.0e-5f<distance) return 0;
        }
    }
    return 1;
}

u32 func_002A0410(const GeorgeMathVec3 *point, const GeorgeMathVec4 *planes,
                  s32 count, float *closest, float tolerance)
{
    s32 index;
    if(count==0) return 0;
    *closest=tolerance;
    for(index=0;index<count;index++,planes++) {
        float distance=((point->x*planes->x+point->y*planes->y)+point->z*planes->z)-planes->w;
        double magnitude;
        float absolute;
        if(tolerance<distance) return 0;
        magnitude=(double)distance;
        if(magnitude<0.0) magnitude=0.0-magnitude;
        absolute=(float)magnitude;
        if(absolute<=*closest) *closest=absolute;
    }
    return 1;
}

void func_002A0E20(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input)
{
    float *o=output->element;
    const float *m=input->element;
    float a=m[5]*m[10]-m[6]*m[9];
    float b,c,determinant,scale;
    float i0,i1,i2,i4,i5,i6,i8,i9,i10,translation;
    o[0]=a;
    o[4]=m[6]*m[8]-m[4]*m[10];
    o[8]=m[4]*m[9]-m[5]*m[8];
    b=m[9]*m[2]-m[10]*m[1];
    o[1]=b;
    o[5]=m[10]*m[0]-m[8]*m[2];
    o[9]=m[8]*m[1]-m[9]*m[0];
    c=m[1]*m[6]-m[2]*m[5];
    o[2]=c;
    o[6]=m[2]*m[4]-m[0]*m[6];
    o[10]=m[0]*m[5]-m[1]*m[4];
    determinant=(m[0]*a+m[4]*b)+m[8]*c;
    scale=determinant!=0.0f?1.0f/determinant:1.0e7f;
    i0=o[0]*scale; i1=o[1]*scale; i2=o[2]*scale;
    i4=o[4]*scale; i5=o[5]*scale; i6=o[6]*scale;
    i8=o[8]*scale; i9=o[9]*scale; i10=o[10]*scale;
    o[0]=i0; o[1]=i1; o[2]=i2;
    o[4]=i4; o[5]=i5; o[6]=i6;
    o[8]=i8; o[9]=i9; o[10]=i10;
    o[12]=-((m[12]*i0+m[13]*i4)+m[14]*i8);
    o[13]=-((m[12]*i1+m[13]*i5)+m[14]*i9);
    translation=(m[12]*i2+m[13]*i6)+m[14]*i10;
    o[3]=0.0f; o[11]=0.0f; o[7]=0.0f;
    o[14]=-translation;
    o[15]=1.0f;
}
