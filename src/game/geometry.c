#include "george/geometry.h"

/* Keep the constructor's normalizer calls and observable vertex reloads. The
 * five normals use cross(a-origin,b-origin), in the observed winding order. */
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

void func_002A3640(GeorgeGeometryPyramid *output, const GeorgeGeometryFrame *frame,
    float depth, float width, float height)
{
    GeorgeMathVec3 forward, horizontal, vertical;
    float x, y, z;
    /* All three scaled axes are captured before output writes. */
    forward.x=frame->axis[2].x*depth;
    forward.y=frame->axis[2].y*depth;
    forward.z=frame->axis[2].z*depth;
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

    {
        NORMAL_COMPONENTS(output->faces[4].vertex,output->faces[1].vertex,output->faces[0].vertex);
        output->faces[0].normal.z=nz;output->faces[0].normal.y=ny;output->faces[0].normal.x=nx;
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
        output->faces[4].normal.x=nx;output->faces[4].normal.z=nz;output->faces[4].normal.y=ny;
    }
    func_002A3538(&output->faces[4].normal);
    output->faces[4].distance=PLANE_DISTANCE(output->faces[4],output->faces[3].vertex);
}

#undef PLANE_DISTANCE
#undef NORMAL_COMPONENTS
