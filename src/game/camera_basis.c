#include "george/camera_basis.h"

typedef unsigned long long CameraBits64;
typedef char camera_basis_bits64_size[(sizeof(CameraBits64)==8)?1:-1];

extern float func_0029C090(float angle);
extern float func_0029C168(float angle);
extern float func_0029B940(float first,float second);
extern CameraBits64 func_00374848(float value);
extern s32 func_00373250(CameraBits64 first,CameraBits64 second);
extern CameraBits64 func_00372CC0(CameraBits64 first,CameraBits64 second);
extern void func_002A1098(GeorgeRotationMatrix *output,
                         const GeorgeRotationMatrix *input);

s32 func_00299BE0(GeorgeCameraMotionTransform *transform)
{
    GeorgeCameraPose *pose=(GeorgeCameraPose *)transform;
    float sin_pitch,cos_pitch,sin_yaw,cos_yaw,x,y,z;
    float old_x,old_y,old_z,radius;
    switch (pose->field00) {
    case 0:
        sin_pitch=func_0029C090(pose->field38);
        cos_pitch=func_0029C168(pose->field38);
        sin_yaw=func_0029C090(pose->field34);
        cos_yaw=func_0029C168(pose->field34);
        radius=pose->field30;
        old_x=pose->field04.x;
        sin_pitch=radius*sin_pitch;
        y=radius*cos_pitch;
        z=sin_pitch*cos_yaw;
        x=sin_pitch*sin_yaw;
        x=x+old_x;
        old_z=pose->field04.z;
        old_y=pose->field04.y;
        z=z+old_z;
        pose->field10.x=x;
        y=y+old_y;
        pose->field10.y=y;
        pose->field10.z=z;
        break;
    case 1:
        sin_pitch=func_0029C090(pose->field38);
        cos_pitch=func_0029C168(pose->field38);
        sin_yaw=func_0029C090(pose->field34);
        cos_yaw=func_0029C168(pose->field34);
        old_x=pose->field04.x;
        cos_yaw=cos_yaw*-0.20000000298023224f;
        sin_yaw=sin_yaw*-0.20000000298023224f;
        y=cos_pitch*-0.20000000298023224f;
        z=sin_pitch*cos_yaw;
        x=sin_pitch*sin_yaw;
        x=x+old_x;
        old_z=pose->field04.z;
        old_y=pose->field04.y;
        z=z+old_z;
        pose->field10.x=x;
        y=y+old_y;
        pose->field10.y=y;
        pose->field10.z=z;
        break;
    case 3:
        return 1;
    default:
        break;
    }
    return 0;
}

void func_00299D68(GeorgeCameraMotionTransform *transform,
                  GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward)
{
    GeorgeCameraPose *pose=(GeorgeCameraPose *)transform;
    GeorgeMathVec3 direction,up,right,reference;
    float length,cos_angle,sin_angle,dot,scale;
    float x,y,z,cross_x,cross_y,cross_z;
    CameraBits64 absolute_y;

    direction.z=pose->field10.z-pose->field04.z;
    direction.y=pose->field10.y-pose->field04.y;
    direction.x=pose->field10.x-pose->field04.x;
    length=func_002A3538(&direction);
    cos_angle=func_0029C168(pose->field2C);
    sin_angle=func_0029C090(pose->field2C);
    absolute_y=func_00374848(direction.y);
    if (func_00373250(absolute_y,0ULL)<0)
        absolute_y=func_00372CC0(0ULL,absolute_y);
    if (func_00373250(absolute_y,0x3FEFAE1480000000ULL)>0) {
        reference.x=0.0f;reference.z=1.0f;reference.y=0.0f;
    } else {
        reference.x=0.0f;reference.y=1.0f;reference.z=0.0f;
    }
    x=direction.x;y=direction.y;z=direction.z;
    up.x=reference.x*cos_angle;
    up.y=reference.y*cos_angle;
    dot=x*reference.x;
    dot=dot+y*reference.y;
    dot=dot+z*reference.z;
    scale=1.0f-cos_angle;
    scale=dot*scale;
    cross_z=x*reference.y-y*reference.x;
    cross_y=z*reference.x-x*reference.z;
    cross_x=y*reference.z-z*reference.y;
    up.z=reference.z*cos_angle;
    x=x*scale;y=y*scale;z=z*scale;
    cross_z=cross_z*sin_angle;
    cross_y=cross_y*sin_angle;
    cross_x=cross_x*sin_angle;
    up.x=(up.x+x)+cross_x;
    up.y=(up.y+y)+cross_y;
    up.z=(up.z+z)+cross_z;
    func_002A3538(&up);
    right.x=up.y*direction.z-up.z*direction.y;
    right.z=up.x*direction.y-up.y*direction.x;
    right.y=up.z*direction.x-up.x*direction.z;
    func_002A3538(&right);
    z=direction.x*right.y-direction.y*right.x;
    x=direction.y*right.z-direction.z*right.y;
    y=direction.z*right.x-direction.x*right.z;
    up.z=z;up.x=x;up.y=y;
    func_002A3538(&up);

    forward->element[1]=right.y;
    forward->element[0]=right.x;
    forward->element[2]=right.z;
    forward->element[4]=up.x;
    forward->element[5]=up.y;
    forward->element[6]=up.z;
    forward->element[8]=direction.x;
    forward->element[9]=direction.y;
    forward->element[10]=direction.z;
    forward->element[12]=pose->field10.x;
    forward->element[13]=pose->field10.y;
    forward->element[14]=pose->field10.z;
    forward->element[3]=0.0f;
    forward->element[15]=1.0f;
    forward->element[11]=0.0f;
    forward->element[7]=0.0f;
    if (pose->field00==2) {
        pose->field30=length;
        pose->field34=func_0029B940(-direction.x,-direction.z);
        pose->field38=func_0029B940((up.x+up.z)*0.5f,up.y);
    }
    func_002A1098(inverse,forward);
}
