#include "george/file_operations.h"
#include "george/accessors.h"
#include "george/heap.h"
#include "george/string_registry.h"

/* Numeric original SDK interfaces. Public SDK resemblance is ABI evidence;
 * these declarations do not identify or recover their implementations. */
extern void func_00363AE0(s32 mode);
extern s32 func_00363C00(u32 token);
extern u32 func_00363C20(const GeorgeFileDmaPacket *packet,s32 count);
extern s32 func_003689B0(const char *path,s32 flags);
extern s32 func_00368C40(s32 descriptor);
extern s32 func_00368DB8(s32 descriptor,s32 offset,s32 whence);
extern s32 func_00368FF8(s32 descriptor,void *destination,s32 count);
extern s32 func_00369268(s32 descriptor,const void *source,s32 count);
extern void *func_002B1BA8(void *context,const char *path);

#include "file_operations_template.h"

FILE_OPEN_WRAPPER(func_002B10D8,0x602)
FILE_OPEN_WRAPPER(func_002B1138,1)
FILE_THREE_WORD_WRAPPER(func_002B1260,const void *,func_00369268)
FILE_THREE_WORD_WRAPPER(func_002B1408,s32,func_00368DB8)
#undef FILE_OPEN_WRAPPER
#undef FILE_THREE_WORD_WRAPPER

void func_002B1198(u32 encoded_descriptor)
{
    if(encoded_descriptor!=0){
        func_00363AE0(0);
        func_00368C40((s32)(encoded_descriptor-1u));
        func_00363AE0(0);
    }
}

s32 func_002B11F0(u32 encoded_descriptor,void *destination,s32 count)
{
    s32 result;
    if(count==0)return 0;
    func_00363AE0(0);
    result=func_00368FF8((s32)(encoded_descriptor-1u),destination,count);
    func_00363AE0(0);
    return result;
}

s32 func_002B1468(u32 encoded_descriptor)
{
    s32 descriptor=(s32)(encoded_descriptor-1u);
    s32 position,end;
    func_00363AE0(0);
    position=func_00368DB8(descriptor,0,1);
    func_00363AE0(0);
    func_00363AE0(0);
    end=func_00368DB8(descriptor,0,2);
    func_00363AE0(0);
    func_00363AE0(0);
    func_00368DB8(descriptor,position,0);
    func_00363AE0(0);
    return end;
}

s32 func_002B1510(const char *path)
{
    char resolved[0x400];
    s32 descriptor;
    u32 encoded=0;
    if(*(const signed char *)path!=0){
        func_002ABAE8(path,resolved);
        func_00363AE0(0);
        descriptor=func_003689B0(resolved,1);
        func_00363AE0(0);
        if(descriptor>=0)encoded=(u32)descriptor+1u;
    }
    if(encoded!=0){
        func_00363AE0(0);
        func_00368C40((s32)(encoded-1u));
        func_00363AE0(0);
        return 1;
    }
    return 0;
}

GeorgeFileSlot *func_002B0710(const char *path)
{
    GeorgeFileSlot *slot=0;
    s32 index;
    if(D_00469BD0[0].field00==0){
        slot=&D_00469BD0[0];
    }else{
        for(index=1;index<20;++index){
            if(D_00469BD0[index].field00==0){
                slot=&D_00469BD0[index];
                break;
            }
        }
    }
    D_003FD23C=D_003FD23C+1u;
    slot->field00=1;
    slot->field10=0;
    slot->field0C=0;
    if(D_003FD240!=0){
        void *record=func_002B1BA8(D_003FD244,path);
        slot->field04=(u32)record;
        if(record==0)goto failure;
        { u32 value=func_002B1AF0(record);
          const void *fresh_record=(const void *)slot->field04;
          slot->field10=value;
          slot->field08=func_002B1AF8(fresh_record); }
    }else{
        u32 descriptor=func_002B1138(path);
        slot->field04=descriptor;
        if(descriptor==0)goto failure;
        slot->field08=(u32)func_002B1468(descriptor);
    }
    if(slot->field04!=0)return slot;
failure:
    D_003FD23C=D_003FD23C-1u;
    slot->field18=0;
    slot->field00=0;
    slot->field04=0;
    slot->field08=0;
    slot->field0C=0;
    slot->field14=0;
    return 0;
}

u32 func_002B0F60(u32 encoded_descriptor,u32 destination,s32 count)
{
    u32 remaining=(u32)count,chunk=(u32)count,total=0;
    void *allocation;
    GeorgeFileDmaPacket packet;
    if(count<=0)return 0;
    for(;;){
        allocation=func_002AEB60(chunk,6);
        if(allocation!=0)break;
        chunk=(u32)((s32)chunk>>1)&~15u;
        if(chunk==0)return 0;
    }
    packet.source=(u32)allocation;
    packet.attribute=0;
    while((s32)remaining>0){
        s32 read_result=0;
        u32 token,partial;
        s32 retain_chunk;
        if(chunk!=0){
            func_00363AE0(0);
            read_result=func_00368FF8((s32)(encoded_descriptor-1u),allocation,(s32)chunk);
            func_00363AE0(0);
        }
        if(read_result==0)break;
        remaining=remaining-(u32)read_result;
        packet.destination=destination;
        packet.size=((u32)read_result+15u)&~15u;
        retain_chunk=(s32)chunk<(s32)remaining;
        destination=destination+(u32)read_result;
        partial=(u32)read_result&15u;
        total=total+(u32)read_result;
        do{token=func_00363C20(&packet,1);}while(token==0);
        while(func_00363C00(token)>=0){}
        if(!retain_chunk)chunk=remaining;
        if(partial!=0 && remaining!=0)break;
    }
    func_002AEE40(allocation);
    return total;
}
