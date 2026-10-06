#include "george/actor_pose_controller.h"

extern const u8 D_0042B078[], D_0042BD08[];
extern const char D_0042B048[], D_0042B960[];
extern void *func_0023C298(const char *, s32);
extern void func_0029A260(GeorgeCameraTransform *, const GeorgeCameraTransform *);
extern void func_0029A168(void *);
extern void func_002A1C08(void *, const void *);
extern void func_002A1C30(GeorgeRotationMatrix *);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))
#define TRANSFORM(object) FIELD(object, 8, GeorgeCameraTransform *)

void *func_00166CA8(void *object, const GeorgeRotationMatrix *input)
{
    float x, y, z;
    GeorgeCameraTransform *transform;
    func_002D06E8((GeorgeScriptObject *)object);
    FIELD(object, 4, const u8 *) = D_0042B078;
    FIELD(object, 0xE0, void *) = func_0023C298(D_0042B048, -1);
    x = input->element[12];
    y = input->element[13];
    z = input->element[14];
    transform = func_0029A100(x, y, z);
    FIELD(object, 0xC, u32) = 0;
    FIELD(object, 8, GeorgeCameraTransform *) = transform;
    func_002A1C08(ADDRESS(object, 0x10), input);
    func_002A1C30(FRAME(object, 0x50));
    func_002A1C30(FRAME(object, 0x90));
    FIELD(object, 0xD0, u32) = 0;
    FIELD(object, 0xD4, u32) = 0;
    FIELD(object, 0xD8, u32) = 0;
    FIELD(object, 0xDC, u32) = 0;
    transform = TRANSFORM(object);
    transform->field3C = FIELD(FIELD(object, 0xE0, void *), 4, float);
    transform = TRANSFORM(object);
    transform->field44 = FIELD(FIELD(object, 0xE0, void *), 0, float) / transform->field48;
    func_0029A308(&TRANSFORM(object)->pose.prefix, FRAME(object, 0x50), FRAME(object, 0x90));
    return object;
}

void *func_00166DD0(void *object)
{
    GeorgeCameraTransform *transform;
    func_002D06E8((GeorgeScriptObject *)object);
    FIELD(object, 4, const u8 *) = D_0042B078;
    FIELD(object, 0xE0, void *) = func_0023C298(D_0042B048, -1);
    transform = func_0029A100(0.0f, 0.0f, 0.0f);
    FIELD(object, 0xC, u32) = 0;
    FIELD(object, 8, GeorgeCameraTransform *) = transform;
    func_002A1C30(FRAME(object, 0x10));
    func_002A1C30(FRAME(object, 0x50));
    func_002A1C30(FRAME(object, 0x90));
    FIELD(object, 0xD4, float) = 0.0f;
    FIELD(object, 0xD0, float) = 0.0f;
    FIELD(object, 0xD8, u32) = 0;
    FIELD(object, 0xDC, u32) = 0;
    transform = TRANSFORM(object);
    transform->field3C = FIELD(FIELD(object, 0xE0, void *), 4, float);
    transform = TRANSFORM(object);
    transform->field44 = FIELD(FIELD(object, 0xE0, void *), 0, float) / transform->field48;
    func_0029A308(&TRANSFORM(object)->pose.prefix, FRAME(object, 0x50), FRAME(object, 0x90));
    return object;
}

void *func_00166EB8(void *object, const void *source)
{
    GeorgeCameraTransform *transform;
    func_002D06E8((GeorgeScriptObject *)object);
    FIELD(object, 4, const u8 *) = D_0042B078;
    transform = func_0029A100(0.0f, 0.0f, 0.0f);
    FIELD(object, 8, GeorgeCameraTransform *) = transform;
    func_0029A260(transform, TRANSFORM(source));
    FIELD(object, 0xC, u32) = FIELD(source, 0xC, u32);
    func_002A1C08(ADDRESS(object, 0x10), ADDRESS(source, 0x10));
    func_002A1C08(ADDRESS(object, 0x50), ADDRESS(source, 0x50));
    func_002A1C08(ADDRESS(object, 0x90), ADDRESS(source, 0x90));
    FIELD(object, 0xD0, float) = FIELD(source, 0xD0, float);
    FIELD(object, 0xD4, float) = FIELD(source, 0xD4, float);
    return object;
}

void func_00166F58(void *object, u32 flags)
{
    void *component;
    FIELD(object, 4, const u8 *) = D_0042B078;
    if (TRANSFORM(object) != NULL) func_0029A168(TRANSFORM(object));
    component = FIELD(object, 0xC, void *);
    if (component != NULL) {
        const GeorgeGoalVirtualWord *method =
            (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(component, 4, void *), 8);
        method->invoke((void *)ADDRESS(component, (s32)method->adjustment), 3);
    }
    func_002D0700((GeorgeScriptObject *)object, flags);
}

void *func_0016B948(void *object, const GeorgeRotationMatrix *input)
{
    void *resource;
    GeorgeMathVec3 first, second;
    GeorgeCameraTransform *transform;
    float ax, ay, az, bx, tx;
    float scale, x, y, z;
    func_00166CA8(object, input);
    FIELD(object, 0x10C, u32) = 0;
    FIELD(object, 4, const u8 *) = D_0042BD08;
    resource = func_0023C298(D_0042B960, -1);
    FIELD(object, 0x10C, void *) = resource;
    FIELD(object, 0x108, float) = FIELD(resource, 0x18, float);
    FIELD(object, 0xF0, float) = FIELD(resource, 0, float);
    FIELD(object, 0xF4, float) = FIELD(resource, 4, float);
    FIELD(object, 0xF8, float) = FIELD(resource, 8, float);
    resource = FIELD(object, 0x10C, void *);
    FIELD(object, 0xFC, float) = FIELD(resource, 0xC, float);
    FIELD(object, 0x100, float) = FIELD(resource, 0x10, float);
    FIELD(object, 0x104, float) = FIELD(resource, 0x14, float);
    FIELD(TRANSFORM(object), 0, u32) = 2;

    scale = FIELD(object, 0x104, float);
    ay = FIELD(object, 0x34, float); az = FIELD(object, 0x38, float);
    ax = FIELD(object, 0x30, float);
    tx = FIELD(object, 0x40, float);
    y = FIELD(object, 0x44, float) + scale * ay;
    z = FIELD(object, 0x48, float) + scale * az;
    x = tx + scale * ax;
    scale = FIELD(object, 0x100, float);
    bx = FIELD(object, 0x20, float);
    x = x + scale * bx;
    y = y + scale * FIELD(object, 0x24, float);
    z = z + scale * FIELD(object, 0x28, float);
    scale = FIELD(object, 0xFC, float);
    x = x + scale * FIELD(object, 0x10, float);
    y = y + scale * FIELD(object, 0x14, float);
    z = z + scale * FIELD(object, 0x18, float);
    first.x = x; first.y = y; first.z = z;

    scale = FIELD(object, 0xF8, float);
    second.x = tx + scale * ax;
    second.z = FIELD(object, 0x48, float) + scale * FIELD(object, 0x38, float);
    second.y = FIELD(object, 0x44, float) + scale * FIELD(object, 0x34, float);
    scale = FIELD(object, 0xF4, float);
    second.x = second.x + scale * bx;
    second.z = second.z + scale * FIELD(object, 0x28, float);
    second.y = second.y + scale * FIELD(object, 0x24, float);
    scale = FIELD(object, 0xF0, float);
    second.x = second.x + scale * FIELD(object, 0x10, float);
    second.z = second.z + scale * FIELD(object, 0x18, float);
    second.y = second.y + scale * FIELD(object, 0x14, float);
    /* All target components are computed before the potentially aliased output
     * writes; the second transform pointer is reloaded after the first vector. */
    transform = TRANSFORM(object);
    transform->pose.fields.field04.x = first.x;
    transform->pose.fields.field04.y = first.y;
    transform->pose.fields.field04.z = first.z;
    transform = TRANSFORM(object);
    transform->pose.fields.field10.x = second.x;
    transform->pose.fields.field10.y = second.y;
    transform->pose.fields.field10.z = second.z;
    return object;
}

void func_0016BDA0(void *object, float elapsed)
{
    void *resource;
    GeorgeMathVec3 first, second, interpolated_first, interpolated_second;
    GeorgeCameraTransform *transform;
    float duration, scale, ratio;
    float ax, bx, cx, tx, x, y, z;
    if (FIELD(object, 0xD8, u32) != 0) return;
    resource = FIELD(object, 0x10C, void *);
    FIELD(object, 0xF0, float) = FIELD(resource, 0, float);
    FIELD(object, 0xF4, float) = FIELD(resource, 4, float);
    FIELD(object, 0xF8, float) = FIELD(resource, 8, float);
    resource = FIELD(object, 0x10C, void *);
    FIELD(object, 0xFC, float) = FIELD(resource, 0xC, float);
    FIELD(object, 0x100, float) = FIELD(resource, 0x10, float);
    FIELD(object, 0x104, float) = FIELD(resource, 0x14, float);
    resource = FIELD(object, 0x10C, void *);
    duration = FIELD(resource, 0x18, float);
    scale = FIELD(object, 0x104, float);
    FIELD(object, 0x108, float) = duration;
    ax = FIELD(object, 0x30, float);
    tx = FIELD(object, 0x40, float);
    x = tx + scale * ax;
    y = FIELD(object, 0x44, float) + scale * FIELD(object, 0x34, float);
    z = FIELD(object, 0x48, float) + scale * FIELD(object, 0x38, float);
    scale = FIELD(object, 0x100, float);
    bx = FIELD(object, 0x20, float);
    x = x + scale * bx;
    y = y + scale * FIELD(object, 0x24, float);
    z = z + scale * FIELD(object, 0x28, float);
    scale = FIELD(object, 0xFC, float);
    cx = FIELD(object, 0x10, float);
    x = x + scale * cx;
    y = y + scale * FIELD(object, 0x14, float);
    z = z + scale * FIELD(object, 0x18, float);
    first.x = x; first.y = y; first.z = z;
    scale = FIELD(object, 0xF8, float);
    second.x = tx + scale * ax;
    second.z = FIELD(object, 0x48, float) + scale * FIELD(object, 0x38, float);
    second.y = FIELD(object, 0x44, float) + scale * FIELD(object, 0x34, float);
    scale = FIELD(object, 0xF4, float);
    second.x = second.x + scale * bx;
    second.z = second.z + scale * FIELD(object, 0x28, float);
    second.y = second.y + scale * FIELD(object, 0x24, float);
    scale = FIELD(object, 0xF0, float);
    second.x = second.x + scale * cx;
    second.z = second.z + scale * FIELD(object, 0x18, float);
    second.y = second.y + scale * FIELD(object, 0x14, float);
    if (0.0f < duration) {
        float old_x, old_y, old_z;
        transform = TRANSFORM(object);
        ratio = elapsed / duration;
        old_x = transform->pose.fields.field10.x;
        old_z = transform->pose.fields.field10.z;
        old_y = transform->pose.fields.field10.y;
        interpolated_second.x = old_x + ratio * (second.x - old_x);
        interpolated_second.z = old_z + ratio * (second.z - old_z);
        interpolated_second.y = old_y + ratio * (second.y - old_y);
        old_x = transform->pose.fields.field04.x;
        old_z = transform->pose.fields.field04.z;
        old_y = transform->pose.fields.field04.y;
        interpolated_first.x = old_x + ratio * (first.x - old_x);
        interpolated_first.z = old_z + ratio * (first.z - old_z);
        interpolated_first.y = old_y + ratio * (first.y - old_y);
    } else {
        interpolated_second = second;
        interpolated_first = first;
    }
    transform = TRANSFORM(object);
    transform->pose.fields.field04.x = interpolated_first.x;
    transform->pose.fields.field04.y = interpolated_first.y;
    transform->pose.fields.field04.z = interpolated_first.z;
    transform = TRANSFORM(object);
    transform->pose.fields.field10.x = interpolated_second.x;
    transform->pose.fields.field10.y = interpolated_second.y;
    transform->pose.fields.field10.z = interpolated_second.z;
    FIELD(object, 0x80, float) = interpolated_second.x;
    FIELD(object, 0x84, float) = interpolated_second.y;
    FIELD(object, 0x88, float) = interpolated_second.z;
    FIELD(object, 0x8C, float) = 1.0f;
    func_0029A308(&TRANSFORM(object)->pose.prefix, FRAME(object, 0x50), FRAME(object, 0x90));
}
