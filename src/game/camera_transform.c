#include "george/camera_transform.h"

extern void *func_002AEC28(u32 size);
extern s32 func_00299BE0(GeorgeCameraMotionTransform *transform);
extern void func_00299D68(GeorgeCameraMotionTransform *transform,
                         GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward);
extern void func_002A1E78(GeorgeRotationMatrix *output,const GeorgeMathVec4 *rotation);
extern float func_0029C168(float angle);
extern float func_0029C090(float angle);

GeorgeCameraTransform *func_0029A100(float x,float y,float z)
{
    GeorgeCameraTransform *transform=(GeorgeCameraTransform *)func_002AEC28(0x68);
    if (transform != NULL) func_0029A188(transform,x,y,z);
    return transform;
}

void func_0029A188(GeorgeCameraTransform *transform,float x,float y,float z)
{
    GeorgeCameraPose *pose=&transform->pose.fields;
    pose->field00=0;
    pose->field04.x=x;
    pose->field04.z=z;
    pose->field04.y=y;
    pose->field10.x=0.0f;
    pose->field10.z=0.0f;
    pose->field10.y=0.0f;
    pose->field2C=0.0f;
    pose->field30=1000.0f;
    pose->field34=0.0f;
    pose->field38=0.0f;
    transform->field3C=1.0f;
    transform->field40=4000.0f;
    transform->field44=34.515998840332031f;
    transform->field48=1.3333330154418945f;
    pose->field1C.x=0.0f;
    pose->field1C.y=0.0f;
    pose->field1C.z=0.0f;
    pose->field1C.w=0.0f;
    func_00299BE0(&transform->pose.prefix);
    transform->field64=0;
    transform->field50=0;
    transform->field4C=0;
    transform->field54.x=0.0f;
    transform->field60=0;
    transform->field54.z=0.0f;
    transform->field54.y=0.0f;
}

void func_0029A308(GeorgeCameraMotionTransform *transform,
                  GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward)
{
    if (func_00299BE0(transform)) {
        GeorgeCameraPose *pose=(GeorgeCameraPose *)transform;
        float z;
        func_002A1E78(forward,&pose->field1C);
        forward->element[12]=pose->field10.x;
        forward->element[13]=pose->field10.y;
        z=pose->field10.z;
        forward->element[15]=1.0f;
        forward->element[14]=z;
        func_002A0E20(inverse,forward);
    } else func_00299D68(transform,inverse,forward);
}

void func_0029A3A0(const GeorgeCameraTransform *transform,GeorgeRotationMatrix *output)
{
    float angle=transform->field44*0.0087266471236944199f;
    float near_value=transform->field3C,far_value=transform->field40;
    float first=func_0029C168(angle);
    float second=func_0029C090(angle);
    float ratio=first/second;
    float horizontal=ratio/transform->field48;
    float sum,difference,depth;
    output->element[1]=0.0f;
    output->element[2]=0.0f;
    output->element[3]=0.0f;
    output->element[0]=horizontal;
    output->element[4]=0.0f;
    output->element[5]=ratio;
    output->element[6]=0.0f;
    output->element[7]=0.0f;
    output->element[8]=0.0f;
    sum=far_value+near_value;
    output->element[9]=0.0f;
    difference=far_value-near_value;
    output->element[11]=-1.0f;
    output->element[10]=-sum/difference;
    output->element[12]=0.0f;
    output->element[15]=0.0f;
    far_value=far_value*-2.0f;
    output->element[13]=0.0f;
    depth=near_value*far_value;
    output->element[14]=depth/difference;
}

void func_0029A498(GeorgeCameraTransform *transform,const GeorgeMathVec3 *delta)
{
    GeorgeCameraPose *pose=&transform->pose.fields;
    float x=delta->x,old_x=pose->field10.x;
    float y=delta->y,z=delta->z;
    float old_y=pose->field10.y,old_z=pose->field10.z;
    pose->field10.x=old_x+x;
    pose->field10.y=old_y+y;
    pose->field10.z=old_z+z;
    x=delta->x;old_x=pose->field04.x;
    z=delta->z;y=delta->y;
    old_y=pose->field04.y;old_z=pose->field04.z;
    pose->field04.x=old_x+x;
    pose->field04.y=old_y+y;
    pose->field04.z=old_z+z;
}
