/* Real initialized C++ vector objects and exact complete production template.
 * Allocator static data below is genuinely typed/instantiated from the primary
 * header. Byte aliases only initialize/observe its proved representation. */
#include "../../src/runtime/sgi/vector_insert.cpp"
#include "vector_insert_golden.h"
extern "C" int printf(const char *,...);
extern "C" void *malloc(size_t);
extern "C" void free(void *);
extern "C" void *insert_real_memmove(void *,const void *,size_t);
extern "C" int func_00100AA8(const unsigned int *,const unsigned int *);
extern "C" void abort(void);
template char *GeorgeVectorInsertAllocator::_S_start_free;
template char *GeorgeVectorInsertAllocator::_S_end_free;
template size_t GeorgeVectorInsertAllocator::_S_heap_size;
template GeorgeVectorInsertAllocator::_Obj *GeorgeVectorInsertAllocator::_S_free_list[16];
template void (*__malloc_alloc_template<0>::__malloc_alloc_oom_handler)();
extern unsigned char observed_heads[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE");
extern unsigned char observed_begin[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE");
extern unsigned char observed_end[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE");
extern unsigned char observed_size[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE");
typedef char NativePointerWidth[(sizeof(void *)==4)?1:-1];
typedef char NativeSizeWidth[(sizeof(size_t)==4)?1:-1];
typedef char NativeDifferenceWidth[(sizeof(ptrdiff_t)==4)?1:-1];
struct ActualVectorObserver : GeorgeVectorInsertTemplate {
    void bind(void **first,void **last,void **limit) { _M_start=first;_M_finish=last;_M_end_of_storage=limit; }
    void insert_aux(void **position,void *const &value) { _M_insert_aux(position,value); }
};
static void **arena;
static const unsigned int BASE=0x20000,WORDS=4096,VALUE=0x20080,OLD=0x20200,POOL=0x21000;
static const InsertCase *current;
static ActualVectorObserver *active;
static unsigned int events[400],event_count,allocations,copies,checks,case_index;
static unsigned int initial_word(unsigned int i) { return 0x5A830201u^(i*0x10203u); }
static void check(bool value) { ++checks;if(!value){printf("insert case%u check%u failed\n",case_index,checks);abort();} }
static unsigned int bits(const void *address) {
    const unsigned char *p=(const unsigned char *)address;
    return p[0]|((unsigned int)p[1]<<8)|((unsigned int)p[2]<<16)|((unsigned int)p[3]<<24);
}
static void store_bits(void *address,unsigned int value) {
    unsigned char *p=(unsigned char *)address;
    for(unsigned int i=0;i<4;++i)p[i]=(unsigned char)(value>>(8*i));
}
static unsigned int native_word(unsigned int value) {
    if(BASE<=value && value<BASE+WORDS*4)return (unsigned int)arena+value-BASE;
    return value;
}
static unsigned int guest_word(unsigned int value) {
    unsigned int start=(unsigned int)arena;
    if(start<=value && value<start+WORDS*4)return BASE+value-start;
    return value;
}
static void event(unsigned int kind,unsigned int a=0,unsigned int b=0,unsigned int c=0) {
    check(event_count+4<=400);events[event_count++]=kind;events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
}
static void handler2() { event(3,2); }
static void handler1() { event(3,1);__malloc_alloc_template<0>::__set_malloc_handler(handler2); }
extern "C" void *george_sgi_engine_allocate(size_t bytes) {
    ++allocations;event(1,bytes,allocations);
    if(current->p[7]&1)store_bits((char *)arena+(VALUE-BASE),0x6F005678);
    return allocations<=current->p[6]?0:(char *)arena+(POOL-BASE);
}
extern "C" void george_sgi_engine_release(void *memory) {
    check(guest_word((unsigned int)memory)==OLD);
    event(2,OLD,guest_word((unsigned int)active->end()),guest_word((unsigned int)active->begin()+active->capacity()*4));
}
/* Calls the complete unchanged licensed generic memmove. The after-copy hook
 * intentionally changes legitimate incoming-value and end objects, exercising
 * source captures/reloads. It is not a new engine callback implementation. */
extern "C" void *memmove(void *destination,const void *source,size_t bytes) {
    ++copies;event(4,guest_word((unsigned int)destination),guest_word((unsigned int)source),bytes);
    void *result=insert_real_memmove(destination,source,bytes);
    if(copies==1 && current->p[7]&2){
        store_bits((char *)arena+(VALUE-BASE),0x7F001234);
        if(current->p[1] && current->p[3]<current->p[1])
            active->bind(active->begin(),arena+(OLD-BASE)/4+current->p[1]-1,active->begin()+active->capacity());
    }
    return result;
}
static unsigned char *global_address(unsigned int i) {
    if(i>=1 && i<=16)return observed_heads+4*(i-1);
    return i==17?observed_begin:i==18?observed_end:observed_size;
}
int main() {
    arena=(void **)malloc(WORDS*sizeof(void *));check(arena!=0);
    for(unsigned int i=0;i<WORDS;++i)construct(arena+i,(void *)initial_word(i));
    for(case_index=0;case_index<sizeof(insert_cases)/sizeof(insert_cases[0]);++case_index){
        current=insert_cases+case_index;event_count=allocations=copies=0;
        for(unsigned int i=0;i<WORDS;++i)arena[i]=(void *)initial_word(i);
        for(unsigned int i=0;i<current->initial_count;++i)arena[current->initial[i].index]=(void *)native_word(current->initial[i].value);
        for(unsigned int i=1;i<20;++i)store_bits(global_address(i),i==19?current->globals_initial[i]:native_word(current->globals_initial[i]));
        __malloc_alloc_template<0>::__set_malloc_handler(handler1);
        ActualVectorObserver object;active=&object;
        object.bind((void **)native_word(bits(arena)),(void **)native_word(bits(arena+1)),(void **)native_word(bits(arena+2)));
        unsigned int result=0;
        if(current->p[0]==0){
            void **position=current->p[2]?arena+(OLD-BASE)/4+current->p[3]:0;
            unsigned int at=current->p[4]==0xFFFFFFFFu?(VALUE-BASE)/4:(OLD-BASE)/4+current->p[4];
            object.insert_aux(position,arena[at]);
            arena[0]=(void *)object.begin();arena[1]=(void *)object.end();arena[2]=(void *)(object.begin()+object.capacity());
        }else result=func_00100AA8((const unsigned int *)(arena+(OLD-BASE)/4),(const unsigned int *)((char *)arena+(VALUE-BASE)));
        check(result==current->result);check(event_count==current->event_count);
        for(unsigned int i=0;i<event_count;++i)check(events[i]==current->events[i]);
        for(unsigned int i=0;i<WORDS;++i){
            unsigned int expected=initial_word(i);
            for(unsigned int j=0;j<current->initial_count;++j)if(current->initial[j].index==i)expected=current->initial[j].value;
            for(unsigned int j=0;j<current->change_count;++j)if(current->changes[j].index==i)expected=current->changes[j].value;
            check(guest_word(bits(arena+i))==expected);
        }
        check(current->globals_expected[0]==0xF1200000u || current->globals_expected[0]==0xF1200010u);
        check(__malloc_alloc_template<0>::__set_malloc_handler(handler1)==(current->globals_expected[0]==0xF1200000u?handler1:handler2));
        for(unsigned int i=1;i<20;++i)check((i==19?bits(global_address(i)):guest_word(bits(global_address(i))))==current->globals_expected[i]);
        /* Suppress unrelated teardown behavior after observing the selected
         * operation. The genuine destructor then sees an empty range. */
        object.bind(0,0,0);active=0;
    }
    destroy(arena,arena+WORDS);free(arena);
    printf("vector_insert: %u checks\n",checks);return 0;
}
