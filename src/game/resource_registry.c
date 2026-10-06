#include "george/resource_registry.h"
#include "george/accessors.h"

/* Each wrapper remains a complete symbol with its original direct call. */
#define REGISTRY_LOOKUP(name,kind_value) \
void *name(const void *key) \
{ \
    GeorgeRegistryRoot *root=D_003F9468; \
    if (root!=0) return func_002193A0(root->context,kind_value,key); \
    return 0; \
}
REGISTRY_LOOKUP(func_00217F78,14)
REGISTRY_LOOKUP(func_00217FB0,0)
REGISTRY_LOOKUP(func_00217FE8,6)
REGISTRY_LOOKUP(func_00218020,3)
#undef REGISTRY_LOOKUP

void *func_002193A0(GeorgeRegistryProviderList *context,u32 kind,const void *key)
{
    GeorgeRegistryProviderLink *link=context->first;
    while (link->next!=0) {
        GeorgeRegistryProviderLink *next=link->next;
        void *result=0;
        if (link->disabled==0) result=func_002A77F0(link->group,kind,key);
        if (result!=0) return result;
        link=next;
    }
    return 0;
}

void func_0021A0B8(GeorgeGenericMap *map,u32 key,void *value)
{
    u32 bucket=key%map->bucket_count;
    GeorgeGenericMapNode *node=map->buckets[bucket];
    GeorgeRegistryNodePool *pool;
    GeorgeGenericMapNode **slots;
    u32 index;
    if ((map->flags&1)==0) {
        while (node!=0) {
            if (node->key==key) { node->value=value; return; }
            node=node->next;
        }
    }
    pool=((GeorgeRegistryPooledMap *)map)->pool;
    index=pool->index;
    slots=pool->slots;
    node=slots[index];
    pool->index=index+1U;
    slots[index]=0;
    if (node==0) return;
    {
        GeorgeGenericMapNode *head=map->buckets[bucket];
        node->key=key;
        node->next=head;
        node->value=value;
    }
    map->buckets[bucket]=node;
    map->count=map->count+1U;
}

#define REGISTRY_REMOVE_NAME func_0021A190
#define REGISTRY_REMOVE_ARGUMENT
#define REGISTRY_REMOVE_OUTPUT(node) ((void)0)
#include "resource_registry_remove_template.h"
#undef REGISTRY_REMOVE_OUTPUT
#undef REGISTRY_REMOVE_ARGUMENT
#undef REGISTRY_REMOVE_NAME
#define REGISTRY_REMOVE_NAME func_0021A238
#define REGISTRY_REMOVE_ARGUMENT ,void **output
#define REGISTRY_REMOVE_OUTPUT(node) (*output=(node)->value)
#include "resource_registry_remove_template.h"
#undef REGISTRY_REMOVE_OUTPUT
#undef REGISTRY_REMOVE_ARGUMENT
#undef REGISTRY_REMOVE_NAME

GeorgeRegistryIndexRow *func_002A8250(GeorgeRegistryIndex *index,const void *key)
{
    u32 count=index->bucket_count;
    if (count!=0) {
        u32 bucket=(u32)key%count;
        u32 *buckets=(u32 *)((u32)index+index->bucket_offset);
        u32 stride=index->row_stride;
        u32 offset=index->row_offset;
        u32 first=buckets[bucket];
        u32 row=(u32)index+offset+stride*first;
        u32 last;
        if (bucket<count-1U) last=buckets[bucket+1U];
        else last=index->row_count;
        last=(u32)index+offset+stride*last;
        while (row<last) {
            if (*(u32 *)row==(u32)key) return (GeorgeRegistryIndexRow *)row;
            row=row+stride;
        }
    }
    return 0;
}

void *func_002AF9A0(GeorgeRegistryData *data,const void *key,u32 *size)
{
    if ((data->flags&4)!=0) {
        GeorgeRegistryIndexRow *row=func_002A8250((GeorgeRegistryIndex *)data->index_or_rows,key);
        if (row==0) return 0;
        if (size!=0) *size=row->word04;
        return (void *)((u32)data->origin+row->word08);
    } else {
        if (data->count>0) {
            GeorgeRegistryIndexRow *rows=(GeorgeRegistryIndexRow *)data->index_or_rows;
            u32 index=0;
            GeorgeRegistryIndexRow *found=0;
            do {
                GeorgeRegistryIndexRow *row=(GeorgeRegistryIndexRow *)((u32)rows+index*12U);
                if (row->key==(u32)key) { found=row; break; }
                index=index+1U;
            } while ((s32)index<data->count);
            if (found!=0) {
                void *result=(void *)((u32)data->origin+found->word08);
                if (size!=0) *size=found->word04;
                return result;
            }
        }
        return 0;
    }
}

void *func_002A77F0(GeorgeRegistryDataGroup *group,u32 kind,const void *key)
{
    GeorgeRegistryData *data=*(GeorgeRegistryData **)((u32)group+(kind<<2)+12U);
    if (data!=0) return func_002AF9A0(data,key,0);
    return 0;
}

u32 func_00229020(void *object,const void *key,u32 *address,u32 *size)
{
    GeorgeRegistryProvider *provider=(GeorgeRegistryProvider *)object;
    u32 bytes;
    void *result;
    *size=0;
    result=func_002AF9A0(provider->data,key,&bytes);
    if (result!=0) {
        GeorgeRegistryData *data=provider->data;
        u32 tag=provider->tag;
        u32 origin=(u32)data->origin;
        u32 base=provider->base;
        u32 output=(base+(((u32)result-origin)>>4))|(tag<<28);
        u32 rounded=(bytes+127U)&0xFFFFFF80U;
        *address=output;
        *size=rounded;
        return 1;
    }
    return 0;
}

u32 func_00229FF0(void *object,u32 kind,const void *key,u32 *address,u32 *size)
{
    GeorgeRegistryIndexedProvider *provider=(GeorgeRegistryIndexedProvider *)object;
    GeorgeRegistryIndex **slot=(GeorgeRegistryIndex **)((u32)provider+0x98U+(kind<<2));
    GeorgeRegistryIndexRow *row;
    if (func_002A8228(*slot)==0) return 0;
    row=func_002A8250(*slot,key);
    if (row==0) return 0;
    if (address!=0) {
        u32 offset=row->word04;
        u32 tag=provider->tag;
        u32 base=provider->base;
        *address=(base+(offset>>4))|(tag<<28);
    }
    if (size!=0) *size=(row->word08+15U)&0xFFFFFFF0U;
    return 1;
}
