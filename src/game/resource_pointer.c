#include "george/resource_pointer.h"

/* Reuse the published resource geometry's low32 numeric field conventions. */
#define R_ADDRESS(object,offset) ((u8 *)((u32)(object)+(u32)(offset)))
#define R_FIELD(object,offset,type) (*(type *)R_ADDRESS(object,offset))

u32 george_resource_payload_is_kind2_or3(const void *owner)
{
    const void *payload=R_FIELD(owner,8,void *);
    u32 kind=R_FIELD(payload,0,u32)&0x30000u;
    return kind==0x20000u||kind==0x30000u;
}

void func_002B62E0(void *owner)
{
    u32 byte=R_FIELD(owner,2,u8);
    u32 base=(u32)owner+16u;
    void *slot;
    u32 relative;
    R_FIELD(owner,4,u32)=base;
    slot=(void *)(base+byte*4u);
    R_FIELD(owner,12,u32)=(u32)slot;
    relative=R_FIELD(slot,0,u32);
    R_FIELD(slot,0,u32)=(u32)owner+relative;
}

void func_002B6308(void *owner)
{
    u32 groups=1u,group=0u,entry=0u;
    void *payload;
    if (R_FIELD(owner,0,u16)&1u)
        groups=1u<<(R_FIELD(owner,2,u8)&31u);
    payload=R_FIELD(owner,8,void *);
    do {
        u32 next_group=group+1u;
        u32 index=0u;
        if (R_FIELD(payload,28,u16)!=0) {
            do {
                void *slot=R_FIELD(owner,12,void *);
                void *table=R_FIELD(slot,0,void *);
                void *at=R_ADDRESS(table,entry*4u);
                u32 relative=R_FIELD(at,0,u32);
                if (relative!=0)
                    R_FIELD(at,0,u32)=(u32)owner+relative;
                payload=R_FIELD(owner,8,void *);
                ++index;
                ++entry;
            } while (index<(u32)R_FIELD(payload,28,u16));
        }
        group=next_group;
        if (group>=groups) break;
        payload=R_FIELD(owner,8,void *);
    } while (1);
}

GeorgeResourcePointerBits george_resource_selector(void *owner, u32 index,
                                                   GeorgeResourcePointerBits mode)
{
    u32 bit=1u<<(index&31u);
    u8 old=R_FIELD(owner,3,u8);
    u8 selected=mode!=0?(u8)(old|bit):(u8)(old&~bit);
    u8 current=R_FIELD(owner,3,u8);
    if (selected!=current) {
        void *payload=R_FIELD(owner,8,void *);
        u32 old_byte=R_FIELD(owner,3,u8);
        u32 count=R_FIELD(payload,28,u16);
        void *slot=R_FIELD(owner,12,void *);
        u32 word=R_FIELD(slot,0,u32);
        word-=count*old_byte*4u;
        R_FIELD(slot,0,u32)=word;
        R_FIELD(owner,3,u8)=selected;
        payload=R_FIELD(owner,8,void *);
        slot=R_FIELD(owner,12,void *);
        count=R_FIELD(payload,28,u16);
        word=R_FIELD(slot,0,u32);
        word+=count*(u32)selected*4u;
        R_FIELD(slot,0,u32)=word;
        /* GNU target/native two-complement narrowing, not portable ISO-C. */
        return (GeorgeResourcePointerBits)(signed long long)(s32)word;
    }
    return (GeorgeResourcePointerBits)current;
}
