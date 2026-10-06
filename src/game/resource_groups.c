#include "george/resource_groups.h"

extern const u8 D_0043A458[];
extern void *D_004682DC;
extern GeorgeResourceRecord *func_00225E80(const void *,u32 kind);
extern void *func_002AEE60(u32 size);
extern void func_00226BC0(GeorgeResourceRecord *,const void *,u32 kind,u32 mode,u32 word);
extern void func_00224678(u32 mode,u32 word,GeorgeResourceRecord *);
extern void func_00226D78(GeorgeResourceRecord *);
extern void func_00226ED8(GeorgeResourceRecord *,GeorgeResourceRecord *);
extern void func_00226E38(GeorgeResourceRecord *,void (*)(u32,GeorgeResourceGroup *),GeorgeResourceGroup *,GeorgeResourceGroup *);
extern void func_00226F40(GeorgeResourceRecord *);
extern GeorgeResourceRecord *func_0020FD48(const void *,u32 mode,u32 word);
extern void func_002B6DC0(void *);
extern u32 func_002B7088(void *,u32 limit,void **output);
extern u32 func_002B70E0(void *,void **output);
extern void func_002B6F28(void *,u32 index,void *payload);
extern void func_002B6F48(void *,u32 index,void *payload);
extern void func_00227B68(void *);
extern void func_00227B98(void *);

#define GROUP_WORD(object,offset) (*(u32 *)((u8 *)(object)+(offset)))
#define GROUP_HALF(object,offset) (*(u16 *)((u8 *)(object)+(offset)))
#define GROUP_POINTER(object,offset) (*(void **)((u8 *)(object)+(offset)))
/* The complete connected relocation/enumeration/publication helper bodies.
 * Output and input may alias: retain the original store sequence. */
void func_002B6DC0(void *holder)
{
    u32 offset=GROUP_WORD(holder,0x20);
    if (offset!=0) GROUP_WORD(holder,0x20)=(u32)holder+offset;
    offset=GROUP_WORD(holder,0x24);
    if (offset!=0) GROUP_WORD(holder,0x24)=(u32)holder+offset;
    offset=GROUP_WORD(holder,8);
    if (offset!=0) GROUP_WORD(holder,8)=(u32)holder+offset;
}
u32 func_002B7088(void *holder,u32 limit,void **output)
{
    u32 index=0;
    u8 *entry=(u8 *)GROUP_POINTER(holder,0x20);
    if (GROUP_HALF(holder,0x1C)!=0&&limit!=0) {
        do {
            *output=GROUP_POINTER(entry,0);
            ++index;entry+=0x24;++output;
        } while (index<GROUP_HALF(holder,0x1C)&&index<limit);
    }
    return index;
}
u32 func_002B70E0(void *holder,void **output)
{
    u32 count=GROUP_HALF(holder,0x1C),total=0,index;
    u8 *entry=(u8 *)GROUP_POINTER(holder,0x20);
    u8 *secondary=(u8 *)GROUP_POINTER(holder,0x24);
    if (entry!=0&&secondary!=0) {
        for (index=0;index<count;++index,entry+=0x24) total+=entry[0x1D];
        for (index=0;index<total;++index,secondary+=8,++output) {
            *output=GROUP_POINTER(secondary,4);
            GROUP_POINTER(secondary,4)=0;
        }
    }
    return total;
}
void func_002B6F28(void *holder,u32 index,void *payload)
{
    void *entry=(u8 *)GROUP_POINTER(holder,0x20)+index*0x24;
    GROUP_POINTER(entry,0)=payload;
    GROUP_WORD(payload,0x10)=0;
}
void func_002B6F48(void *holder,u32 index,void *payload)
{
    void *entry=(u8 *)GROUP_POINTER(holder,0x24)+(index<<3);
    GROUP_POINTER(entry,4)=payload;
}
/* Reuse the already reviewed activation/count sequence. Each group loop
 * preserves its own fresh count load and original numeric call. */
#define GROUP_ACTIVATE(child_) do { \
    GeorgeResourceRecord *group_child=(child_); \
    if (group_child!=0) { \
        if (group_child->flags&0x80) func_00226D78(group_child); \
        else group_child->count=(u16)(group_child->count+1); \
    } \
} while (0)

void func_0020F128(GeorgeResourceGroup *group,u32 mode)
{
    void *holder=group->field10;
    if (mode==0) {
        void *first[16],*second[16];
        u32 pending=0,index;
        func_002B6DC0(holder);
        group->count24=(u16)func_002B7088(holder,16,first);
        group->count26=(u16)func_002B70E0(holder,second);
        for (index=0;index<group->count24;++index) {
            void *key=first[index];
            if (key==0) group->field28[index]=0;
            else {
                GeorgeResourceRecord *child=func_0020FD48(key,0,group->field1C);
                group->field28[index]=child;
                if (child!=0&&(child->flags&0x20)==0) {
                    pending=1;
                    func_00226E38(child,func_0020F440,group,group);
                }
            }
        }
        if (group->count26!=0) GROUP_WORD(holder,0)|=0x40;
        for (index=0;index<group->count26;++index) {
            void *key=second[index];
            if (key==0) group->field68[index]=0;
            else {
                GeorgeResourceRecord *child=func_0020FD48(key,0,group->field1C);
                group->field68[index]=child;
                if (child!=0&&(child->flags&0x20)==0) {
                    pending=1;
                    func_00226E38(child,func_0020F440,group,group);
                }
            }
        }
        group->flags|=4;
        if (pending==0) func_0020F440(0,group);
    } else if (group->flags&4) {
        func_002B6DC0(holder);
        func_0020F440(0xFFFFFFFFu,group);
        if (group->count26!=0) GROUP_WORD(holder,0)|=0x40;
    }
}

void func_0020F340(GeorgeResourceGroup *group)
{
    u32 index;
    for (index=0;index<group->count24;++index) GROUP_ACTIVATE(group->field28[index]);
    for (index=0;index<group->count26;++index) GROUP_ACTIVATE(group->field68[index]);
    if (group->count26!=0) func_00227B68(group->field10);
}

void func_0020F440(u32 result,GeorgeResourceGroup *group)
{
    u32 ready=1,index;
    for (index=0;index<group->count24;++index) {
        GeorgeResourceRecord *child=group->field28[index];
        if (child==0) func_002B6F28(group->field10,index,D_004682DC);
        else if (child->flags&0x20)
            func_002B6F28(group->field10,index,group->field28[index]->field10);
        else { ready=0; break; }
    }
    if (group->count26!=0) {
        if (ready==0) return;
        for (index=0;index<group->count26;++index) {
            GeorgeResourceRecord *child=group->field68[index];
            if (child==0) func_002B6F48(group->field10,index,D_004682DC);
            else if (child->flags&0x20)
                func_002B6F48(group->field10,index,group->field68[index]->field10);
            else { ready=0; break; }
        }
    }
    if (ready!=0) {
        if (result!=0xFFFFFFFFu&&group->count26!=0) func_00227B68(group->field10);
        group->flags|=0x20;
        func_00226F40((GeorgeResourceRecord *)group);
    }
}

/* The group release order differs from the single-child resource: virtual
 * release occurs first, then a pending detach reads the array entry fresh. */
#define GROUP_RELEASE(array_,index_,pending_) do { \
    GeorgeResourceRecord *group_child=(array_)[index_]; \
    if (group_child!=0) { \
        const GeorgeGoalVirtualWord *group_pair=(const GeorgeGoalVirtualWord *)(group_child->field20+0x10); \
        group_pair->invoke((void *)((u32)group_child+(s32)group_pair->adjustment),0); \
        if (pending_) func_00226ED8((array_)[index_],(GeorgeResourceRecord *)group); \
    } \
} while (0)

void func_0020F5D8(GeorgeResourceGroup *group)
{
    u32 pending=0,index;
    u8 flags;
    if (group->count26!=0) func_00227B98(group->field10);
    flags=group->flags;
    if ((flags&4)!=0&&(flags&0x20)==0) pending=1;
    for (index=0;index<group->count24;++index) GROUP_RELEASE(group->field28,index,pending);
    for (index=0;index<group->count26;++index) GROUP_RELEASE(group->field68,index,pending);
}

GeorgeResourceRecord *func_0020F6F8(void *key,u32 mode,u32 word)
{
    GeorgeResourceGroup *group=(GeorgeResourceGroup *)func_00225E80(key,2);
    if (group!=0) {
        u8 flags=group->flags;
        if (mode!=0) {
            if ((flags&0x40)==0) {
                group->flags|=0x40;
                func_00224678(1,0,(GeorgeResourceRecord *)group);
            }
        } else if (flags&0x80) func_00226D78((GeorgeResourceRecord *)group);
        else group->count=(u16)(group->count+1);
        return (GeorgeResourceRecord *)group;
    }
    group=(GeorgeResourceGroup *)func_002AEE60(0xE8);
    func_00226BC0((GeorgeResourceRecord *)group,key,2,mode,word);
    {
        u8 flags=group->flags;
        group->count24=0;
        group->field20=D_0043A458;
        group->count26=0;
        if (flags&0x10) group->flags|=0x26;
    }
    if (group->flags&8) {
        if (group!=0) {
            const GeorgeGoalVirtualWord *pair=(const GeorgeGoalVirtualWord *)(group->field20+8);
            pair->invoke((void *)((u32)group+(s32)pair->adjustment),3);
        }
        group=0;
    } else if (mode!=0) func_00224678(1,0,(GeorgeResourceRecord *)group);
    return (GeorgeResourceRecord *)group;
}
