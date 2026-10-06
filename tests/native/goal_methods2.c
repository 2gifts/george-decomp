#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../../src/game/goal_methods2.c"

/* Native 32-bit callbacks exercise observable paths and instruction-derived
 * expected values. This harness is ignored research, not target match proof. */
GeorgeGoalMember D_003F8D50[256], D_003F8DD8[256], D_003F8E38[256];
GeorgeGoalMember D_003F8E68[256], D_003F8EE0[256];
u32 D_003F8DD0;
static int checks, failures, calls, route_mode, member_mode;
static void *context_expected, *adjusted_seen, *active_goal;
static GeorgeGoalOwner *angle_owner;
static GeorgeGoalDrive *active_drive;
static GeorgeGoalIdle *active_idle;
static GeorgeMathVec3 target_change;
static float angle0, angle1;
static int change_angle_owner, predicate_value, helper_order, idle_return;
static int idle_signed_arg, idle_calls, random_calls, query_value, virtual_calls;
static int route_calls[4], mutate_predicate;
static u32 idle_words[4], idle_enabled, random_count;
static float idle_float, virtual_float;
static void *lookup_result;
static GeorgeGoalRouteResult route_result;
static GeorgeGoalRouteWord route_word;
static u32 *route_word14, *route_word20;
static u32 hash_seen, lookup_word;
static GeorgeMathVec3 scratch_output;
static GeorgeGoalOwner *resource_owner;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    ++failures; printf("FAIL line %d: %s\n", __LINE__, #condition); } } while (0)
#define CLOSE(a,b) (fabsf((a)-(b)) < 0.000001f)
#define SET_OWNER(goal,owner) ((goal)->base.links.unknown00=(u32)(owner))

float func_0029B940(float first, float second)
{
    angle0=first; angle1=second;
    if (change_angle_owner) {
        ((GeorgeGoalBase *)active_goal)->links.unknown00=(u32)angle_owner;
    }
    return 7.25f;
}
s32 func_001CF588(void *context,u32 word)
{
    ++route_calls[0]; CHECK(context==context_expected); CHECK(word==0x12345678u);
    if (route_mode==1) *route_word14=0xFFFFFFFFu;
    return route_mode==0;
}
s32 func_001D1760(void *context,u32 word,u32 byte)
{
    ++route_calls[1]; CHECK(context==context_expected);
    CHECK(word==0x87654321u); CHECK(byte==0x11u);
    return route_mode==2;
}
GeorgeGoalRouteResult *func_001CF340(void *context)
{
    ++route_calls[2]; CHECK(context==context_expected);
    return route_mode==3 ? &route_result : NULL;
}
GeorgeGoalRouteWord *func_001CE130(void *context,u8 *byte)
{
    ++route_calls[3]; CHECK(context==context_expected); CHECK(byte==(u8 *)route_word20);
    *byte=7;
    return route_mode==4 ? &route_word : NULL;
}
void func_001DDD70(GeorgeGoalDrive *goal) { CHECK(goal==active_drive); helper_order=1; goal->field44=1; }
void func_001DE6E0(GeorgeGoalDrive *goal) { CHECK(helper_order==1); helper_order=2; goal->field44=2; }
void func_001DDBB8(GeorgeGoalDrive *goal) { CHECK(helper_order==2); helper_order=3; goal->field44=3; }
s32 func_00177C40(GeorgeGoalEntity *entity,u32 a,u32 b,u32 c,u32 d,
                     s32 enabled,s32 byte,float value)
{
    CHECK(entity!=NULL); ++idle_calls;
    idle_words[0]=a;idle_words[1]=b;idle_words[2]=c;idle_words[3]=d;
    idle_enabled=enabled;idle_signed_arg=byte;idle_float=value;
    return idle_return;
}
s32 func_00397178(void)
{
    ++random_calls;
    if(random_count) active_idle->field90=random_count;
    return 17;
}
s32 func_001B69E8(void *context) { ++calls; CHECK(context==context_expected); return predicate_value; }
s32 func_00211BA8(void) { ++calls; return query_value; }
void *func_00239FD8(u32 word) { lookup_word=word; return &route_result; }
void *func_00238BA0(void *object,u32 hash) { CHECK(object==&route_result); hash_seen=hash; return lookup_result; }
void func_00121A80(void *object,GeorgeMathVec3 *position,GeorgeMathVec3 *scratch)
{
    CHECK(object==lookup_result); CHECK(position==&((GeorgeGoalInteractionPosition *)active_goal)->field18);
    scratch->x=19;scratch->y=20;scratch->z=21;
    scratch_output=*scratch;position->x=11;position->y=12;position->z=13;
}
GeorgeGoalOwner *func_001BA610(u32 word) { CHECK(word==0xCAFEu); return resource_owner; }
static double bits_double(GeorgeGoalBits64 bits) { union { double d;GeorgeGoalBits64 u; } v;v.u=bits;return v.d; }
GeorgeGoalBits64 func_00374848(float value) { union { double d;GeorgeGoalBits64 u; } v;v.d=value;return v.u; }
s32 func_00373250(GeorgeGoalBits64 a,GeorgeGoalBits64 b)
{
    double x=bits_double(a),y=bits_double(b);
    if(x!=x||y!=y)return 1;
    return x<y?-1:x>y?1:0;
}
GeorgeGoalBits64 func_00372CC0(GeorgeGoalBits64 a,GeorgeGoalBits64 b)
{
    union {double d;GeorgeGoalBits64 u;} v;v.d=bits_double(a)-bits_double(b);return v.u;
}
static void member_call(void *adjusted)
{
    ++calls;adjusted_seen=adjusted;
    if(member_mode==1)((GeorgeGoalLook *)active_goal)->field10=7;
    else if(member_mode==2)active_drive->field45=4;
    else if(member_mode==3) {
        GeorgeGoalWalkIntersection *goal=active_goal;
        goal->field11=3;
        *(GeorgeMathVec3 *)((u8 *)goal+0x28)=target_change;
    } else if(member_mode==4)((GeorgeGoalEnterVehicle *)active_goal)->field3E=2;
}
static s32 virtual_int(void *adjusted)
{
    ++virtual_calls;adjusted_seen=adjusted;
    if(mutate_predicate)active_drive->field8C=1;
    return predicate_value;
}
static void virtual_scalar(void *adjusted,float value) { ++virtual_calls;adjusted_seen=adjusted;virtual_float=value; }
static void virtual_void(void *adjusted) { ++virtual_calls;adjusted_seen=adjusted; }
static void direct_entry(GeorgeGoalMember *entry,s16 adjustment)
{
    entry->adjustment=adjustment;entry->selector=-1;entry->target.direct=member_call;
}
static void test_member_calls(void)
{
    GeorgeGoalLook *goal=calloc(1,0x20000);
    GeorgeGoalBits64 vtable[2];
    CHECK(goal!=NULL);active_goal=goal;member_mode=1;
    goal->field14=3;calls=0;
    D_003F8D50[3].selector=0;
    CHECK(func_001D9D10(goal)==0&&calls==0);
    direct_entry(&D_003F8D50[3],-8);
    CHECK(func_001D9D10(goal)==7&&calls==1);
    CHECK(adjusted_seen==(void *)((u32)goal-8u));
    CHECK(func_001D9D10(goal)==7&&calls==1);
    goal->field10=0;goal->base.links.field04=(GeorgeGameplayGoal *)vtable;
    vtable[1]=((GeorgeGoalBits64)(u32)member_call<<32)|32767u;
    D_003F8D50[3].selector=2;D_003F8D50[3].target.vtable_offset=4;
    D_003F8D50[3].adjustment=32767;
    CHECK(func_001D9D10(goal)==7&&calls==2);
    CHECK(adjusted_seen==(u8 *)goal+65534);
    free(goal);
}
static void test_timers(void)
{
    GeorgeGoalTimedAction goal;GeorgeGoalOwner owner;GeorgeGoalEntity entity;
    union {u32 bits;float value;} nan;
    memset(&goal,0,sizeof(goal));memset(&owner,0,sizeof(owner));memset(&entity,0,sizeof(entity));
    SET_OWNER(&goal,&owner);owner.field08=&entity;owner.field60=0.5f;owner.field24=0xFFFF;
    entity.field0C=0x12;goal.field10=0.25f;
    CHECK(func_001D99F8(&goal)==0&&goal.field19==0&&goal.field10==0.25f);
    entity.field0C=7;
    CHECK(func_001D99F8(&goal)==0&&goal.field19==1&&goal.field10==0.25f);
    CHECK(func_001D99F8(&goal)==1&&goal.field10==-0.25f&&owner.field24==0xFFF7);
    goal.field18=0;goal.field10=0.5f;owner.field24=0xFFFF;
    CHECK(func_001D99F8(&goal)==0&&goal.field10==0.0f&&owner.field24==0xFFFF);
    nan.bits=0x7FC00001u;goal.field10=nan.value;
    CHECK(func_001D99F8(&goal)==0&&goal.field10!=goal.field10&&owner.field24==0xFFFF);
    goal.field19=9;goal.field10=42;
    CHECK(func_001D99F8(&goal)==0&&goal.field10==42&&goal.field19==9);
    { GeorgeGoalWait wait;memset(&wait,0,sizeof(wait));SET_OWNER(&wait,&owner);
      *(float *)&wait.field14=0.25f;
      CHECK(func_001DC750(&wait)==0&&*(float *)&wait.field14==-0.25f);
      CHECK(func_001DC750(&wait)==1);*(float *)&wait.field14=nan.value;
      CHECK(func_001DC750(&wait)==1); }
    { union {GeorgeGoalBits64 align;u8 bytes[256];} aliased;
      GeorgeGoalTimedAction *same=(GeorgeGoalTimedAction *)(aliased.bytes+64);
      GeorgeGoalOwner *overlap=(GeorgeGoalOwner *)(aliased.bytes+44);
      union {u32 bits;float value;} negative;
      memset(&aliased,0,sizeof(aliased));SET_OWNER(same,overlap);
      same->field19=1;negative.bits=0xBF800008u;same->field10=negative.value;
      overlap->field60=0.0f;
      CHECK((void *)&overlap->field24==(void *)&same->field10);
      CHECK(func_001D99F8(same)==1&&same->field10==-1.0f);
    }
}
static void test_drive(void)
{
    GeorgeGoalDrive goal;GeorgeGoalVirtualObject resource;
    union {GeorgeGoalBits64 align;u8 bytes[0x200];} table;
    GeorgeGoalVirtualInt *entry=(GeorgeGoalVirtualInt *)(table.bytes+0x138);
    memset(&goal,0,sizeof(goal));memset(&resource,0,sizeof(resource));
    resource.field04=table.bytes;entry->adjustment=-8;entry->invoke=virtual_int;
    goal.field14=(u32)&resource;active_goal=&goal;active_drive=&goal;member_mode=2;
    direct_entry(&D_003F8DD8[3],12);predicate_value=1;mutate_predicate=0;D_003F8DD0=0xFFFFFFFFu;
    CHECK(func_001DC988(&goal)==4&&helper_order==3&&D_003F8DD0==0);
    CHECK(goal.field8C==1&&adjusted_seen==(u8 *)&goal+12);
    helper_order=0;CHECK(func_001DC988(&goal)==4&&helper_order==0&&D_003F8DD0==0);
    predicate_value=0;CHECK(func_001DC988(&goal)==4&&goal.field8C==0);
    predicate_value=1;mutate_predicate=1;D_003F8DD0=5;
    CHECK(func_001DC988(&goal)==4&&D_003F8DD0==5);mutate_predicate=0;
}
static void test_movement(void)
{
    GeorgeGoalOwner owner,other;GeorgeGoalEntity entity;
    GeorgeMathVec3 origin={2,99,3},target={5,-99,7};
    GeorgeGoalWalkIntersection walk;GeorgeGoalEnterVehicle enter;GeorgeGoalBase face;
    memset(&owner,0,sizeof(owner));memset(&other,0,sizeof(other));memset(&entity,0,sizeof(entity));
    owner.field08=&entity;other.field08=&entity;active_goal=&face;face.links.unknown00=(u32)&owner;
    change_angle_owner=0;
    CHECK(func_001DC178(&origin,&target)==7.25f&&CLOSE(angle0,0.16f)&&CLOSE(angle1,0.12f));
    target=origin;func_001DC178(&origin,&target);CHECK(angle0==0&&angle1==0);
    *(GeorgeMathVec3 *)entity.field40=origin;
    *(GeorgeMathVec3 *)(entity.field40+0xC)=(GeorgeMathVec3){3,100,4};
    entity.field0C=3;owner.field34=9;CHECK(func_001DB818(&face)==0&&owner.field34==9);
    entity.field0C=7;change_angle_owner=1;angle_owner=&other;
    CHECK(func_001DB818(&face)==0&&other.field34==7.25f&&CLOSE(other.field30,6.2831854820251465f));
    CHECK(owner.field34==9&&CLOSE(angle0,0.16f)&&CLOSE(angle1,0.12f));
    memset(&walk,0,sizeof(walk));SET_OWNER(&walk,&owner);walk.field10=2;
    active_goal=&walk;member_mode=3;target_change=(GeorgeMathVec3){5,500,7};
    direct_entry(&D_003F8E38[2],0);other.field34=0;
    CHECK(func_001DF630(&walk)==3&&other.field34==7.25f);
    CHECK(CLOSE(angle0,0.16f)&&CLOSE(angle1,0.12f));
    change_angle_owner=0;
    memset(&enter,0,sizeof(enter));active_goal=&enter;member_mode=4;enter.field3D=5;
    direct_entry(&D_003F8EE0[5],-4);
    CHECK(func_001E82D8(&enter)==2&&adjusted_seen==(u8 *)&enter-4);
}
static void test_routes(void)
{
    u32 a,b,c;int mode;context_expected=&a;route_word14=&b;route_word20=&c;
    route_result.field40=0x2468u;route_word.field00=0x1357u;
    for(mode=0;mode<=5;++mode) {
        a=0x12345678u;b=0x87654321u;c=0xAABBCC11u;route_mode=mode;
        memset(route_calls,0,sizeof(route_calls));
        {s32 result=func_001DC468(&a,&a,&b,&c);
         CHECK(result==(mode==0?0:mode==2?1:mode==3?0:mode==4?1:2));}
        CHECK(route_calls[0]==1);
        if(mode==0)CHECK(route_calls[1]==0&&route_calls[2]==0&&route_calls[3]==0);
        if(mode==1)CHECK(route_calls[1]==0&&b==0xFFFFFFFFu);
        if(mode==2)CHECK(route_calls[2]==0&&route_calls[3]==0);
        if(mode==3)CHECK(a==0x2468u&&route_calls[3]==0);
        if(mode==4)CHECK(b==0x1357u&&c==0xAABBCC07u);
        if(mode==5)CHECK(b==0x87654321u&&c==0xAABBCC07u);
    }
}
static void test_idle(void)
{
    GeorgeGoalIdle goal;GeorgeGoalOwner owner;GeorgeGoalEntity entity;u32 *record;
    memset(&goal,0,sizeof(goal));memset(&owner,0,sizeof(owner));memset(&entity,0,sizeof(entity));
    SET_OWNER(&goal,&owner);owner.field08=&entity;active_idle=&goal;
    goal.field98=2;record=(u32 *)((u8 *)&goal+0x30);
    record[0]=101;record[1]=202;record[2]=303;record[3]=404;
    goal.field9C=0xFE;idle_return=0;idle_calls=0;
    CHECK(func_001E07E0(&goal)==2&&goal.field9E==3&&idle_calls==1);
    CHECK(idle_words[0]==101&&idle_words[1]==202&&idle_words[2]==303&&idle_words[3]==404);
    CHECK(idle_enabled==1&&idle_signed_arg==-2&&idle_float==5);
    goal.field9E=1;entity.field0C=0xC;
    CHECK(func_001E07E0(&goal)==2&&goal.field9E==1&&idle_calls==1);
    entity.field0C=7;CHECK(func_001E07E0(&goal)==4&&goal.field9E==3&&idle_calls==2);
    goal.field9E=2;entity.field0C=0xB;CHECK(func_001E07E0(&goal)==4&&goal.field9E==2);
    entity.field0C=7;CHECK(func_001E07E0(&goal)==4&&goal.field9E==0);
    goal.field9E=3;goal.field90=3;goal.field94=1;goal.field98=2;goal.field9D=0;
    CHECK(func_001E07E0(&goal)==1&&goal.field98==0&&goal.field94==0&&goal.field9E==0);
    goal.field9E=3;goal.field98=0x7FFFFFFFu;goal.field94=0xFFFFFFFFu;goal.field9F=0;
    CHECK(func_001E07E0(&goal)==0&&goal.field98==(u32)-2&&goal.field94==0xFFFFFFFFu);
    goal.field9E=3;goal.field90=4;goal.field9D=1;goal.field94=1;random_count=3;random_calls=0;
    CHECK(func_001E07E0(&goal)==1&&goal.field98==2&&goal.field94==0&&random_calls==1);
    goal.field9E=0xFF;goal.field9F=0x80;CHECK(func_001E07E0(&goal)==-128&&goal.field9E==0xFF);
    random_count=0;
}
static void test_resource_paths(void)
{
    GeorgeGoalWords words;GeorgeGoalInteractionPosition interaction;
    GeorgeGoalOwner owner;GeorgeGoalEntity entity;GeorgeGoalBase stop;
    GeorgeGoalVehicle vehicle;GeorgeGoalVirtualObject horn;
    union {GeorgeGoalBits64 align;u8 bytes[0x200];} table;
    GeorgeGoalVirtualFloat *scalar=(GeorgeGoalVirtualFloat *)(table.bytes+0x20);
    GeorgeGoalVirtualVoid *horn_entry=(GeorgeGoalVirtualVoid *)(table.bytes+0x1E0);
    float speeds[]={0.5f,-0.5f,1.0f,-1.0f,2.0f,-2.0f};int i;
    memset(&owner,0,sizeof(owner));memset(&words,0,sizeof(words));memset(&entity,0,sizeof(entity));
    SET_OWNER(&words,&owner);owner.field08=&entity;
    words.field14=0;calls=0;CHECK(func_001E5A18(&words)==1&&calls==0);
    words.field14=1;context_expected=entity.field40;predicate_value=1;
    CHECK(func_001E5A18(&words)==0&&calls==1);predicate_value=0;CHECK(func_001E5A18(&words)==1);
    memset(&interaction,0,sizeof(interaction));interaction.field14=0xBEEFu;active_goal=&interaction;
    query_value=0;lookup_word=0;CHECK(func_001E6870(&interaction)==2&&lookup_word==0);
    query_value=1;lookup_result=NULL;CHECK(func_001E6870(&interaction)==2&&lookup_word==0xBEEFu);
    lookup_result=&route_word;CHECK(func_001E6870(&interaction)==1&&hash_seen==0x21F5A0EAu);
    CHECK(interaction.field18.x==11&&interaction.field18.y==12&&interaction.field18.z==13&&scratch_output.z==21);
    words.field10=0xCAFE;resource_owner=&owner;entity.field0C=0xE;
    CHECK(func_001E8900(&words)==0);entity.field0C=7;CHECK(func_001E8900(&words)==1);
    memset(&vehicle,0,sizeof(vehicle));vehicle.field04=table.bytes;owner.unknown00=(u32)&vehicle;
    stop.links.unknown00=(u32)&owner;scalar->adjustment=-4;scalar->invoke=virtual_scalar;
    for(i=0;i<6;++i) {
        vehicle.field18=speeds[i];virtual_calls=0;
        CHECK(func_001E89E0(&stop)==(i<2?1:0)&&virtual_calls==(i<2?1:0));
        if(i<2)CHECK(virtual_float==0&&adjusted_seen==(u8 *)&vehicle-4);
    }
    words.field10=(u32)&vehicle;virtual_calls=0;
    CHECK(func_001E8DE0(&words)==1&&virtual_calls==0);
    vehicle.field38=&horn;horn.field04=table.bytes;horn_entry->adjustment=8;horn_entry->invoke=virtual_void;
    CHECK(func_001E8DE0(&words)==1&&virtual_calls==1&&adjusted_seen==(u8 *)&horn+8);
}
int main(void)
{
    CHECK(sizeof(void *)==4);
    test_member_calls();test_timers();test_drive();test_movement();test_routes();test_idle();test_resource_paths();
    printf("%d native semantic checks, %d failures\n",checks,failures);
    return failures?1:0;
}
