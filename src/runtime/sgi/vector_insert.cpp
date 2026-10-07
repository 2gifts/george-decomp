/*
 * Genuine explicit instantiation of the pinned HP/SGI vector implementation.
 * Upstream definitions and their notices are retained without instruction-
 * shaped substitutes or changes to their pointer-publication assignments.
 * See this directory's provenance record and docs/vector_insert.md.
 *
 * Copyright (c) 1994 Hewlett-Packard Company
 * Copyright (c) 1996 Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.
 * Hewlett-Packard Company and Silicon Graphics make no representations about
 * the suitability of this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

/* Source-local names refer to the actual engine allocation/free wrappers,
 * rather than changing the project's central libc malloc/free identities.
 * The genuine stdlib declarations and complete template bodies see these
 * transparent symbol renames. Their selected original call interfaces are
 * separately proven in the source-free scope.
 */
/* Type lookup compatibility only: the unchanged historical GNU new header
 * declares this type in std, then uses it unqualified in global overloads. */
namespace std { struct nothrow_t; }
using std::nothrow_t;

#define malloc george_sgi_engine_allocate
#define free george_sgi_engine_release

#include "include/vector"

#undef free
#undef malloc

typedef __default_alloc_template<false, 0> GeorgeVectorInsertAllocator;
typedef vector<void *, GeorgeVectorInsertAllocator> GeorgeVectorInsertTemplate;

/* Assert target storage shape without asserting the original element spelling.
 * Four-byte pointer elements are trivial/POD in the unchanged type-traits.
 */
typedef char GeorgeVectorInsertPointerWidth[(sizeof(void *) == 4) ? 1 : -1];
typedef char GeorgeVectorInsertSizeWidth[(sizeof(size_t) == 4) ? 1 : -1];
typedef char GeorgeVectorInsertDifferenceWidth[(sizeof(ptrdiff_t) == 4) ? 1 : -1];
typedef char GeorgeVectorInsertRangeWidth[(sizeof(GeorgeVectorInsertTemplate) == 12) ? 1 : -1];

/* This emits the entire authentic method. No numeric forwarding body or
 * differently ordered replacement algorithm is a selected recovery source.
 */
template void vector<void *, GeorgeVectorInsertAllocator>::_M_insert_aux(
    void **, void *const &);
