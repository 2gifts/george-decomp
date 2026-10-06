/* Observable-path checks of recovered C; no original files are required. */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../../src/game/goal_methods3.c"

static int checks, failures;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    ++failures; printf("line %d: %s\n", __LINE__, #condition); } } while (0)
static int near(float actual, float expected) { return fabsf(actual - expected) < 0.0001f; }
static float from_bits(u32 bits) { union { float f; u32 u; } v; v.u = bits; return v.f; }
static u32 float_bits(float value) { union { float f; u32 u; } v; v.f = value; return v.u; }
static GoalBits64 double_bits(double value) { union { double d; GoalBits64 u; } v; v.d=value; return v.u; }
static double from_double(GoalBits64 bits) { union { double d; GoalBits64 u; } v; v.u=bits; return v.d; }

static u32 owner_storage[0xA0/4], other_owner_storage[0xA0/4];
static u32 entity_storage[0x760/4], other_entity_storage[0x760/4];
static GeorgeGoalOwner *owner = (GeorgeGoalOwner *)owner_storage;
static GeorgeGoalOwner *other_owner = (GeorgeGoalOwner *)other_owner_storage;
static GeorgeGoalEntity *entity = (GeorgeGoalEntity *)entity_storage;
static GeorgeGoalEntity *other_entity = (GeorgeGoalEntity *)other_entity_storage;
static GeorgeGoalRoadGeometry roads[2];
static GeorgeGoalIntersectionRecord records[2][4];
static GeorgeGoalRoadEntry road_entries[2][4];
static GeorgeMathVec3 vectors[2][8];
static u32 goal_storage[0x1000/4];
static GeorgeGoalMotion *active_motion;
static GeorgeGoalBase *active_goal;
static int lookup_calls, motion_calls, normalize_calls, scale_calls, state_calls;
static int mutate_lookup, mutate_motion, mutate_compare, soft_compares;
static GoalBits64 state_mask;
static s32 last_mode, last_reverse;
static u32 last_index, last_word;
static float last_scalar, distance = 2.0f, motion_distance = 2.0f;
static GeorgeMathVec3 blend_vector, sent_motion;
static int random_index;
static s32 random_values[3] = { 1, 2, 3 };
static u32 route_count, route_word, route_mode;
static u8 route_index;
static int virtual_count, virtual_state;
static GeorgeGoalEnterVehicle *virtual_goal;
static GeorgeGoalVirtualObject virtual_objects[2];
static u32 virtual_tables[2][0x1A8/4];
static GeorgeMathVec3 virtual_vectors[2];

GeorgeGoalRoad *func_001CCA28(u32 word)
{
    ++lookup_calls;
    if (mutate_lookup && active_goal) active_goal->links.unknown00 = (u32)other_owner;
    if (word >> 16 > 1) return NULL;
    return (GeorgeGoalRoad *)&roads[word >> 16];
}
s32 func_001CF7A0(GeorgeMathVec3 *out, GeorgeGoalRoadGeometry *road,
                 u32 word, u32 index, s32 mode, float scalar)
{
    (void)road; last_word=word; last_index=index; last_mode=mode; last_scalar=scalar;
    out->x=10; out->y=20; out->z=30; return 7;
}
s32 func_001CFBF8(void *out, u32 word, u32 index)
{ (void)out; last_word=word; last_index=index; return 1; }
u32 func_001D0B30(GeorgeGoalRoadGeometry *road, const GeorgeMathVec3 *position,
                  u32 word, u32 *next, s32 mode, GeorgeMathVec3 *out)
{
    (void)road; (void)position; (void)word; (void)next; last_mode=mode;
    out->x=100; out->y=200; out->z=300; return route_count;
}
u32 func_001D0370(GeorgeGoalRoadGeometry *road, u32 word, u32 next, s32 mode,
                  GeorgeMathVec3 *out, u32 *out_mode, u32 *out_word, u8 *out_index)
{
    (void)road; (void)word; (void)next; last_mode=mode;
    out->x=100; out->y=200; out->z=300;
    *out_mode=route_mode; *out_word=route_word; *out_index=route_index;
    return route_count;
}
void func_0018FD00(GeorgeGoalEntity *actor, GoalBits64 flags, s32 mode)
{ ++state_calls; state_mask=flags; CHECK(mode==0); actor->field0C=0x20; }
s32 func_00177E48(GeorgeGoalEntity *actor, const GeorgeMathVec3 *vector)
{
    ++motion_calls; CHECK(actor == (mutate_motion ? other_entity : entity));
    sent_motion=*vector;
    if (active_motion) active_motion->field18=motion_distance;
    if (mutate_motion && active_goal) active_goal->links.unknown00=(u32)other_owner;
    return 1;
}
s32 func_001773F0(GeorgeGoalEntity *actor, u32 object, u32 first, u32 second)
{
    CHECK(actor==entity); last_word=object; last_index=first; last_reverse=(s32)second;
    if (active_goal) active_goal->links.unknown00=(u32)other_owner;
    return virtual_state;
}
void func_001BAA68(u32 object, u32 word, s32 byte, s32 mode)
{
    last_word=word; last_index=object; last_mode=byte; CHECK(mode==0);
    if (active_goal) active_goal->links.unknown00=(u32)other_owner;
}
void func_002A3390(GeorgeMathVec3 *out, const GeorgeMathVec3 *current,
                  const GeorgeMathVec3 *target, float blend)
{ (void)current; (void)target; CHECK(blend==1); *out=blend_vector; }
float func_002A3538(GeorgeMathVec3 *vector)
{ ++normalize_calls; vector->x=1; vector->y=0; vector->z=0; return distance; }
float func_002A35C0(GeorgeMathVec3 *out, const GeorgeMathVec3 *in, float scale)
{
    GeorgeMathVec3 copy=*in; ++scale_calls; last_scalar=scale;
    out->x=copy.x*scale; out->y=copy.y*scale; out->z=copy.z*scale; return scale;
}
float func_0029C168(float angle) { return angle + 1.0f; }
float func_0029C090(float angle) { return angle + 2.0f; }
s32 func_00397178(void) { return random_values[random_index++ % 3]; }
GoalBits64 func_00374848(float value) { return double_bits(value); }
float func_003734F8(GoalBits64 value) { return (float)from_double(value); }
s32 func_00373250(GoalBits64 first, GoalBits64 second)
{
    double a=from_double(first), b=from_double(second);
    ++soft_compares;
    if (mutate_compare && soft_compares==1) {
        active_goal->links.unknown00=(u32)other_owner;
        ((GeorgeGoalLook *)active_goal)->field1C=0.5f;
    }
    return a<b ? -1 : a>b ? 1 : 0;
}
GoalBits64 func_00372CC0(GoalBits64 a, GoalBits64 b) { return double_bits(from_double(a)-from_double(b)); }
GoalBits64 func_00372C68(GoalBits64 a, GoalBits64 b) { return double_bits(from_double(a)+from_double(b)); }
GoalBits64 func_00372D28(GoalBits64 a, GoalBits64 b) { return double_bits(from_double(a)*from_double(b)); }
float func_001DC178(const GeorgeMathVec3 *a, const GeorgeMathVec3 *b)
{ (void)a; (void)b; if (active_goal) active_goal->links.unknown00=(u32)other_owner; return 1.25f; }

static const GeorgeMathVec3 *virtual_vector(void *adjusted)
{
    CHECK(adjusted == (void *)&virtual_objects[virtual_count]);
    if (virtual_count==0) virtual_goal->field14=(u32)&virtual_objects[1];
    return &virtual_vectors[virtual_count++];
}
static s32 virtual_integer(void *adjusted)
{ CHECK(adjusted==(void *)&virtual_objects[1]); return virtual_state; }

static void reset(void)
{
    int i;
    memset(owner_storage,0,sizeof(owner_storage)); memset(other_owner_storage,0,sizeof(other_owner_storage));
    memset(entity_storage,0,sizeof(entity_storage)); memset(other_entity_storage,0,sizeof(other_entity_storage));
    memset(goal_storage,0,sizeof(goal_storage)); memset(records,0,sizeof(records)); memset(vectors,0,sizeof(vectors));
    memset(road_entries,0,sizeof(road_entries));
    owner->field08=entity; other_owner->field08=other_entity;
    owner->field60=other_owner->field60=0.25f;
    entity->field0C=other_entity->field0C=0x20;
    for (i=0;i<2;++i) { roads[i].field38=records[i]; roads[i].field3C=road_entries[i]; roads[i].field48=vectors[i]; }
    active_goal=(GeorgeGoalBase *)goal_storage; active_goal->links.unknown00=(u32)owner;
    active_motion=NULL; lookup_calls=motion_calls=normalize_calls=scale_calls=state_calls=0;
    mutate_lookup=mutate_motion=mutate_compare=soft_compares=0;
    last_mode=last_reverse=0; last_index=last_word=0; last_scalar=0; distance=motion_distance=2;
    blend_vector.x=2; blend_vector.y=blend_vector.z=0;
    random_index=0; route_count=0; route_mode=0x123456FE; route_word=0x00010000; route_index=0xFF;
    virtual_count=0; virtual_state=1;
}

static void minimum_checks(void)
{
    /* Expected clamp(+1) encodings include both signs of quiet/signaling NaNs,
     * zeros, infinities, normals and smallest/largest subnormals. */
    static const u32 input[]={0,0x80000000,0x3F000000,0xBF000000,0x3F800000,0x40000000,
        0xC0000000,0x7F800000,0xFF800000,0x7FC12345,0xFFC12345,0x7F812345,0xFF812345,
        1,0x80000001,0x007FFFFF,0x807FFFFF,0x00800000,0x80800000};
    static const u32 expected[]={0,0x80000000,0x3F000000,0xBF000000,0x3F800000,0x3F800000,
        0xC0000000,0x3F800000,0xFF800000,0x3F800000,0xFFC12345,0x3F800000,0xFF812345,
        1,0x80000001,0x007FFFFF,0x807FFFFF,0x00800000,0x80800000};
    unsigned i;
    for(i=0;i<sizeof(input)/sizeof(input[0]);++i) CHECK(float_bits(george_ee_minimum(from_bits(input[i]),1.0f))==expected[i]);
    CHECK(float_bits(george_ee_minimum(-0.0f,0.0f))==0x80000000);
    CHECK(float_bits(george_ee_minimum(-1.0f,-2.0f))==0xC0000000);
    CHECK(float_bits(george_ee_minimum(-2.0f,-1.0f))==0xC0000000);
}

static void geometry_checks(void)
{
    GeorgeMathVec3 output; float *source;
    reset(); CHECK(func_001D17D8(0x10002)==&road_entries[1][2]); CHECK(func_001D17D8(0xFFFF0000)==NULL);
    CHECK(func_001D1820(&output,0x10002,257,-258,0.75f)==7);
    CHECK(last_word==0x10002 && last_index==1 && last_mode==-258 && last_scalar==0.75f);
    CHECK(func_001D1820(&output,0xFFFF0000,257,-258,0.75f)==0);
    records[0][0].field10=1; vectors[0][2].x=1; vectors[0][2].y=2; vectors[0][2].z=3;
    CHECK(func_001D18A8(&output,0,257,257)==1); CHECK(output.x==1&&output.y==2&&output.z==3);
    CHECK(func_001D18A8(&output,0,257,1)==1); CHECK(output.x==-1&&output.y==-2&&output.z==-3);
    records[0][0].field10=0; source=(float *)&vectors[0][0]; source[0]=1;source[1]=2;source[2]=3;source[3]=4;
    func_001D18A8((GeorgeMathVec3 *)(source+1),0,0,1);
    CHECK(source[1]==-1&&source[2]==-1&&source[3]==-1);
}

static void angle_checks(void)
{
    GeorgeGoalLook *goal=(GeorgeGoalLook *)goal_storage;
    reset(); CHECK(near(func_001DC200(0,1,0.5f),0.5f)); CHECK(near(func_001DC200(0,1,2),1));
    CHECK(near(func_001DC200(0,1,-1),-1)); CHECK(near(func_001DC200(3,-3,0.5f),3.1415927f));
    CHECK(near(func_001DC200(-3,3,0.5f),-3.1415927f));
    reset(); goal->field1C=0.5f; goal->field24=2;entity->field5C=0.55f;
    func_001D9DC8(goal); CHECK(goal->field14==1);CHECK(owner->field34==0.5f&&owner->field30==2);
    reset(); goal->field1C=0.5f;owner->field2C=1;entity->field5C=1;
    func_001D9DC8(goal);CHECK(goal->field14==0&&owner->field34==0);
    reset();goal->field18=0.3f;goal->field20=0.8f;entity->field5C=0.8f;
    func_001DA3C8(goal);CHECK(goal->field10==1&&owner->field34==0.3f);
    reset(); goal->field1C=1;entity->field5C=0;other_entity->field5C=0.55f;mutate_compare=1;
    func_001D9DC8(goal);CHECK(goal->field14==1);CHECK(active_goal->links.unknown00==(u32)other_owner);
}

static void motion_checks(void)
{
    GeorgeGoalMotion motion;
    reset();memset(&motion,0,sizeof(motion));motion.field1C=2;motion.field24=3;
    func_001DB968(owner,&motion); CHECK(motion.field18==4); CHECK(scale_calls==1&&last_scalar==6);
    CHECK(near(sent_motion.x,3)&&sent_motion.y==0&&sent_motion.z==0);CHECK(motion.field00.x==sent_motion.x);
    reset();memset(&motion,0,sizeof(motion));entity->field0C=0;
    owner->field24=0x20; ADDRESS(GeorgeMathVec3,owner,0x54)->x=2;
    ADDRESS(GeorgeMathVec3,owner,0x54)->y=3;ADDRESS(GeorgeMathVec3,owner,0x54)->z=4;
    ADDRESS(GeorgeMathVec3,entity,0x88)->y=1;motion.field1C=motion.field24=1;
    func_001DB968(owner,&motion);CHECK(state_calls==1&&state_mask==0x80000000000ULL);
    CHECK(sent_motion.x==2&&sent_motion.y==0&&sent_motion.z==4);
    reset();memset(&motion,0,sizeof(motion));owner->field24=0x80;
    func_001DB968(owner,&motion);CHECK(scale_calls==0&&sent_motion.x==0&&sent_motion.y==0&&sent_motion.z==0);
    reset();memset(&motion,0,sizeof(motion));blend_vector.x=from_bits(0x7FC12345);
    motion.field1C=motion.field24=1;func_001DB968(owner,&motion);CHECK(scale_calls==1&&isnan(sent_motion.x));
}

static void road_checks(void)
{
    GeorgeGoalRoadState *goal=(GeorgeGoalRoadState *)goal_storage;
    reset();goal->prefix.field12=2;goal->field54=0;OWNER_BYTE(owner,0x21)=7;
    func_001E0630(goal);CHECK(goal->prefix.field10==0&&goal->prefix.field12==0&&OWNER_BYTE(owner,0x21)==0);
    CHECK(near(goal->field54,0.6f)&&motion_calls==1&&sent_motion.x==0&&sent_motion.y==0&&sent_motion.z==0);
    reset();goal->prefix.field12=1;goal->field54=0.25f;
    func_001E0630(goal);CHECK(goal->field54==0&&goal->prefix.field10==0);
    goal->field54=from_bits(0x7FC12345);func_001E0630(goal);CHECK(isnan(goal->field54)&&goal->prefix.field10==0);
    reset();records[0][0].field05=4;OWNER_BYTE(owner,0x20)=0;OWNER_BYTE(owner,0x21)=0;
    vectors[0][1].x=1;func_001DFFE0(goal);
    CHECK(OWNER_BYTE(owner,0x20)==1&&goal->prefix.field10==1&&goal->prefix.field11==0);
    reset();records[0][0].field05=2;records[0][0].field1C=0xFFFFFFFE;
    func_001DFFE0(goal);CHECK(owner->field18.bits==0xFFFFFFFE&&goal->prefix.field11==1&&motion_calls==0);
    reset();records[0][0].field05=2;records[0][0].field1C=0x10000;records[1][0].field04=4;
    OWNER_BYTE(owner,0x22)=2;func_001DFFE0(goal);CHECK(OWNER_BYTE(owner,0x21)==1&&owner->field14==0);
    reset();records[0][0].field05=2;records[0][0].field1C=0x10000;records[1][0].field04=4;
    OWNER_BYTE(owner,0x22)=0xFE;func_001DFFE0(goal);CHECK(owner->field14==0x10000&&OWNER_BYTE(owner,0x20)==0);
    reset();entity->field0C=2;goal->field54=1.0f;goal->prefix.field12=0;
    func_001E0218(goal);CHECK(goal->prefix.field10==2&&goal->prefix.field12==1&&goal->field54==5&&motion_calls==0);
    reset();entity->field0C=0;goal->field54=1.0f;goal->prefix.field12=255;
    func_001E0218(goal);CHECK(goal->prefix.field10==2&&goal->prefix.field12==0&&near(goal->field54,1.2f));
    reset();active_motion=ROAD_MOTION(goal);motion_distance=2;goal->prefix.field10=1;
    goal->field3C.x=from_bits(0x7FC12345);func_001E0218(goal);CHECK(goal->prefix.field10==1&&owner->field18.bits==0xFFFFFFFF);
    reset();active_motion=ROAD_MOTION(goal);motion_distance=0.001f;goal->prefix.field10=1;
    goal->field3C.x=from_bits(0x7FC12345);func_001E0218(goal);CHECK(goal->prefix.field10==0);
}

static void intersection_checks(void)
{
    GeorgeGoalIntersectionState *goal=(GeorgeGoalIntersectionState *)goal_storage;
    reset();route_count=0;func_001DF728(goal);CHECK(random_index==0&&goal->field13==0&&goal->field10==0);
    reset();route_count=1;func_001DF728(goal);CHECK(random_index==3&&goal->field13==1&&goal->field12==1&&goal->field10==1);
    CHECK(near(goal->field44.x,1.0006283f)&&near(goal->field44.z,2.0012566f)&&goal->field44.y==0);
    reset();active_motion=&goal->field1C;motion_distance=0.5f;goal->field12=0;goal->field13=1;goal->field18=2;
    goal->field44.x=1;goal->field5C.x=10;goal->field5C.y=20;goal->field5C.z=30;
    func_001DF8D8(goal);CHECK(goal->field12==1&&goal->field1C.field0C.x==12&&near(goal->field1C.field0C.y,20.1f));
    reset();active_motion=&goal->field1C;motion_distance=0.5f;route_count=0;
    records[1][0].field18=7;records[1][0].field05=4;records[1][0].field04=0;
    *ADDRESS(u32,&road_entries[0][0],0x40)=8;
    func_001DF8D8(goal);CHECK(OWNER_BYTE(owner,0x21)==1&&OWNER_BYTE(owner,0x20)==3);
    CHECK(owner->field14==0x10000&&OWNER_SIGNED_BYTE(owner,0x22)==-2&&goal->field10==2);
    reset();active_motion=&goal->field1C;motion_distance=0.5f;route_count=1;route_index=0;
    func_001DF8D8(goal);CHECK(goal->field13==1&&goal->field12==1&&goal->field10==0);
}

static void enter_checks(void)
{
    GeorgeGoalEnterVehicle *goal=(GeorgeGoalEnterVehicle *)goal_storage;
    const GeorgeGoalVirtualVector vector_method={0,0,virtual_vector};
    const GeorgeGoalVirtualInt int_method={0,0,virtual_integer};
    reset();goal->field3F=1;ADDRESS(GeorgeMathVec3,goal,0x18)->x=7;
    func_001E8608(goal);CHECK(goal->field3F==0&&ENTER_MOTION(goal)->field0C.x==7&&goal->field3D==1);
    CHECK(other_owner->field34==1.25f&&near(other_owner->field30,6.2831855f)&&owner->field34==0);
    reset();*ADDRESS(u8,goal,0x3C)=255;goal->field14=0x87654321;
    func_001E86A8(goal);CHECK(last_word==0x87654321&&last_index==255&&last_reverse==255);
    CHECK((other_owner->field24&8)!=0&&goal->field3D==3&&owner->field24==0);
    reset();entity->field0C=0xE;*ADDRESS(u32,entity,0x750)=1;entity->field364=123;
    owner->field24=other_owner->field24=0xFFFF;goal->field42=0xFE;goal->field14=456;
    func_001E8708(goal);CHECK(last_index==123&&last_word==456&&last_mode==-2&&goal->field3E==1);
    CHECK(other_owner->field24==0xFFF7&&owner->field24==0xFFFF);
    reset();entity->field0C=0;owner->field24=0xFFFF;
    func_001E8708(goal);CHECK(owner->field24==0xFFF7&&goal->field3E==4);
    reset();virtual_goal=goal;
    virtual_objects[0].field04=(u8 *)virtual_tables[0];virtual_objects[1].field04=(u8 *)virtual_tables[1];
    memcpy((u8 *)virtual_tables[0]+0x38,&vector_method,sizeof(vector_method));
    memcpy((u8 *)virtual_tables[1]+0x38,&vector_method,sizeof(vector_method));
    memcpy((u8 *)virtual_tables[1]+0x1A0,&int_method,sizeof(int_method));
    virtual_vectors[0].x=2;virtual_vectors[0].y=virtual_vectors[0].z=0;
    virtual_vectors[1].x=2;virtual_vectors[1].y=virtual_vectors[1].z=0;
    goal->field14=(u32)&virtual_objects[0];goal->field40=1;virtual_state=0;
    active_motion=ENTER_MOTION(goal);motion_distance=0.5f;goal->field3F=0;
    func_001E8390(goal);CHECK(virtual_count==2&&goal->field3E==2&&goal->field3D==2&&motion_calls==1);
}

int main(void)
{
    minimum_checks();geometry_checks();angle_checks();motion_checks();road_checks();intersection_checks();enter_checks();
    printf("goal_methods3: %d checks, %d failures\n",checks,failures);
    return failures != 0;
}
