#include "george/actor_route.h"

#define AT(type,base,offset) ((type *)((u32)(base)+(u32)(offset)))
#define FIELD(type,base,offset) (*AT(type,base,offset))

void func_001B7AB8(GeorgeActorRouteActor *actor,const GeorgeMathVec3 *point)
{
    GeorgeGoalRoad *road=NULL;
    GeorgeGoalOwner *owner=&actor->field34;
    u16 *result=func_001CEA80(point,&road,AT(u8,actor,0x54),NULL,5.0f);
    if(result!=NULL) {
        u32 number=FIELD(u16,result,0);
        const void *record=AT(void,FIELD(void *,road,0x38),number*0x34U);
        u32 index=FIELD(u16,record,0x10)+FIELD(u8,owner,0x20);
        const GeorgeMathVec3 *normal=AT(GeorgeMathVec3,FIELD(void *,road,0x48),index*12U);
        void *object=(void *)owner->field04;
        const GeorgeMathVec3 *reference;
        float nx,ny,nz,x,y,z,dot;
        if(object!=NULL) {
            const GeorgeActorRouteVirtualEntry *entry=AT(GeorgeActorRouteVirtualEntry,
                FIELD(void *,object,4),0x98);
            void *frame=entry->invoke(AT(void,object,entry->adjustment));
            reference=AT(GeorgeMathVec3,frame,0x20);
        } else reference=&owner->field08->fieldD0;
        /* The normal address survives the callback; its XYZ reads follow it. */
        nx=normal->x;ny=normal->y;
        x=reference->x;y=reference->y;z=reference->z;
        nz=normal->z;
        dot=(x*nx+y*ny)+z*nz;
        FIELD(u8,owner,0x21)=0.0f<dot?0:1;
        {
            u32 word=FIELD(u32,result,0);
            u32 vertex=FIELD(u8,actor,0x54);
            owner->field14=word;
            FIELD(u32,owner,0x1C)=0;
            FIELD(u8,owner,0x22)=0;
            func_001CFBF8(&owner->field80,word,vertex);
        }
    } else {
        GeorgeGoalOwner *reloaded;
        GeorgeMathVec3 *normal=AT(GeorgeMathVec3,actor,0xB4);
        float x,y,z,dx,dy,dz;
        FIELD(u32,owner,0x1C)=0;
        FIELD(u8,owner,0x22)=0;
        owner->field14=0xFFFFFFFFU;
        FIELD(u32,normal,0)=0;
        FIELD(u32,normal,8)=0;
        normal->y=1.0f;
        reloaded=func_001BA610((u32)actor);
        dx=reloaded->field3C.x;x=normal->x;
        y=normal->y;dy=reloaded->field3C.y;
        z=normal->z;dz=reloaded->field3C.z;
        owner->field8C=(x*dx+y*dy)+z*dz;
    }
}

void func_001E2D88(GeorgeActorRouteState *state,const GeorgeMathVec3 *point)
{
    GeorgeGoalReferencedObject *object=state->field110;
    GeorgeMathVec3 *target;
    GeorgeGoalRoad *road=NULL;
    u16 *first,*second;
    if(object!=NULL && ((object->field06&0x40U)!=0 || object->field20.word!=0)) {
        const GeorgeMathVec3 *position=func_001CAFE0(state->field110);
        state->field114.x=position->x;
        state->field114.y=position->y;
        state->field114.z=position->z;
    }
    target=&state->field114;
    first=func_001CEA80(point,&road,&state->field3F,NULL,10000.0f);
    second=func_001CEA80(target,&road,&state->field41,NULL,10000.0f);
    if(first==NULL || second==NULL) state->field3E=2;
    else {
        u32 first_word=FIELD(u32,first,0);
        void *owner=state->field10;
        state->field48=first_word;
        state->field4C=FIELD(u32,second,0);
        {
            u32 second_word=FIELD(u32,second,0);
            u32 fresh_first=FIELD(u32,first,0);
            func_0020D080(owner,AT(void,state,0x14),5,fresh_first,second_word,point,target);
        }
        func_001CC628(state->field10);
        state->field46|=1;
    }
}

s32 func_001CC628(void *object)
{
    if(FIELD(u8,D_003F8C28,1)>=30U) return 0;
    if(FIELD(u8,object,3)!=0) return 1;
    FIELD(u8,object,3)=1;
    {
        void *manager=D_003F8C28;
        u32 count=FIELD(u8,manager,1);
        FIELD(void *,manager,0x6CU+count*4U)=object;
    }
    {
        void *manager=D_003F8C28;
        FIELD(u8,manager,1)=(u8)(FIELD(u8,manager,1)+1U);
    }
    return 1;
}

#undef FIELD
#undef AT
