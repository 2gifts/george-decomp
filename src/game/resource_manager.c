#include "george/resource_manager.h"

extern u32 func_00229FC8(void *);
extern u32 func_00229FF0(void *,u32 kind,const void *key,u32 *address,u32 *size);
extern u32 func_00229020(void *,const void *key,u32 *address,u32 *size);
/* The original caller supplies three GPR lanes; the published pool body
 * consumes only its first argument. Native tests use an explicit bridge. */
extern void *func_002AD700(struct GeorgeSlotPool *,u32 size,u32 mode);
extern void *func_0020E598(u32 size);
extern void *func_00217F78(const void *key);
extern void *func_00218020(const void *key);
extern void *func_00217FB0(const void *key);
extern void *func_00217FE8(const void *key);
extern void func_00225D38(GeorgeResourceRecord *);
extern void func_0021ABD8(void *queue,u32 auxiliary,const void *key,u32 address,
                        u32 size,void *payload,GeorgeResourceCompletion,
                        GeorgeResourceRecord *,u32 special);
extern void func_0021A0B8(GeorgeGenericMap *,u32 key,void *value);
extern u32 func_0021A190(GeorgeGenericMap *,u32 key);

#define MANAGER() ((GeorgeResourceManagerState *)D_003F960C)
#define PROVIDER(manager,index) \
    (*(GeorgeResourceProvider **)((u32)(manager)+0x100U+((index)<<2)))

#define RESOURCE_ACCOUNT_FUNCTION func_00224750
#define RESOURCE_ACCOUNT_CHANGE(value,amount) ((value)-(amount))
#include "resource_accounting_template.h"
#undef RESOURCE_ACCOUNT_CHANGE
#undef RESOURCE_ACCOUNT_FUNCTION

#define RESOURCE_ACCOUNT_FUNCTION func_00224678
#define RESOURCE_ACCOUNT_CHANGE(value,amount) ((value)+(amount))
#include "resource_accounting_template.h"
#undef RESOURCE_ACCOUNT_CHANGE
#undef RESOURCE_ACCOUNT_FUNCTION

void func_00225C28(const void *key,u32 kind)
{
    u32 shifted=kind<<2;
    GeorgeGenericMap *map=*(GeorgeGenericMap **)((u32)MANAGER()+0x78U+shifted);
    if (func_0021A190(map,(u32)key)!=0) {
        u32 *count=(u32 *)((u32)MANAGER()+0x98U+shifted);
        *count=*count-1U;
    }
}

void func_00224AB8(GeorgeResourceRecord *record)
{
    GeorgeResourceBasePrefix *base=(GeorgeResourceBasePrefix *)record;
    u32 status=1,index=0,auxiliary=0,address;
    if (MANAGER()->provider_count!=0) {
        do {
            GeorgeResourceProvider *provider=PROVIDER(MANAGER(),index);
            if (provider->enabled==1) {
                u32 provider_kind=provider->kind;
                u32 kind=base->kind;
                const void *key=base->key;
                status=2;
                if (provider_kind==9) {
                    if (func_00229FC8(provider->object)!=0 && kind<7) {
                        u32 command;
                        switch (kind) {
                        case 0: command=1; break;
                        case 1: command=5; break;
                        case 2: command=4; break;
                        case 3: command=3; break;
                        case 4: command=0; break;
                        case 6: command=2; break;
                        default: goto next_provider;
                        }
                        if (func_00229FF0(provider->object,command,key,&address,&base->size)!=0)
                            status=0;
                    }
                } else if (kind==provider_kind) {
                    if (func_00229020(provider->object,key,&address,&base->size)!=0)
                        status=0;
                }
                if (status<2) {
                    GeorgeResourceProvider *fresh=PROVIDER(MANAGER(),index);
                    if (fresh->kind==9)
                        auxiliary=*(u32 *)((u32)fresh->object+0x244U);
                    else
                        auxiliary=*(u32 *)((u32)fresh->object+0xA8U);
                    break;
                }
            }
next_provider:
            index=index+1U;
        } while (index<MANAGER()->provider_count);
    }
    if (status==0) {
        GeorgeResourceAllocator *allocator;
        void *allocation=0;
        u32 size=base->size;
        u32 allocated_size=size;
        u8 flags;
        base->field08=address;
        flags=base->flags;
        allocator=MANAGER()->allocator;
        if (allocator->cursor!=0 && allocator->mode==1) {
            allocated_size=(allocated_size+0x7FU)&~0x7FU;
            if (allocator->override_pool!=0) {
                allocation=func_002AD700(allocator->override_pool,allocated_size,1);
            } else if (*(u32 *)((u32)allocator+0xFB8U)>=allocated_size) {
                u32 remaining=*(u32 *)((u32)allocator+0xFB8U);
                void *cursor=allocator->cursor;
                *(u32 *)((u32)allocator+0xFB8U)=remaining-allocated_size;
                allocator->cursor=(void *)((u32)cursor+allocated_size);
                allocation=cursor;
            }
        } else {
            if ((flags&0x40)!=0) {
                u32 slot;
                allocated_size=(size+0x7FU)&~0x7FU;
                slot=allocated_size>>7;
                if (slot<1000) {
                    struct GeorgeSlotPool *pool=allocator->slots[slot];
                    if (pool!=0) allocation=func_002AD700(pool,allocated_size,1);
                }
            }
            if (allocation==0) allocation=func_0020E598(allocated_size);
        }
        base->payload=allocation;
        func_00224678(0,1,record);
        {
            u32 word=base->field1C;
            base->field14=auxiliary;
            func_0021ABD8(MANAGER()->request_queue,auxiliary,base->key,
                         base->field08,base->size,base->payload,
                         func_00225D38,record,word==1);
        }
    } else {
        void *payload=0;
        u32 kind=base->kind;
        if (kind==1) payload=func_00218020(base->key);
        else if (kind==0) payload=func_00217F78(base->key);
        else if (kind==3) payload=func_00217FB0(base->key);
        else if (kind==6) payload=func_00217FE8(base->key);
        if (payload==0) {
            base->flags|=8;
            return;
        }
        {
            u8 flags=base->flags;
            base->payload=payload;
            kind=base->kind;
            base->flags=(u8)(flags|0x10);
            /* Capture kind before the flags store, as in the original. */
            index=kind<<2;
        }
        goto publish;
    }
    index=(u32)base->kind<<2;
publish:
    {
        GeorgeResourceManagerState *manager=MANAGER();
        const void *key=base->key;
        GeorgeGenericMap *map=*(GeorgeGenericMap **)((u32)manager+0x78U+index);
        u32 *count,*peak;
        u32 value;
        func_0021A0B8(map,(u32)key,record);
        count=(u32 *)((u32)MANAGER()+0x98U+index);
        *count=*count+1U;
        manager=MANAGER();
        peak=(u32 *)((u32)manager+0xB8U+index);
        value=*(u32 *)((u32)manager+0x98U+index);
        if (*peak<value) *peak=value;
    }
}

#undef PROVIDER
#undef MANAGER
