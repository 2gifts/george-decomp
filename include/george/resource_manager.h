#ifndef GEORGE_RESOURCE_MANAGER_H
#define GEORGE_RESOURCE_MANAGER_H

#include "george/resource_base.h"
#include "george/deimos_tables.h"

/* Observed fields through +168. Names describe access, not a complete class.
 * Provider slots start at +100; no bound on provider_count is added here. */
typedef struct GeorgeResourceManagerState {
    u8 reserved00[0x24];
    u32 count24,count28;
    u8 reserved2C[4];
    u32 bytes30,bytes34;
    u32 kind_bytes38[8],kind_count58[8];
    GeorgeGenericMap *kind_maps78[8];
    u32 kind_count98[8],kind_peakB8[8];
    u8 reservedD8[0x24];
    u32 provider_count;
    u8 reserved100[0x64];
    void *request_queue;
    GeorgeResourceAllocator *allocator;
} GeorgeResourceManagerState;

typedef struct GeorgeResourceProvider {
    u32 kind,enabled;
    u8 reserved08[8];
    void *object;
} GeorgeResourceProvider;

typedef void (*GeorgeResourceCompletion)(GeorgeResourceRecord *);
extern GeorgeResourceManagerPrefix *D_003F960C;

typedef char resource_state_bytes30[(offsetof(GeorgeResourceManagerState,bytes30)==0x30)?1:-1];
typedef char resource_state_kindbytes38[(offsetof(GeorgeResourceManagerState,kind_bytes38)==0x38)?1:-1];
typedef char resource_state_kindcounts58[(offsetof(GeorgeResourceManagerState,kind_count58)==0x58)?1:-1];
typedef char resource_state_maps78[(offsetof(GeorgeResourceManagerState,kind_maps78)==0x78)?1:-1];
typedef char resource_state_counts98[(offsetof(GeorgeResourceManagerState,kind_count98)==0x98)?1:-1];
typedef char resource_state_peakB8[(offsetof(GeorgeResourceManagerState,kind_peakB8)==0xB8)?1:-1];
typedef char resource_state_provider_count[(offsetof(GeorgeResourceManagerState,provider_count)==0xFC)?1:-1];
typedef char resource_state_queue[(offsetof(GeorgeResourceManagerState,request_queue)==0x164)?1:-1];
typedef char resource_state_allocator[(offsetof(GeorgeResourceManagerState,allocator)==0x168)?1:-1];
typedef char resource_state_prefix_size[(sizeof(GeorgeResourceManagerState)==0x16C)?1:-1];
typedef char resource_provider_object[(offsetof(GeorgeResourceProvider,object)==0x10)?1:-1];

void func_00224AB8(GeorgeResourceRecord *) GEORGE_SAVE128;
void func_00225C28(const void *key,u32 kind) GEORGE_SAVE128;
void func_00224750(u32 first,u32 second,GeorgeResourceRecord *);
void func_00224678(u32 first,u32 second,GeorgeResourceRecord *);

#endif
