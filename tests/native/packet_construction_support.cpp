/* Complete unchanged SGI helper definitions instantiated for native observation.
 * No target helper award or original C++ class spelling is inferred. */
namespace std { struct nothrow_t; }
using std::nothrow_t;
#define malloc george_sgi_engine_allocate
#define free george_sgi_engine_release
#include "../../src/runtime/sgi/include/vector"
#undef free
#undef malloc
typedef __default_alloc_template<false,0> PacketAllocator;
template void *PacketAllocator::_S_refill(size_t);
template char *PacketAllocator::_S_chunk_alloc(size_t,int &);
template void *__malloc_alloc_template<0>::_S_oom_malloc(size_t);
template char *PacketAllocator::_S_start_free;
template char *PacketAllocator::_S_end_free;
template size_t PacketAllocator::_S_heap_size;
template PacketAllocator::_Obj *PacketAllocator::_S_free_list[16];
template void (*__malloc_alloc_template<0>::__malloc_alloc_oom_handler)();
extern "C" void packet_install_handler(void (*handler)()) { __malloc_alloc_template<0>::__set_malloc_handler(handler); }
