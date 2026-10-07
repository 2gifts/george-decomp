#include "george/actor_route_init.h"
#include "george/actor_route.h"
#include "george/actor_movement.h"
#include "george/geometry_bounds.h"
#include "george/route_setup.h"
#include "george/ee_math.h"

#define AT(type,base,offset) ((type *)((u32)(base)+(u32)(offset)))
#define FIELD(type,base,offset) (*AT(type,base,offset))

u32 func_001CD578(const GeorgeMathVec3 *point,u32 *tag,
                  GeorgeActorRouteRecord **record_output,
                  GeorgeActorRouteCollection **collection_output)
{
    GeorgeActorRouteQueryHeader *header;
    GeorgeActorRouteCollection *collection;
    u32 index;
    float best=1000000000.0f;
    *tag=0xFFFFFFFFU;
    header=(GeorgeActorRouteQueryHeader *)func_001CC710(point);
    collection=header!=NULL?header->field24:NULL;
    *collection_output=collection;
    if(collection==NULL) return 0;
    if(collection->field00==0) return 0;
    index=0;
    do {
        GeorgeActorRouteCollection *current=*collection_output;
        GeorgeActorRouteRecord *record=AT(GeorgeActorRouteRecord,current->field24,
                                          current->field20[index]);
        u32 inside=0;
        if((u32)record->field14-1U>=2U)
            inside=func_002A0390(point,record->field18,record->field17);
        if(inside!=0) {
            *record_output=record;
            *tag=header->field00|index;
            return 1;
        } else {
            float px=point->x,cx=record->field04.x;
            float cy=record->field04.y,py=point->y;
            float pz=point->z,cz=record->field04.z;
            float x=cx-px,y=cy-py,z=cz-pz;
            float distance=george_ee_square_root((x*x+y*y)+z*z);
            if(distance<best) {
                *record_output=record;
                best=distance;
                *tag=header->field00|index;
            }
        }
        ++index;
    } while(index<(*collection_output)->field00);
    return 0;
}

void func_001DB250(GeorgeActorRouteInitState *state)
{
    GeorgeGoalReferencedObject *reference;
    GeorgeMathVec3 *target;
    GeorgeActorRouteRecord *record;
    GeorgeActorRouteCollection *collection;
    GeorgeGoalOwner *owner;
    u32 result,owner_tag;
    state->fieldB8=0.75f;
    reference=state->fieldC0;
    state->field81=0xFF;
    state->field80=0;
    state->field74=0;
    state->field88=0;
    if(reference!=NULL && ((reference->field06&0x40U)!=0 || reference->field20.word!=0)) {
        const GeorgeMathVec3 *position=func_001CAFE0(state->fieldC0);
        state->fieldC4.x=position->x;
        state->fieldC4.y=position->y;
        state->fieldC4.z=position->z;
    }
    target=&state->fieldC4;
    result=func_001CD578(target,&state->field84,&record,&collection);
    owner=state->field00;
    state->field82=(u8)result;
    owner_tag=FIELD(u32,owner,0xC);
    if(owner_tag==0xFFFFFFFFU || state->field84==0xFFFFFFFFU) {
        state->field76=2;
        return;
    }
    if((u8)result==0 || state->field84!=owner_tag) {
        GeorgeMathVec3 zero;
        zero.x=0.0f;zero.y=0.0f;zero.z=0.0f;
        func_00177E48(owner->field08,&zero);
    }
    FIELD(u32,state,0xA4)=0;
    FIELD(u32,state,0xAC)=0;
    FIELD(u32,state,0xA8)=0;
    owner=state->field00;
    state->field77&=0xFEU;
    {
        const GeorgeMathVec3 *position=&owner->field08->fieldD0;
        state->field98.x=position->x;
        state->field98.y=position->y;
        state->field98.z=position->z;
    }
    owner=state->field00;
    {
        u32 tag=state->field84;
        GeorgeGoalEntity *entity=owner->field08;
        void *setup_owner=state->field10;
        u32 first=FIELD(u32,owner,0xC);
        func_0020BEA8(setup_owner,AT(void,state,0x14),12,first,tag,
                       AT(GeorgeMathVec3,entity,0x40),target);
    }
    func_001CC628(state->field10);
    state->field76=0;
    state->field75=0;
}

u32 func_0020D190(GeorgeActorRouteCompletion *owner,u8 *output)
{
    u8 flags,step;
    *output=0xFF;
    flags=owner->field00;
    if((flags&4U)==0 || (flags&2U)!=0) return 0;
    step=owner->fieldCB4;
    owner->field00=(u8)(flags&0xFBU);
    *output=step;
    return 1;
}

#undef FIELD
#undef AT
