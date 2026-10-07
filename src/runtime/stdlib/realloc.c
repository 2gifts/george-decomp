/* realloc.c -- a wrapper for realloc_r.  */

/*
 * Copyright (c) 1994, 1997 Cygnus Solutions.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * at Cygnus Solutions.  Cygnus Solutions may not be used to
 * endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Modified 2026-10-06 for george-decomp: add the observed retail lock and
 * unlock calls. Keep three separately evaluated _REENT expressions and
 * retain the allocation result across unlock. This is an adaptation of
 * the licensed newlib baseline, not a claim of unchanged retail source.
 */

#include <_ansi.h>
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>

#ifndef _REENT_ONLY

extern void __malloc_lock _PARAMS ((struct _reent *));
extern void __malloc_unlock _PARAMS ((struct _reent *));

_PTR
_DEFUN (realloc, (ap, nbytes),
	_PTR ap _AND
	size_t nbytes)
{
  _PTR result;
  __malloc_lock (_REENT);
  result = _realloc_r (_REENT, ap, nbytes);
  __malloc_unlock (_REENT);
  return result;
}

#endif
