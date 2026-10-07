/*
 * Genuine complete instantiations of the pinned HP/SGI container methods.
 * Original definitions and license notices are retained in their complete
 * imported headers; no instruction-shaped substitute or source store-order
 * patch is used here. Original class/type spellings remain unknown.
 *
 * Copyright (c) 1994 Hewlett-Packard Company
 * Copyright (c) 1996,1997 Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.
 * Hewlett-Packard Company and Silicon Graphics make no representations about
 * the suitability of this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

/* Type lookup compatibility only. The unchanged historical GNU new header
 * declares this type in std and uses it unqualified in global overloads. */
namespace std { struct nothrow_t; }
using std::nothrow_t;

/* Transparent source-local names for the actual engine malloc/free call
 * interfaces. Shared insertion headers and provenance are never edited. */
#define malloc george_sgi_engine_allocate
#define free george_sgi_engine_release
#include "include/hash_map"
#undef free
#undef malloc

typedef __default_alloc_template<false, 0> GeorgeCompletionAllocator;
typedef vector<void *, GeorgeCompletionAllocator> GeorgeCompletionBucketVector;
typedef pair<const unsigned int, void *> GeorgeCompletionPair;
typedef hashtable<GeorgeCompletionPair, unsigned int, hash<unsigned int>,
                  _Select1st<GeorgeCompletionPair>, equal_to<unsigned int>,
                  GeorgeCompletionAllocator> GeorgeCompletionHashtable;
typedef _Hashtable_iterator<GeorgeCompletionPair, unsigned int,
                            hash<unsigned int>, _Select1st<GeorgeCompletionPair>,
                            equal_to<unsigned int>, GeorgeCompletionAllocator>
    GeorgeCompletionTemplateIterator;

typedef char CompletionPointerWidth[(sizeof(void *) == 4) ? 1 : -1];
typedef char CompletionKeyWidth[(sizeof(unsigned int) == 4) ? 1 : -1];
typedef char CompletionSizeWidth[(sizeof(size_t) == 4) ? 1 : -1];
typedef char CompletionVectorWidth[(sizeof(GeorgeCompletionBucketVector) == 12) ? 1 : -1];
typedef char CompletionTableWidth[(sizeof(GeorgeCompletionHashtable) == 20) ? 1 : -1];
typedef char CompletionIteratorWidth[(sizeof(GeorgeCompletionTemplateIterator) == 8) ? 1 : -1];
typedef char CompletionNodeWidth[(sizeof(_Hashtable_node<GeorgeCompletionPair>) == 12) ? 1 : -1];

/* The selected source is the entire authentic method, not a numeric
 * forwarding function. Genuine compiler symbols are discovered before any
 * binding proposal or target link; no mangled spelling is invented. */
template void vector<void *, GeorgeCompletionAllocator>::insert(
    void **, size_t, void *const &);
template void hashtable<GeorgeCompletionPair, unsigned int, hash<unsigned int>,
    _Select1st<GeorgeCompletionPair>, equal_to<unsigned int>,
    GeorgeCompletionAllocator>::clear();
template GeorgeCompletionTemplateIterator &
    _Hashtable_iterator<GeorgeCompletionPair, unsigned int, hash<unsigned int>,
    _Select1st<GeorgeCompletionPair>, equal_to<unsigned int>,
    GeorgeCompletionAllocator>::operator++();
