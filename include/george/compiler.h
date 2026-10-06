#ifndef GEORGE_COMPILER_H
#define GEORGE_COMPILER_H

/* Opt in per function/source recipe with -DGEORGE_USE_SAVE128.
 * The pinned GNU EE GCC 2.9 backend supports register_precision with a bare
 * register identifier. This preserves s0-s7 and fp as 128-bit values while
 * leaving ra at the backend's ordinary 64-bit precision.
 *
 * The repeated final s0 is intentional: this compiler's generic attribute
 * handler overwrites the first attribute's arguments when processing later
 * entries. Its MIPS backend appends the later register specifications. Repeat
 * s0 last to preserve that first register along with the appended entries.
 * See docs/reuse.md and the public pinned backend's mips.c/tree.c.
 */
#ifdef GEORGE_USE_SAVE128
#if !defined(__GNUC__) || __GNUC__ != 2 || __GNUC_MINOR__ != 9
#error GEORGE_USE_SAVE128 requires the pinned GNU EE GCC 2.9 compiler profile
#endif
#if !defined(R5900) && !defined(_R5900) && !defined(__R5900__)
#error GEORGE_USE_SAVE128 requires the R5900 target
#endif
#define GEORGE_SAVE128 \
    __attribute__((register_precision(s0,128), \
                   register_precision(s1,128), \
                   register_precision(s2,128), \
                   register_precision(s3,128), \
                   register_precision(s4,128), \
                   register_precision(s5,128), \
                   register_precision(s6,128), \
                   register_precision(s7,128), \
                   register_precision(fp,128), \
                   register_precision(s0,128)))
#else
#define GEORGE_SAVE128
#endif

#endif /* GEORGE_COMPILER_H */
