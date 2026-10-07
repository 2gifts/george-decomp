#ifndef GEORGE_STRING_REGISTRY_H
#define GEORGE_STRING_REGISTRY_H
#include "george/types.h"
#include "george/compiler.h"

typedef struct GeorgeStringRegistryPair {
    char *key;
    char *path;
} GeorgeStringRegistryPair;
typedef char GeorgeStringRegistryPairSize[(sizeof(GeorgeStringRegistryPair)==8)?1:-1];

extern u32 D_003FD1D8;
extern u32 D_003FD1DC;
extern GeorgeStringRegistryPair *D_003FD1E0;
/* Only first-byte identities are established; no total buffer extent claim. */
extern char D_00469A00[],D_00469A80[],D_00469B00[];
extern const char D_00446FE8[],D_00446FF0[],D_00446FF8[];

s32 func_002AB790(const char *key,const char *path) GEORGE_SAVE128;
void func_002ABAE8(const char *input,char *output) GEORGE_SAVE128;
void func_002ABDE8(const char *text) GEORGE_SAVE128;
void func_002ABEF8(void) GEORGE_SAVE128;
#endif
