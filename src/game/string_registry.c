#include "george/string_registry.h"
#include "george/heap.h"

extern u32 func_00295050(const char *text);
extern void *func_003934F8(void *destination,const void *source,u32 size);
extern char *func_00393B74(char *destination,const char *source);
extern char *func_00393758(char *destination,const char *source);
extern s32 func_00393A28(const char *left,const char *right);
extern char *func_003984D8(char *text);
extern char *func_00398628(const char *text,const char *needle);

#define REGISTRY_PAIR(index) ((GeorgeStringRegistryPair *)((u32)D_003FD1E0+((u32)(index)<<3)))

s32 func_002AB790(const char *key,const char *path)
{
    s32 ready=1;
    if(D_003FD1E0==0){
        D_003FD1D8=0;
        D_003FD1E0=func_002AEC28(80);
        if(D_003FD1E0!=0){
            D_003FD1DC=10;
            D_00469A00[0]=0;
            D_00469A80[0]=0;
            D_00469B00[0]=0;
        }
    }
    if((s32)D_003FD1D8 >= (s32)D_003FD1DC){
        GeorgeStringRegistryPair *table=func_002AEC28(D_003FD1DC<<4);
        ready=0;
        if(table!=0){
            func_003934F8(table,D_003FD1E0,D_003FD1D8<<3);
            func_002AEE40(D_003FD1E0);
            { u32 capacity=D_003FD1DC;
              D_003FD1E0=table;
              D_003FD1DC=capacity<<1; }
            ready=1;
        }
    }
    if(ready){
        u32 key_length=func_00295050(key);
        u32 path_length=func_00295050(path);
        char *key_copy=func_002AEC28(key_length+2u);
        char *path_copy=func_002AEC28(path_length+2u);
        ready=0;
        if(key_copy!=0 && path_copy!=0){
            func_00393B74(key_copy,key);
            func_00393B74(path_copy,path);
            func_003984D8(key_copy);
            func_003984D8(path_copy);
            /* Tests use the captured lengths and original input bytes. */
            if(*(const signed char *)((u32)key+key_length-1u)!=':')
                func_00393758(key_copy,D_00446FE8);
            { signed char end=*(const signed char *)((u32)path+path_length-1u);
              if(end!='/' && end!='\\')func_00393758(path_copy,D_00446FF0); }
            REGISTRY_PAIR(D_003FD1D8)->key=key_copy;
            REGISTRY_PAIR(D_003FD1D8)->path=path_copy;
            D_003FD1D8=D_003FD1D8+1u;
            ready=1;
        }else{
            if(key_copy!=0)func_002AEE40(key_copy);
            if(path_copy!=0)func_002AEE40(path_copy);
        }
    }
    return ready;
}

void func_002ABAE8(const char *input,char *output)
{
    /* Observed stack positions: text+0,prefix+400,saved RA+480.
     * Local representation does not establish safe original input capacities. */
    char scratch[0x480];
    char *text=scratch,*prefix=scratch+0x400;
    char *cursor,*destination,*tail;
    s32 has_prefix=0,matched=0,index;
    if(D_003FD1E0==0){
        D_003FD1D8=0;
        D_003FD1E0=func_002AEC28(80);
        if(D_003FD1E0!=0){
            D_003FD1DC=10;
            D_00469A80[0]=0;
            D_00469B00[0]=0;
            D_00469A00[0]=0;
        }
    }
    func_00393B74(text,input);
    func_003984D8(text);
    func_00393B74(output,D_00469A00);
    cursor=text;
    if((signed char)*cursor!=0){
        if((signed char)*cursor!=':'){
            ++cursor;
            while((signed char)*cursor!=0 && (signed char)*cursor!=':')++cursor;
        }
    }
    if((signed char)*cursor==':'){
        cursor=text;destination=prefix;
        /* A leading colon searches for a later colon, as in the original. */
        do{*destination++=*cursor++;}while((signed char)*cursor!=':');
        *destination=*cursor;
        destination[1]=0;
        has_prefix=1;
    }
    if(has_prefix && (s32)D_003FD1D8>0){
        index=0;
        do{
            if(func_00393A28(prefix,REGISTRY_PAIR(index)->key)==0){
                tail=text+1;
                if((signed char)text[0]!=':'){
                    signed char value;
                    do{value=(signed char)*tail++;}while(value!=':');
                }
                if((signed char)REGISTRY_PAIR(index)->path[1]!=':'){
                    func_00393758(output,D_00469A80);
                    func_00393758(output,D_00469B00);
                }
                matched=1;
                func_00393758(output,REGISTRY_PAIR(index)->path);
                func_00393758(output,tail);
                break;
            }
            index=(s32)((u32)index+1u);
        }while(index<(s32)D_003FD1D8);
    }
    if(!matched){
        if((signed char)text[1]==':'){
            func_00393B74(output,D_00469A00);
            func_00393758(output,text);
        }else if(func_00398628(text,D_00446FF8)==text){
            func_00393B74(output,text);
        }else{
            func_00393758(output,D_00469A80);
            func_00393758(output,D_00469B00);
            func_00393758(output,text);
        }
    }
    cursor=output;
    while((signed char)*cursor!=0){
        if((signed char)*cursor=='/')*cursor='\\';
        ++cursor;
    }
}

void func_002ABDE8(const char *text)
{
    D_00469A00[0]=0;
    if(text!=0)func_00393B74(D_00469A00,text);
}

void func_002ABEF8(void)
{
    if(D_003FD1E0==0){
        D_003FD1D8=0;
        D_003FD1E0=func_002AEC28(80);
        if(D_003FD1E0!=0){
            D_00469A00[0]=0;
            D_003FD1DC=10;
            D_00469A80[0]=0;
            D_00469B00[0]=0;
        }
    }
}
#undef REGISTRY_PAIR
