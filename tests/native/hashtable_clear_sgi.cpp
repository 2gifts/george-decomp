/* Separate genuine SGI object/lifetime reference. Complete algorithms and
 * notices remain in the unchanged pinned headers/TU. No target helper award. */
#include "../../src/runtime/sgi_resolver/completion_containers.cpp"

template char *__default_alloc_template<false,0>::_S_start_free;
template char *__default_alloc_template<false,0>::_S_end_free;
template size_t __default_alloc_template<false,0>::_S_heap_size;
template __default_alloc_template<false,0>::_Obj * volatile
    __default_alloc_template<false,0>::_S_free_list[16];
template void (*__malloc_alloc_template<0>::__malloc_alloc_oom_handler)();

extern "C" {
void clear_sgi_unexecuted(void);
/* Read-only linkage view of the genuine instantiated allocator storage;
 * it is distinct from the C adaptation's D_003F21B8. */
void *clear_sgi_head(void);

unsigned clear_sgi_reference(void)
{
    unsigned counts[]={0,1,2,3,6,12};unsigned total=0;
    for (unsigned c=0;c<6;++c) {
        GeorgeCompletionHashtable table(0,hash<unsigned>(),equal_to<unsigned>());
        typedef _Hashtable_node<GeorgeCompletionPair> Node;
        Node *saved[12];
        unsigned n=counts[c];
        for (unsigned i=0;i<n;++i) {
            unsigned key=53*i;
            table.insert_unique_noresize(GeorgeCompletionPair(key,(void *)&counts[c]));
            saved[i]=table.find(key)._M_cur;
        }
        void *old_head=clear_sgi_head();
        table.clear();
        if (table.size()!=0 || table.begin()!=table.end()) clear_sgi_unexecuted();
        ++total;
        /* Insertions prepend, so traversal visits n-1..0 and deallocation
         * prepends again. The final free chain is0..n-1 then old_head. */
        void *head=clear_sgi_head();
        if (n==0) {
            if (head!=old_head) clear_sgi_unexecuted();
            ++total;
        } else {
            if (head!=saved[0]) clear_sgi_unexecuted();
            ++total;
            /* The allocator overlays its own free-link storage after scalar
             * value destruction. Observe raw pointer representations through
             * byte copy, rather than dereference ended Node object lifetimes. */
            for (unsigned i=0;i<n;++i) {
                void *next=0;const unsigned char *p=(const unsigned char *)saved[i];
                unsigned char *q=(unsigned char *)&next;
                for (unsigned j=0;j<sizeof(next);++j) q[j]=p[j];
                if (next!=(i+1<n ? (void *)saved[i+1] : old_head)) clear_sgi_unexecuted();
                ++total;
            }
        }
    }
    return total;
}
}

/* Every historical null-handler diagnostic/exit path is deliberately
 * unreachable in this reference domain. No fabricated ostream semantics. */
ostream &ostream::operator<<(const char *) {clear_sgi_unexecuted();return *this;}
ostream &endl(ostream &s) {clear_sgi_unexecuted();return s;}
