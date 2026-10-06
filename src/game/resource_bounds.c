#include "george/resource_bounds.h"

extern const u8 D_0043A458[], D_0043A4B0[];
extern GeorgeResourceRecord *func_00225E80(const void *,u32 kind);
extern void *func_002AEE60(u32 size);
extern void func_00226BC0(GeorgeResourceRecord *,const void *,u32 kind,u32 mode,u32 word);
extern void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *);
extern void func_00226D78(GeorgeResourceRecord *);
extern void func_002267D0(GeorgeResourceRecord *,u32 mode);
extern void func_002B6F28(void *holder,u32 index,void *payload);
extern void *func_0022B360(void *holder,GeorgeMathVec4 *point,GeorgeRotationMatrix *frame);

#define BOUNDS_WORD(object,offset) (*(u32 *)((u8 *)(object)+(offset)))
#define BOUNDS_HALF(object,offset) (*(u16 *)((u8 *)(object)+(offset)))
#define BOUNDS_POINTER(object,offset) (*(void **)((u8 *)(object)+(offset)))

u32 func_0020F858(const GeorgeResourceGroup *group) { return group->count24; }
GeorgeResourceRecord *func_0020F860(const GeorgeResourceGroup *group,u32 index)
{ return *(GeorgeResourceRecord **)((u8 *)group+0x28+(index<<2)); }

GeorgeResourceGroup *func_0020F870(GeorgeResourceGroup *group,const void *key,u32 mode,u32 word)
{
    u8 flags;
    func_00226BC0((GeorgeResourceRecord *)group,key,2,mode,word);
    flags=group->flags;
    group->count24=0;
    group->field20=D_0043A458;
    group->count26=0;
    if (flags&0x10) group->flags|=0x26;
    return group;
}

void func_002B6F60(void *holder,void *payload,u32 index)
{
    if (index==0xFFFFFFFFu) {
        u32 i=0,offset=0;
        if (BOUNDS_HALF(holder,0x1C)!=0) {
            do {
                void *entry=(u8 *)BOUNDS_POINTER(holder,0x20)+offset;
                BOUNDS_POINTER(entry,0)=payload;
                BOUNDS_WORD(payload,0x10)=0;
                ++i;offset+=0x24;
            } while (i<BOUNDS_HALF(holder,0x1C));
        }
    } else {
        void *entry=(u8 *)BOUNDS_POINTER(holder,0x20)+index*0x24;
        BOUNDS_POINTER(entry,0)=payload;
        BOUNDS_WORD(payload,0x10)=0;
    }
}

u32 func_0020F8D0(GeorgeResourceGroup *group,const GeorgeResourceRecord *child,u32 index)
{
    void *holder=group->field10;
    if (child!=0) {
        if ((child->flags&4)==0) return 0;
        func_002B6F60(holder,child->field10,index);
    } else {
        u32 i;
        for (i=0;i<group->count24;++i) {
            const GeorgeResourceRecord *entry=group->field28[i];
            if (entry!=0&&(entry->flags&4)!=0)
                func_002B6F28(holder,i,entry->field10);
        }
    }
    return 1;
}

void func_0020F990(GeorgeResourceGroup *group) { func_0020F5D8(group); }
void func_0020F9B0(GeorgeResourceGroup *group,u32 mode)
{
    group->field20=D_0043A458;
    if ((group->flags&0x80)==0) func_0020F5D8(group);
    func_002267D0((GeorgeResourceRecord *)group,mode);
}

void func_0020FA08(GeorgeResourceBounds *bounds,u32 mode)
{
    if (mode==0) {
        GeorgeMathVec4 point;
        GeorgeRotationMatrix frame;
        GeorgeResourceRecord *payload;
        float x,y,z;
        bounds->base.flags|=0x24;
        payload=(GeorgeResourceRecord *)func_0022B360(bounds->base.field10,&point,&frame);
        x=point.x;y=point.y;z=point.z;
        bounds->base.field24=payload;
        bounds->field28.x=x;
        bounds->field28.z=z;
        bounds->field28.y=y;
        x=frame.element[0];y=frame.element[5];z=frame.element[10];
        bounds->field34.x=x;
        bounds->field34.z=z;
        bounds->field34.y=y;
    }
}

/* The same reviewed scalar initialization sequence is inlined in the factory
 * and explicit constructor; the factory retains its original call graph. */
#define INITIALIZE_BOUNDS(bounds_) do { \
    GeorgeResourceBounds *initialized=(bounds_); \
    initialized->base.field24=0; \
    BOUNDS_WORD(initialized,0x28)=0; \
    initialized->base.field20=D_0043A4B0; \
    BOUNDS_WORD(initialized,0x30)=0; \
    BOUNDS_WORD(initialized,0x2C)=0; \
    BOUNDS_WORD(initialized,0x34)=0; \
    BOUNDS_WORD(initialized,0x3C)=0; \
    BOUNDS_WORD(initialized,0x38)=0; \
    if (initialized->base.flags&0x10) { \
        initialized->base.flags|=0x26; \
        func_0020FA08(initialized,0); \
    } \
} while (0)

GeorgeResourceBounds *func_0020FA88(const void *key,u32 mode,u32 word)
{
    GeorgeResourceBounds *bounds=(GeorgeResourceBounds *)func_00225E80(key,6);
    if (bounds!=0) {
        u8 flags=bounds->base.flags;
        if (mode!=0) {
            if ((flags&0x40)==0) {
                bounds->base.flags|=0x40;
                func_00224678(1,0,&bounds->base);
            }
        } else if (flags&0x80) func_00226D78(&bounds->base);
        else bounds->base.count=(u16)(bounds->base.count+1);
        return bounds;
    }
    bounds=(GeorgeResourceBounds *)func_002AEE60(0x40);
    func_00226BC0(&bounds->base,key,6,mode,word);
    INITIALIZE_BOUNDS(bounds);
    if (bounds->base.flags&8) {
        if (bounds!=0) {
            const GeorgeGoalVirtualWord *pair=(const GeorgeGoalVirtualWord *)(bounds->base.field20+8);
            pair->invoke((void *)((u32)bounds+(s32)pair->adjustment),3);
        }
        bounds=0;
    } else if (mode!=0) func_00224678(1,0,&bounds->base);
    return bounds;
}

void *func_0020FC18(const GeorgeResourceBounds *bounds) { return bounds->base.field24; }
GeorgeMathVec3 *func_0020FC20(GeorgeResourceBounds *bounds) { return &bounds->field28; }
GeorgeMathVec3 *func_0020FC28(GeorgeResourceBounds *bounds) { return &bounds->field34; }
GeorgeResourceBounds *func_0020FC30(GeorgeResourceBounds *bounds,const void *key,u32 mode,u32 word)
{
    func_00226BC0(&bounds->base,key,6,mode,word);
    INITIALIZE_BOUNDS(bounds);
    return bounds;
}
void func_0020FCC0(GeorgeResourceBounds *bounds,u32 mode)
{
    void *payload;
    bounds->base.field20=D_0043A4B0;
    payload=bounds->base.field24;
    if (payload!=0) {
        if (BOUNDS_HALF(payload,4)!=0) {
            u16 count=(u16)(BOUNDS_HALF(payload,6)-1U);
            BOUNDS_HALF(payload,6)=count;
            if (count==0) {
                const GeorgeGoalVirtualWord *pair=(const GeorgeGoalVirtualWord *)((u8 *)BOUNDS_POINTER(payload,0)+8);
                pair->invoke((void *)((u32)payload+(s32)pair->adjustment),3);
            }
        }
        bounds->base.field24=0;
    }
    func_002267D0(&bounds->base,mode);
}
