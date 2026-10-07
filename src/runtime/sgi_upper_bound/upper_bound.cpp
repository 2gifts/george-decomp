/* Genuine explicit instantiation of the complete unchanged HP/SGI helper.
 * The original type names, compiler and template spelling remain unknown.
 * All algorithm definitions and complete notices are retained in the imported
 * headers. See LICENSES/SGI-STL.txt and the existing pinned provenance.
 * This replaces already recovered C algorithms; it adds no new functions.
 */

/* The same declaration-only historical type lookup compatibility as the
 * reviewed resolver TU; no algorithm/header/keyword is changed. */
namespace std { struct nothrow_t; }
using std::nothrow_t;

#include "hash_map"

template void **__upper_bound<void **, void *,
    int (*)(const void *, const void *), int>(
    void **, void **, void *const &,
    int (*)(const void *, const void *), int *);
