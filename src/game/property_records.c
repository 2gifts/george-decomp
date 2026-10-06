#include "george/property_records.h"

extern char *func_00393B74(char *output,const char *input);
extern float func_0029C168(float angle);

/* ADDU's observed low-word address arithmetic is retained. */
#define PROPERTY_ADDRESS(type,base,offset) ((type *)((u32)(base)+(u32)(offset)))
#define PAYLOAD(type,record) PROPERTY_ADDRESS(type,record,8)

void func_0029A508(void *object,GeorgePropertyRecord *record,u32 count,
                   const GeorgePropertyDescriptor *descriptors)
{
    while (record!=NULL&&record->kind!=0) {
        u32 code=0,index;
        s32 found=-1;
        if (count!=0) code=record->code;
        for (index=0;index<count;++index) {
            if (descriptors[index].code==code) { found=(s32)index;break; }
        }
        if (found>=0) {
            const GeorgePropertyDescriptor *entry=descriptors+found;
            float *destination;
            const float *source;
            float value,x,y,z,w;
            u32 offset,packed,mask,bits;
            switch (entry->type) {
            case 3:
                destination=PROPERTY_ADDRESS(float,object,entry->offset);
                source=PAYLOAD(float,record);
                destination[0]=source[0];
                destination[1]=source[1];
                destination[2]=source[2];
                break;
            case 7:
            case 8:
                destination=PROPERTY_ADDRESS(float,object,entry->offset);
                source=PAYLOAD(float,record);
                destination[0]=source[0];
                destination[1]=source[1];
                destination[2]=source[2];
                destination=PROPERTY_ADDRESS(float,object,entry->offset);
                destination[3]=source[3];
                destination[4]=source[4];
                destination[5]=source[5];
                destination[6]=source[6];
                break;
            case 6:
            case 9:
                offset=entry->offset;
                *PROPERTY_ADDRESS(u32,object,offset)=(u32)PAYLOAD(void,record);
                break;
            case 16:
                bits=entry->convert(PAYLOAD(void,record));
                *PROPERTY_ADDRESS(u32,object,entry->offset)=bits;
                break;
            case 5:
                func_00393B74(PROPERTY_ADDRESS(char,object,entry->offset),PAYLOAD(char,record));
                break;
            case 10:
                if (*PAYLOAD(u32,record)!=0) {
                    offset=entry->offset;mask=entry->mask;
                    bits=*PROPERTY_ADDRESS(u32,object,offset);
                    *PROPERTY_ADDRESS(u32,object,offset)=bits|mask;
                } else {
                    offset=entry->offset;mask=entry->mask;
                    bits=*PROPERTY_ADDRESS(u32,object,offset);
                    *PROPERTY_ADDRESS(u32,object,offset)=bits&~mask;
                }
                break;
            case 11:
                value=*PAYLOAD(float,record);
                offset=entry->offset;
                value=value*0.01745329424738884f;
                *PROPERTY_ADDRESS(float,object,offset)=value;
                break;
            case 12:
                value=func_0029C168(*PAYLOAD(float,record)*0.01745329424738884f);
                *PROPERTY_ADDRESS(float,object,entry->offset)=value;
                break;
            case 13:
                value=func_0029C168(*PAYLOAD(float,record)*0.0087266471236944199f);
                *PROPERTY_ADDRESS(float,object,entry->offset)=value;
                break;
            case 14:
                packed=*PAYLOAD(u32,record);
                destination=PROPERTY_ADDRESS(float,object,entry->offset);
                x=(float)(packed&0xFF)*0.0039215688593685627f;
                y=(float)((packed>>8)&0xFF)*0.0039215688593685627f;
                z=(float)((packed>>16)&0xFF)*0.0039215688593685627f;
                destination[0]=x;
                *PROPERTY_ADDRESS(float,object,entry->offset+4)=y;
                *PROPERTY_ADDRESS(float,object,entry->offset+8)=z;
                break;
            case 15:
                packed=*PAYLOAD(u32,record);
                destination=PROPERTY_ADDRESS(float,object,entry->offset);
                x=(float)(packed&0xFF)*0.0039215688593685627f;
                y=(float)((packed>>8)&0xFF)*0.0039215688593685627f;
                z=(float)((packed>>16)&0xFF)*0.0039215688593685627f;
                w=(float)(packed>>24)*0.0039215688593685627f;
                destination[0]=x;
                *PROPERTY_ADDRESS(float,object,entry->offset+4)=y;
                *PROPERTY_ADDRESS(float,object,entry->offset+8)=z;
                *PROPERTY_ADDRESS(float,object,entry->offset+16)=w;
                break;
            case 1:
            case 2:
            case 4:
            default:
                offset=entry->offset;
                bits=*PAYLOAD(u32,record);
                *PROPERTY_ADDRESS(u32,object,offset)=bits;
                break;
            }
        }
        record=(record->flags&0x80)?NULL:PROPERTY_ADDRESS(GeorgePropertyRecord,record,record->step);
    }
}

GeorgePropertyRecord *func_0029A890(const GeorgePropertyRecord *record)
{
    if (record->flags&0x80) return NULL;
    return PROPERTY_ADDRESS(GeorgePropertyRecord,record,record->step);
}

u32 func_0029A8B8(GeorgePropertyRecord *record)
{
    u32 result=func_002B2448(record->code);
    record->code=result;
    return result;
}

#undef PAYLOAD
#undef PROPERTY_ADDRESS
