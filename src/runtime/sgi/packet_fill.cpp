/*
 * Genuine explicit instantiation emitted from the complete unchanged
 * HP/SGI fill_n definition. The byte value remains a const reference; no
 * captured-value memset or numeric forwarding implementation is supplied.
 * See the entire pinned headers and LICENSES/SGI-STL.txt for the notices.
 *
 * Copyright (c) 1994 Hewlett-Packard Company
 * Copyright (c) 1996-1998 Silicon Graphics Computer Systems, Inc.
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.
 * Hewlett-Packard Company and Silicon Graphics make no representations about
 * the suitability of this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

/* Type lookup compatibility for the unchanged historical GNU new header,
 * which declares this type in std and refers to it unqualified globally. */
namespace std { struct nothrow_t; }
using std::nothrow_t;

#include "include/stl_algobase.h"

typedef char PacketFillPointerWidth[(sizeof(unsigned char *) == 4) ? 1 : -1];
typedef char PacketFillCountWidth[(sizeof(unsigned int) == 4) ? 1 : -1];

template unsigned char *fill_n<unsigned char *, unsigned int, unsigned char>(
    unsigned char *, unsigned int, const unsigned char &);
