#ifndef GEORGE_RESOURCE_REGISTRY_H
#define GEORGE_RESOURCE_REGISTRY_H

#include "george/deimos_tables.h"

/* The 219D58/219EB8 allocation bodies establish this 16-byte pointer stack.
 * It is separate from GeorgeSlotPool: taking a slot increments index, and
 * returning a node decrements it. Capacity checks are not added here. */
typedef struct GeorgeRegistryNodePool {
    u32 capacity,index;
    GeorgeGenericMapNode **slots;
    GeorgeGenericMapNode *storage;
} GeorgeRegistryNodePool;
typedef struct GeorgeRegistryPooledMap {
    GeorgeGenericMap map;
    GeorgeRegistryNodePool *pool;
} GeorgeRegistryPooledMap;

/* Self-relative index prefix; rows have the observed, runtime-supplied stride. */
typedef struct GeorgeRegistryIndex {
    u32 bucket_count,row_count,bucket_offset,row_offset,row_stride;
    u8 reserved14[12];
} GeorgeRegistryIndex;
typedef struct GeorgeRegistryIndexRow { u32 key,word04,word08; } GeorgeRegistryIndexRow;
typedef struct GeorgeRegistryData {
    u32 kind,flags;
    s32 count;
    u8 reserved0C[16];
    void *index_or_rows;
    void *origin;
} GeorgeRegistryData;
typedef struct GeorgeRegistryDataGroup { u8 reserved00[12]; GeorgeRegistryData *first; } GeorgeRegistryDataGroup;

/* These observed prefixes do not establish original class names. The final
 * linked record is a sentinel: its next pointer is zero and it is not queried. */
typedef struct GeorgeRegistryProviderLink {
    struct GeorgeRegistryProviderLink *next;
    u8 reserved04[12];
    u32 disabled;
    u8 reserved14[0x84];
    GeorgeRegistryDataGroup *group;
} GeorgeRegistryProviderLink;
typedef struct GeorgeRegistryProviderList { u8 reserved00[0x84]; GeorgeRegistryProviderLink *first; } GeorgeRegistryProviderList;
typedef struct GeorgeRegistryRoot { GeorgeRegistryProviderList *context; } GeorgeRegistryRoot;
typedef struct GeorgeRegistryProvider {
    u8 reserved00[0x10];
    GeorgeRegistryData *data;
    u32 base;
    u8 reserved18[0x8C];
    u32 tag;
} GeorgeRegistryProvider;
typedef struct GeorgeRegistryIndexedProvider {
    u8 reserved00[0x88];
    u32 base;
    u8 reserved8C[12];
    GeorgeRegistryIndex *first;
    u8 reserved9C[0x124];
    u32 tag;
} GeorgeRegistryIndexedProvider;

#define REGISTRY_OFFSET(type,field,n) typedef char registry_##type##_##field[(offsetof(type,field)==(n))?1:-1]
typedef char registry_pool_size[(sizeof(GeorgeRegistryNodePool)==16)?1:-1];
typedef char registry_pooled_map_size[(sizeof(GeorgeRegistryPooledMap)==20)?1:-1];
typedef char registry_index_size[(sizeof(GeorgeRegistryIndex)==32)?1:-1];
typedef char registry_row_size[(sizeof(GeorgeRegistryIndexRow)==12)?1:-1];
REGISTRY_OFFSET(GeorgeRegistryNodePool,index,4);
REGISTRY_OFFSET(GeorgeRegistryNodePool,slots,8);
REGISTRY_OFFSET(GeorgeRegistryNodePool,storage,12);
REGISTRY_OFFSET(GeorgeRegistryPooledMap,pool,16);
REGISTRY_OFFSET(GeorgeRegistryData,index_or_rows,0x1C);
REGISTRY_OFFSET(GeorgeRegistryData,origin,0x20);
REGISTRY_OFFSET(GeorgeRegistryProviderLink,disabled,0x10);
REGISTRY_OFFSET(GeorgeRegistryProviderLink,group,0x98);
REGISTRY_OFFSET(GeorgeRegistryProviderList,first,0x84);
REGISTRY_OFFSET(GeorgeRegistryProvider,data,0x10);
REGISTRY_OFFSET(GeorgeRegistryProvider,base,0x14);
REGISTRY_OFFSET(GeorgeRegistryProvider,tag,0xA4);
REGISTRY_OFFSET(GeorgeRegistryIndexedProvider,base,0x88);
REGISTRY_OFFSET(GeorgeRegistryIndexedProvider,first,0x98);
REGISTRY_OFFSET(GeorgeRegistryIndexedProvider,tag,0x1C0);
#undef REGISTRY_OFFSET

extern GeorgeRegistryRoot *D_003F9468;
void *func_00217F78(const void *key);
void *func_00217FB0(const void *key);
void *func_00217FE8(const void *key);
void *func_00218020(const void *key);
void *func_002193A0(GeorgeRegistryProviderList *,u32 kind,const void *key) GEORGE_SAVE128;
void func_0021A0B8(GeorgeGenericMap *,u32 key,void *value);
u32 func_0021A190(GeorgeGenericMap *,u32 key);
u32 func_0021A238(GeorgeGenericMap *,u32 key,void **output);
u32 func_00229020(void *,const void *key,u32 *address,u32 *size) GEORGE_SAVE128;
u32 func_00229FF0(void *,u32 kind,const void *key,u32 *address,u32 *size) GEORGE_SAVE128;
void *func_002A77F0(GeorgeRegistryDataGroup *,u32 kind,const void *key);
void *func_002AF9A0(GeorgeRegistryData *,const void *key,u32 *size) GEORGE_SAVE128;
GeorgeRegistryIndexRow *func_002A8250(GeorgeRegistryIndex *,const void *key);

#endif
