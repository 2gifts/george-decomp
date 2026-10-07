#include "george/geometry_frustum.h"

/* Reuse the reviewed ordinary-C cross and dot expression shapes from
 * geometry.c. Each complete output normal is followed by the real normalizer.
 * The sixth vertex and opposite plane retain their distinct store sequence. */
#define NORMAL_COMPONENTS(first, second, origin) \
    float ax=(first).x-(origin).x; \
    float ay=(first).y-(origin).y; \
    float az=(first).z-(origin).z; \
    float bx=(second).x-(origin).x; \
    float by=(second).y-(origin).y; \
    float bz=(second).z-(origin).z; \
    float nx=ay*bz-az*by; \
    float ny=az*bx-ax*bz; \
    float nz=ax*by-ay*bx

#define PLANE_DISTANCE(face, point) \
    (((face).normal.x*(point).x+(face).normal.y*(point).y)+(face).normal.z*(point).z)

void func_002A3DF8(GeorgeGeometrySixFaces *output, const GeorgeGeometryFrame *frame,
                  float near_depth, float far_depth, float width, float height)
{
    GeorgeMathVec3 forward, horizontal, vertical;
    float x, y, z;
    /* All scaled axes precede output writes; position is deliberately fresh. */
    forward.x=frame->axis[2].x*far_depth;
    forward.y=frame->axis[2].y*far_depth;
    forward.z=frame->axis[2].z*far_depth;
    horizontal.x=frame->axis[0].x*width;
    horizontal.y=frame->axis[0].y*width;
    horizontal.z=frame->axis[0].z*width;
    vertical.x=frame->axis[1].x*height;
    vertical.y=frame->axis[1].y*height;
    vertical.z=frame->axis[1].z*height;
    output->faces[0].vertex.x=frame->position.x;
    output->faces[0].vertex.y=frame->position.y;
    output->faces[0].vertex.z=frame->position.z;

    x=((frame->position.x-forward.x)-horizontal.x)-vertical.x;
    y=((frame->position.y-forward.y)-horizontal.y)-vertical.y;
    z=((frame->position.z-forward.z)-horizontal.z)-vertical.z;
    output->faces[1].vertex.x=x;output->faces[1].vertex.y=y;output->faces[1].vertex.z=z;
    x=((frame->position.x-forward.x)+horizontal.x)-vertical.x;
    y=((frame->position.y-forward.y)+horizontal.y)-vertical.y;
    z=((frame->position.z-forward.z)+horizontal.z)-vertical.z;
    output->faces[2].vertex.x=x;output->faces[2].vertex.y=y;output->faces[2].vertex.z=z;
    x=((frame->position.x-forward.x)+horizontal.x)+vertical.x;
    y=((frame->position.y-forward.y)+horizontal.y)+vertical.y;
    z=((frame->position.z-forward.z)+horizontal.z)+vertical.z;
    output->faces[3].vertex.x=x;output->faces[3].vertex.y=y;output->faces[3].vertex.z=z;
    x=((frame->position.x-forward.x)-horizontal.x)+vertical.x;
    y=((frame->position.y-forward.y)-horizontal.y)+vertical.y;
    z=((frame->position.z-forward.z)-horizontal.z)+vertical.z;
    output->faces[4].vertex.x=x;output->faces[4].vertex.y=y;output->faces[4].vertex.z=z;

    /* Temporary scaled-axis publication is visible to aliased frame loads.
     * Y/Z remain captured, whereas X is freshly read from the output. */
    output->faces[5].vertex.x=frame->axis[2].x*near_depth;
    y=frame->axis[2].y*near_depth;
    output->faces[5].vertex.y=y;
    z=frame->axis[2].z*near_depth;
    output->faces[5].vertex.z=z;
    x=frame->position.x-output->faces[5].vertex.x;
    z=frame->position.z-z;
    y=frame->position.y-y;
    output->faces[5].vertex.x=x;
    output->faces[5].vertex.z=z;
    output->faces[5].vertex.y=y;

    {
        NORMAL_COMPONENTS(output->faces[4].vertex,output->faces[1].vertex,output->faces[0].vertex);
        output->faces[0].normal.x=nx;output->faces[0].normal.y=ny;output->faces[0].normal.z=nz;
    }
    func_002A3538(&output->faces[0].normal);
    output->faces[0].distance=PLANE_DISTANCE(output->faces[0],output->faces[0].vertex);
    {
        NORMAL_COMPONENTS(output->faces[2].vertex,output->faces[3].vertex,output->faces[0].vertex);
        output->faces[1].normal.x=nx;output->faces[1].normal.y=ny;output->faces[1].normal.z=nz;
    }
    func_002A3538(&output->faces[1].normal);
    output->faces[1].distance=PLANE_DISTANCE(output->faces[1],output->faces[0].vertex);
    {
        NORMAL_COMPONENTS(output->faces[1].vertex,output->faces[2].vertex,output->faces[0].vertex);
        output->faces[2].normal.x=nx;output->faces[2].normal.y=ny;output->faces[2].normal.z=nz;
    }
    func_002A3538(&output->faces[2].normal);
    output->faces[2].distance=PLANE_DISTANCE(output->faces[2],output->faces[0].vertex);
    {
        NORMAL_COMPONENTS(output->faces[3].vertex,output->faces[4].vertex,output->faces[0].vertex);
        output->faces[3].normal.x=nx;output->faces[3].normal.y=ny;output->faces[3].normal.z=nz;
    }
    func_002A3538(&output->faces[3].normal);
    output->faces[3].distance=PLANE_DISTANCE(output->faces[3],output->faces[0].vertex);
    {
        NORMAL_COMPONENTS(output->faces[2].vertex,output->faces[4].vertex,output->faces[3].vertex);
        output->faces[4].normal.x=nx;output->faces[4].normal.y=ny;output->faces[4].normal.z=nz;
    }
    func_002A3538(&output->faces[4].normal);
    output->faces[5].normal.x=output->faces[4].normal.x;
    output->faces[4].distance=PLANE_DISTANCE(output->faces[4],output->faces[3].vertex);
    output->faces[5].normal.y=output->faces[4].normal.y;
    output->faces[5].normal.z=output->faces[4].normal.z;
    output->faces[5].normal.x=-output->faces[5].normal.x;
    y=output->faces[5].normal.y;
    z=output->faces[5].normal.z;
    y=-y;z=-z;
    output->faces[5].normal.y=y;
    output->faces[5].normal.z=z;
    output->faces[5].distance=PLANE_DISTANCE(output->faces[5],output->faces[5].vertex);
}

#undef PLANE_DISTANCE
#undef NORMAL_COMPONENTS
