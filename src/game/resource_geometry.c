#include "george/resource_geometry.h"

extern const u8 D_0043A408[];
extern GeorgeResourceRecord *func_00225E80(const void *key,u32 kind);
extern void *func_002AEE60(u32 size);
extern void func_00226BC0(GeorgeResourceRecord *,const void *key,u32 kind,u32 mode,u32 word);
extern void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *);
extern void func_00226D78(GeorgeResourceRecord *);
extern void func_00226FB0(GeorgeResourceRecord *);
extern void func_00226E38(GeorgeResourceRecord *,void (*)(u32,GeorgeResourceRecord *),GeorgeResourceRecord *,GeorgeResourceRecord *);
extern void func_002B62E0(void *);
extern void func_002B6308(void *);
extern void func_002867E8(void *,u32 word);
extern GeorgeResourceRecord *func_0020F6F8(void *,u32 mode,u32 word);
extern void func_0021C508(void *);
extern void func_002A1C60(const GeorgeRotationMatrix *,const GeorgeMathVec4 *,GeorgeMathVec4 *);
extern void func_0029FEF8(GeorgeGeometryFrame *,const void *,const GeorgeRotationMatrix *);

#define R_ADDRESS(object,offset) ((u8 *)((u32)(object)+(u32)(offset)))
#define R_FIELD(object,offset,type) (*(type *)R_ADDRESS(object,offset))
/* Reuse complete repeated tails as ordinary C. GCC2.9 otherwise outlines
 * static inline copies into helpers with no original call/binding evidence. */
#define resource_initialize_tail(record_) do { \
    GeorgeResourceRecord *r_tail_record=(record_); \
    u8 r_tail_flags=r_tail_record->flags; \
    r_tail_record->field24=0; \
    r_tail_record->field20=D_0043A408; \
    if (r_tail_flags&0x10) { \
        r_tail_record->flags|=2; \
        func_00226FB0(r_tail_record); \
    } \
} while (0)
#define resource_finish_payload(holder_) do { \
    void *r_finish_holder=(holder_); \
    void *r_finish_payload; \
    func_002B6308(r_finish_holder); \
    r_finish_payload=R_FIELD(r_finish_holder,8,void *); \
    if (R_FIELD(r_finish_payload,0,u32)&0x80000) \
        func_002867E8(r_finish_holder,R_FIELD(r_finish_payload,8,u32)); \
} while (0)

GeorgeResourceRecord *func_0020EC10(GeorgeResourceRecord *record,const void *key,u32 mode,u32 word)
{
    func_00226BC0(record,key,1,mode,word);
    resource_initialize_tail(record);
    return record;
}
GeorgeResourceRecord *func_0020EAA0(const void *key,u32 mode,u32 word)
{
    GeorgeResourceRecord *record=func_00225E80(key,1);
    if (record!=0) {
        u8 flags=record->flags;
        if (mode!=0) {
            if ((flags&0x40)==0) {
                record->flags|=0x40;
                func_00224678(1,0,record);
            }
        } else if (flags&0x80) func_00226D78(record);
        else record->count=(u16)(record->count+1u);
        return record;
    }
    record=(GeorgeResourceRecord *)func_002AEE60(0x28);
    func_00226BC0(record,key,1,mode,word);
    resource_initialize_tail(record);
    if (record->flags&8) {
        if (record!=0) {
            const GeorgeGoalVirtualWord *pair=(const GeorgeGoalVirtualWord *)R_ADDRESS(record->field20,8);
            pair->invoke(R_ADDRESS(record,(s32)pair->adjustment),3);
        }
        record=0;
    } else if (mode!=0) func_00224678(1,0,record);
    return record;
}
void func_0020E920(GeorgeResourceRecord *record,u32 mode)
{
    void *holder=record->field10;
    if (record->flags&0x10) {
        record->flags|=0x20;
        record->flags|=4;
    }
    else if (mode!=0) {
        u8 flags;
        if (record->field24==0) return;
        func_002B62E0(holder);
        holder=record->field10;
        flags=record->flags;
        R_FIELD(holder,8,void *)=record->field24->field10;
        record->flags|=0x20;
        resource_finish_payload(holder);
        record->flags|=flags;
    } else {
        GeorgeResourceRecord *other;
        func_002B62E0(holder);
        other=func_0020F6F8(R_FIELD(holder,8,void *),0,record->field1C);
        record->field24=other;
        if (other!=0) {
            if ((other->flags&0x20)==0)
                func_00226E38(other,func_0020EEC8,record,record);
            else {
                holder=record->field10;
                R_FIELD(holder,8,void *)=other->field10;
                record->flags|=0x20;
                resource_finish_payload(holder);
                if (R_FIELD(holder,2,u8)!=0) func_0021C508(holder);
            }
        }
        record->flags|=4;
    }
}
void func_0020EEC8(u32 result,GeorgeResourceRecord *record)
{
    void *holder=record->field10;
    R_FIELD(holder,8,void *)=record->field24->field10;
    record->flags|=0x20;
    resource_finish_payload(holder);
    if (result!=0xFFFFFFFFu&&R_FIELD(holder,2,u8)!=0) func_0021C508(holder);
}
u32 func_0020EC78(const GeorgeResourceRecord *record)
{
    if (record->flags&0x20)
        return R_FIELD(R_FIELD(record->field10,8,void *),0,u32)&0x8000;
    return 0;
}
u32 func_0020ECA0(const GeorgeResourceRecord *record,const GeorgeRotationMatrix *matrix,const GeorgeGeometryFace *faces)
{
    if (record->flags&0x20) {
        void *payload=R_FIELD(record->field10,8,void *);
        const GeorgeMathVec4 *input=(const GeorgeMathVec4 *)R_ADDRESS(payload,0x30);
        GeorgeMathVec4 sphere __attribute__((aligned(16)));
        GeorgeGeometryFrame frame __attribute__((aligned(16)));
        s32 classification;
        sphere.x=input->x;sphere.y=input->y;sphere.z=input->z;sphere.w=input->w;
        func_002A1C60(matrix,input,&sphere);
        classification=(s32)func_002A48F0(faces,&sphere);
        if (classification==1||classification==2) {
            func_0029FEF8(&frame,R_ADDRESS(payload,0x40),matrix);
            return func_002A4808(faces,&frame);
        }
    }
    return 0;
}
