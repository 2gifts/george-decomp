#include "george/file_archive_lifecycle.h"
#include "george/string_algorithms.h"

/* These numeric SDK/module/scheduler interfaces are observations of selected
 * callers. Their complete implementations and hardware behavior are unawarded. */
extern u32 func_00377820(s32 mode);
extern u32 func_00378038(s32 mode);
extern u32 func_00378258(u32 sector,u32 count,void *buffer,const void *mode);
extern GeorgeHeap *func_002AE830(u32 size,const char *name,u32 alignment);
extern void *func_003936A0(void *destination,s32 value,u32 size);
extern char *func_003934F8(char *destination,const char *source,u32 size);
extern char *func_00393758(char *destination,const char *source);
extern char *func_00393B74(char *destination,const char *source);
extern char *func_00393C90(char *destination,const char *source,u32 size);
extern s32 func_00393E48(const char *left,const char *right,u32 size);
extern char *func_003984D8(char *text);
extern void func_00295D30(const char *path);
extern void func_002C3BC8(s32 argument4);
extern void func_002C8C38(s32 argument4,s32 argument5);
extern void func_002C35B0(s32 argument4,s32 argument5,u32 argument6,u32 argument7);
extern const char D_00447398[],D_004473C8[],D_004473D8[],D_004473E0[];
extern const char D_004473E8[],D_004473F8[],D_00447410[],D_00447428[];
extern const u16 D_004473D0;

/* Genuine newer-GCC inline directive: these expressions have no retail
 * function entry. The old EE compiler uses its ordinary inline declaration. */
#if defined(__GNUC__) && __GNUC__ >= 3
#define ARCHIVE_ALWAYS_INLINE __attribute__((always_inline))
#else
#define ARCHIVE_ALWAYS_INLINE
#endif

/* Byte displacement mirrors the original wrapping SLL/ADDU address algebra.
 * Native fixtures still require the resulting cell to be a valid C object. */
static inline ARCHIVE_ALWAYS_INLINE GeorgeGenericMapNode **archive_bucket(GeorgeGenericMapNode **buckets,u32 index)
{
    return (GeorgeGenericMapNode **)((u32)buckets+(index<<2));
}

static inline ARCHIVE_ALWAYS_INLINE u32 archive_le32(const u8 *bytes)
{
    return ((u32)bytes[3]<<24)|((u32)bytes[2]<<16)|((u32)bytes[1]<<8)|bytes[0];
}

void *func_002A7E68(GeorgeGenericMap *map,u32 *key_output)
{
    if(map->count!=0){
        u32 count=map->bucket_count;
        u32 index=0;
        if(count!=0){
            do{
                GeorgeGenericMapNode *node=*archive_bucket(map->buckets,index);
                if(node!=0){
                    void *value=node->value;
                    GeorgeGenericMapNode **buckets;
                    GeorgeGenericMapNode *next;
                    if(key_output!=0)*key_output=node->key;
                    buckets=map->buckets;
                    next=node->next;
                    *archive_bucket(buckets,index)=next;
                    func_002AEE40(node);
                    map->count=map->count-1u;
                    return value;
                }
                ++index;
            }while(index<count);
        }
    }
    if(key_output!=0)*key_output=0;
    return 0;
}

GeorgeGenericMap *func_002A7F58(u32 bucket_count)
{
    u32 bytes=bucket_count<<2;
    GeorgeGenericMap *map=func_002AEC28(bytes+16u);
    if(map!=0){
        GeorgeGenericMapNode **buckets=(GeorgeGenericMapNode **)((u32)map+16u);
        map->bucket_count=bucket_count;
        map->flags=0;
        map->count=0;
        map->buckets=buckets;
        func_003936A0(buckets,0,bytes);
    }
    return map;
}

void func_002A7FC8(GeorgeGenericMap *map)
{
    u32 index=0;
    if(map->bucket_count!=0){
        do{
            GeorgeGenericMapNode *node=*archive_bucket(map->buckets,index);
            while(node!=0){
                GeorgeGenericMapNode *next=node->next;
                func_002AEE40(node);
                node=next;
            }
            *archive_bucket(map->buckets,index)=0;
            ++index;
        }while(index<map->bucket_count);
    }
    map->count=0;
}

u32 func_002B15C8(GeorgeArchiveContextView *context,u32 sector,char *path,u32 depth)
{
    /* SP+0..CF is uninitialized scratch. Name, path and lowercase outputs start
     * at 0,50,90; these offsets do not establish safe per-string capacities. */
    union {
        u16 name_prefix;
        char bytes[0xD0];
    } scratch_storage;
    char *scratch=scratch_storage.bytes;
    u8 *buffer;
    u32 result=0;
    while(func_00377820(1)!=0){}
    buffer=func_002AEB60(context->field24,7);
    if(buffer!=0){
        u32 remaining=0;
        const void *mode=&context->field28;
        result=1;
        do{
            u32 next_sector=sector+1u;
            u8 *record;
            u32 stride;
            /* The original directory loop retries ZERO. */
            while(func_00378258(sector,1,buffer,mode)==0){}
            while(func_00377820(1)!=0){}
            record=buffer;
            stride=((u32)record[1]<<8)|record[0];
            while(stride!=0){
                u32 extent=archive_le32(record+2);
                u32 size=archive_le32(record+0xA);
                u32 directory=record[0x19]&2;
                u32 first=record[0x21];
                u32 length=record[0x20];
                if(first!=1){
                    if(length==1){
                        u32 block_size=context->field24;
                        scratch_storage.name_prefix=D_004473D0;
                        remaining=(size+block_size-1u)/block_size-1u;
                    }else{
                        func_003934F8(scratch,(const char *)record+0x21,length);
                        scratch[length]=0;
                        if(directory!=0){
                            func_00393758(path,scratch);
                            ++depth;
                            func_00393758(path,D_004473D8);
                            func_002B15C8(context,extent,path,depth);
                        }else{
                            char *full_path=scratch+0x50;
                            GeorgeArchiveDirectoryRecord *entry;
                            func_00393B74(full_path,path);
                            func_00393C90(full_path,scratch,length-2u);
                            func_002AEAF0(context->field54);
                            entry=func_002AEC28(12);
                            if(entry!=0){
                                u32 key;
                                func_00393B74(scratch+0x90,full_path);
                                func_003984D8(scratch+0x90);
                                entry->field00=extent;
                                entry->field04=size;
                                key=func_0029C648((const signed char *)scratch+0x90);
                                entry->field08=key;
                                func_002A7CD0(context->field50,key,entry);
                            }
                            func_002AEAF0(0);
                        }
                    }
                }
                record=(u8 *)((u32)record+stride);
                stride=((u32)record[1]<<8)|record[0];
            }
            sector=next_sector;
            --remaining;
        }while(remaining!=0xFFFFFFFFu);
        func_002AEE40(buffer);
    }
    if(depth!=0){
        u32 length=func_00295050((const signed char *)path);
        if(length!=0){
            signed char *cursor=(signed char *)((u32)path+length-1u);
            if(*cursor==0x5C)cursor=(signed char *)((u32)cursor-1u);
            while((u32)path<(u32)cursor && *cursor!=0x5C)
                cursor=(signed char *)((u32)cursor-1u);
            if(*cursor==0x5C)cursor[1]=0;
            else *cursor=0;
        }
    }
    return result;
}

u32 func_002B1900(GeorgeArchiveContextView *context)
{
    char path[0x40];
    u8 *buffer;
    u32 result=0;
    func_00378038(0);
    buffer=func_002AEB60(context->field24,7);
    if(buffer!=0){
        const void *mode=&context->field28;
        /* Both loader loops retry NONZERO, as encoded in the original. */
        while(func_00378258(16,1,buffer,&context->field28)!=0){}
        while(func_00377820(1)!=0){}
        if(buffer[0]==1 && func_00393E48((const char *)buffer+1,D_004473E0,5)==0){
            if(func_00295050((const signed char *)context)==0 ||
               func_00393E48((const char *)buffer+0x28,(const char *)context,32)==0){
                u32 block_size=((u32)buffer[0x81]<<8)|buffer[0x80];
                if(block_size==2048){
                    u32 extent;
                    context->field24=block_size;
                    context->field2A=0;
                    extent=archive_le32(buffer+0x8C);
                    while(func_00378258(extent,1,buffer,mode)!=0){}
                    while(func_00377820(1)!=0){}
                    path[0]=D_004473C8[0];
                    extent=archive_le32(buffer+2);
                    result=func_002B15C8(context,extent,path,0)!=0;
                }
            }
        }
        func_002AEE40(buffer);
    }
    func_00378038(0);
    return result;
}

GeorgeArchiveContextView *func_002B1D30(const char *prefix,u32 argument5,u32 argument6)
{
    GeorgeArchiveContextView *context=func_002AEC28(0x58);
    if(context!=0){
        GeorgeHeap *heap=func_002AE830(0x3C000,D_004473E8,3);
        void *value;
        context->field20=0;
        context->field29=0;
        context->field2A=0;
        context->field24=2048;
        context->field54=heap;
        context->field28=255;
        context->field50=func_002A7F58(2023);
        context->field2C=0;
        context->field30=0;
        context->field34=0;
        context->field38=0;
        *(char *)context=D_004473C8[0];
        if(prefix!=0)func_00393B74((char *)context,prefix);
        while((value=func_002A7E68(context->field50,0))!=0)func_002AEE40(value);
        func_002A7FC8(context->field50);
        context->field20=func_002B1900(context);
        func_00295D30(D_004473F8);
        func_002C3BC8(0);
        func_002C8C38(3,7);
        func_00295D30(D_00447410);
        func_00295D30(D_00447428);
        func_002C35B0(400,16,argument5,argument6);
    }
    return context;
}

void func_002B1E88(GeorgeArchiveContextView *context)
{
    void *value;
    while((value=func_002A7E68(context->field50,0))!=0)func_002AEE40(value);
    func_002A7FC8(context->field50);
    func_002AEE40(context);
}

void func_002B0860(u32 mode,u32 argument5)
{
    if(D_003FD23C==0xFFFFFFFFu){
        GeorgeFileSlot *slot=D_00469BD0;
        u32 end=(u32)slot+20u*28u;
        D_003FD244=func_002B1D30(D_00447398,mode,argument5);
        D_003FD240=mode;
        D_003FD23C=0;
        slot->field00=0;
        while(1){
            slot->field04=0;
            slot->field08=0;
            slot->field0C=0;
            slot->field14=0;
            slot->field18=0;
            slot=(GeorgeFileSlot *)((u32)slot+28u);
            if(!((s32)(u32)slot<(s32)end))break;
            slot->field00=0;
        }
    }
}
