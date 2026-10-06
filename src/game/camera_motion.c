#include "george/camera_motion.h"

extern void *func_002AEC28(u32 size);

static __inline__ void translate(GeorgeCameraMotionTransform *transform,
                                 const GeorgeMathVec3 *delta, float scale)
{
    float y, z;
    transform->field04.x += delta->x * scale;
    y = transform->field04.y;
    z = transform->field04.z;
    transform->field04.y = y + delta->y * scale;
    transform->field04.z = z + delta->z * scale;
}

void func_002B59F8(GeorgeCameraMotion *motion, float elapsed)
{
    GeorgeRotationMatrix inverse, forward;
    GeorgeMathVec3 delta, first, second;
    GeorgeCameraMotionTransform *transform;
    GeorgeInputState *input;
    float pitch = 0.0f, yaw = 0.0f, speed = 0.0f, scale, angle;
    u32 held;

    elapsed = george_ee_maximum(elapsed,0.016666667535901069f);
    transform = motion->transform;
    delta.x = delta.y = delta.z = 0.0f;
    func_0029A308(transform,&inverse,&forward);
    input = motion->input;
    held = input->held;

    if ((held & 0x01400000u) == 0x01400000u) {
        float amount;
        first.x = 0.0f; first.y = 1.0f; first.z = 0.0f;
        scale = transform->field30 * 0.11999999731779099f;
        amount = input->axes[0][1].delta * scale;
        delta.y = amount * first.y + delta.y;
        delta.x = amount * first.x + delta.x;
    } else if (held & 0x00800000u) {
        float axis = -input->axes[0][1].delta;
        yaw = input->axes[0][0].delta * 0.75f;
        pitch = axis * 0.75f;
    } else if (held & 0x00400000u) {
        float axis = -input->axes[0][1].delta;
        speed = (axis + axis) * 5.0f;
    } else if (held & 0x01000000u) {
        float amount, x, y, z;
        first.x = forward.element[0]; first.z = forward.element[2];
        second.x = forward.element[8]; second.z = forward.element[10];
        first.y = second.y = 0.0f;
        func_002A3538(&first);
        func_002A3538(&second);
        scale = transform->field30 * 0.11999999731779099f;
        input = motion->input;
        amount = input->axes[0][0].delta * scale;
        z = amount * first.z + delta.z;
        x = amount * first.x + delta.x;
        y = amount * first.y + delta.y;
        delta.z = z; delta.x = x; delta.y = y;
        scale = transform->field30 * 0.11999999731779099f;
        amount = -input->axes[0][1].delta * scale;
        delta.z = amount * second.z + z;
        delta.x = amount * second.x + x;
        delta.y = amount * second.y + y;
    }

    transform->field00 = 0;
    translate(transform,&delta,elapsed * 5.0f);
    angle = transform->field34 + (yaw + yaw) * elapsed;
    transform->field34 = angle;
    transform->field30 += (speed + speed) * elapsed;
    transform->field38 += (pitch + pitch) * elapsed;
    if (angle < 0.0f) angle += 6.2831854820251465f;
    else if (!(angle <= 6.2831854820251465f)) angle -= 6.2831854820251465f;
    transform->field34 = angle;
}

void func_002B5D08(GeorgeCameraMotion *motion, float elapsed)
{
    GeorgeRotationMatrix inverse, forward;
    GeorgeMathVec3 delta, first, second;
    GeorgeCameraMotionTransform *transform = motion->transform;
    GeorgeInputState *input;
    float amount, x, y, z, yaw, pitch;
    func_0029A308(transform,&inverse,&forward);
    first.x = forward.element[0]; first.z = forward.element[2];
    second.x = forward.element[8]; second.y = forward.element[9];
    second.z = forward.element[10]; first.y = forward.element[1];
    delta.x = delta.y = delta.z = 0.0f;
    func_002A3538(&first);
    func_002A3538(&second);
    input = motion->input;
    amount = input->axes[0][0].current * 0.10000000149011612f;
    z = amount * first.z + delta.z;
    x = amount * first.x + delta.x;
    y = amount * first.y + delta.y;
    delta.z = z; delta.x = x; delta.y = y;
    amount = input->axes[0][1].current * 0.10000000149011612f;
    delta.z = amount * second.z + z;
    delta.x = amount * second.x + x;
    delta.y = amount * second.y + y;
    pitch = -input->axes[1][1].current;
    yaw = -input->axes[1][0].current;
    pitch += pitch; yaw += yaw;
    transform->field00 = 0;
    transform->field30 = 0.10000000149011612f;
    translate(transform,&delta,elapsed);
    transform->field34 += yaw * elapsed;
    transform->field38 += pitch * elapsed;
}

GeorgeCameraMotion *func_002B5EA8(GeorgeCameraMotionTransform *transform,
                                GeorgeInputState *input)
{
    GeorgeCameraMotion *motion = (GeorgeCameraMotion *)func_002AEC28(12);
    if (motion != NULL) {
        motion->transform = transform;
        motion->input = input;
        motion->mode = 0;
    }
    return motion;
}

void func_002B5F10(GeorgeCameraMotion *motion, s32 mode)
{
    motion->mode = mode;
}

void func_002B5F18(GeorgeCameraMotion *motion, float elapsed)
{
    if (motion->mode == 1) func_002B5D08(motion,elapsed);
    else func_002B59F8(motion,elapsed);
}
