#include "george/road_queries.h"

#define AT(type,object,offset) ((type *)((u32)(object)+(u32)(offset)))
#define FIELD(type,object,offset) (*AT(type,object,offset))
#if __GNUC__ >= 3
#define ROAD_INLINE static __inline__ __attribute__((always_inline))
#else
#define ROAD_INLINE static __inline__
#endif

extern float func_0037AF00(float value);
extern u32 func_00374748(float value);

void *func_001CC710(const GeorgeMathVec3 *point)
{
    u32 index=0;
    if(FIELD(u8,D_003F8C28,0)!=0) {
        do {
            void *provider=FIELD(void *,D_003F8C28,8U+index*4U);
            if((FIELD(u8,provider,2)&4U)!=0) {
                void *header=FIELD(void *,provider,0x10);
                if(func_002A00A0(AT(GeorgeBounds,header,4),point)!=0)
                    return header;
            }
            ++index;
        } while(index<FIELD(u8,D_003F8C28,0));
    }
    return NULL;
}

/* The published spatial polygon bodies use this same strict tiny-edge/XZ
 * arithmetic. Here there are exactly four copied vertices and no Y gate. */
ROAD_INLINE float quad_side(const GeorgeMathVec3 *point,
                            const GeorgeMathVec3 *quad)
{
    s32 index;
    float side=0.0f;
    for(index=0;index<4;++index) {
        s32 next=(index+1)%4;
        float z=quad[index].z,x=quad[index].x;
        float dz=quad[next].z-z;
        float negative_dx=x-quad[next].x;
        if(-0.10000000149011612f<dz && dz<0.10000000149011612f &&
           -0.10000000149011612f<negative_dx && negative_dx<0.10000000149011612f)
            continue;
        {
            float point_x=point->x,point_z=point->z;
            float current_z=quad[index].z,current_x=quad[index].x;
            float z_product=(point_z-current_z)*negative_dx;
            float x_product=(point_x-current_x)*dz;
            side=x_product+z_product;
        }
        if(0.0f<side) break;
    }
    return side;
}

ROAD_INLINE void copy_vertex(GeorgeMathVec3 *out,const GeorgeMathVec3 *in)
{
    out->x=in->x;
    out->y=in->y;
    out->z=in->z;
}

ROAD_INLINE void make_quad(GeorgeMathVec3 *quad,void *record,
                           const GeorgeMathVec3 *positions,u32 first,u32 second,
                           u32 index,u32 next)
{
    u32 base=FIELD(u16,record,first);
    copy_vertex(quad,positions+base+index);
    base=FIELD(u16,record,second);
    copy_vertex(quad+1,positions+base+index);
    base=FIELD(u16,record,second);
    copy_vertex(quad+2,positions+base+next);
    base=FIELD(u16,record,first);
    copy_vertex(quad+3,positions+base+next);
}

float func_001CE620(const GeorgeMathVec3 *point,GeorgeGoalRoad *road,
                   s32 input_index,s32 input_count,u8 *vertex,s32 *mode,
                   float threshold)
{
    s32 count=(s32)(s16)input_count-1,index=0;
    u32 offset=(u32)(s32)(s16)input_index*0x34U;
    GeorgeMathVec3 quad[4];
    if(count<=0) return threshold;
    do {
        void *record=AT(u8,FIELD(void *,road,0x38),offset);
        const GeorgeMathVec3 *positions=FIELD(const GeorgeMathVec3 *,road,0x48);
        u32 current=(u8)index,next=(u8)(index+1);
        float side;
        make_quad(quad,record,positions,0xC,8,current,next);
        side=quad_side(point,quad);
        if(side<threshold) {
            if(mode!=NULL) {
                float second_side;
                record=AT(u8,FIELD(void *,road,0x38),offset);
                positions=FIELD(const GeorgeMathVec3 *,road,0x48);
                make_quad(quad,record,positions,0xE,0xA,current,next);
                second_side=quad_side(point,quad);
                *mode=second_side<=0.0f?1:0;
            }
            *vertex=(u8)index;
            threshold=side;
        }
        ++index;
    } while(index<count);
    return threshold;
}

u16 *func_001CEA80(const GeorgeMathVec3 *point,GeorgeGoalRoad **output_road,
                 u8 *output_vertex,s32 *output_mode,float radius)
{
    void *header;
    GeorgeGoalRoad *road,*previous_road;
    GeorgeRoadQueryCell *cell=NULL;
    u16 *closest=NULL;
    GeorgeMathVec3 relative;
    u32 width,columns,x,z,index,previous_key;
    float threshold=radius*radius;
    header=func_001CC710(point);
    if(header==NULL) return NULL;
    road=FIELD(GeorgeGoalRoad *,header,0x2C);
    if(road==NULL) return NULL;
    relative.x=point->x-FIELD(float,header,4);
    relative.z=point->z-FIELD(float,header,0xC);
    relative.y=point->y-FIELD(float,header,8);
    width=func_00374748(func_0037AF00(FIELD(float,header,0x10)-FIELD(float,header,4)));
    columns=width/40U;
    if(width%40U!=0) ++columns;
    x=func_00374748(relative.x)/40U;
    z=func_00374748(relative.z)/40U;
    {
        GeorgeRegistryIndexRow *row=func_002A8250(FIELD(GeorgeRegistryIndex *,road,0x34),
                                                (const void *)(x+z*columns));
        if(row!=NULL) {
            cell=AT(GeorgeRoadQueryCell,FIELD(void *,road,0x30),row->word04);
            if(cell->field0C==0) {
                u32 count=cell->field00;
                u8 *data=AT(u8,cell,0x10);
                cell->field04=(u16 *)data;
                cell->field0C=1;
                cell->field08=data+(count+1U)*2U;
            }
        }
    }
    index=0;
    if(cell!=NULL) {
        while(index<cell->field00) {
            u16 byte_offset=cell->field04[(u16)index];
            u32 number=FIELD(u16,cell->field08,byte_offset);
            u16 *record=AT(u16,FIELD(void *,road,0x38),number*0x34U);
            float next=func_001CE620(point,road,(s16)number,FIELD(u8,record,5),
                                    output_vertex,output_mode,threshold);
            if(next<threshold) {
                threshold=next;
                closest=record;
            }
            ++index;
        }
        *output_road=road;
    }
    if(closest!=NULL) return closest;
    previous_key=0x0FFFFFFFU;
    previous_road=NULL;
    index=0;
    while(index<FIELD(u16,road,6)) {
        u32 key=FIELD(u32,FIELD(void *,road,0x24),index*4U);
        u32 mask=key&0x7FFF0000U;
        if(previous_key!=mask) {
            previous_key=mask;
            previous_road=func_001CCA28(key);
        }
        if(previous_road!=NULL) {
            s32 number=FIELD(s16,FIELD(void *,road,0x24),index*4U);
            u16 *record=AT(u16,FIELD(void *,previous_road,0x38),(u32)number*0x34U);
            float next=func_001CE620(point,previous_road,number,FIELD(u8,record,5),
                                    output_vertex,output_mode,threshold);
            if(next<threshold) {
                closest=record;
                threshold=next;
                *output_road=previous_road;
            }
        }
        ++index;
    }
    return closest;
}
