#ifndef GEORGE_SGI_UPPER_BOUND_H
#define GEORGE_SGI_UPPER_BOUND_H

/* Declaration-only compatible numeric view for measured call probes.
 * The complete genuine method is defined in unchanged imported SGI headers.
 * Original type/class/template names remain unknown. The first four lanes
 * transfer begin/end/key-slot/predicate; the fifth type pointer is unused.
 * Initialized aligned same-array signed32 count and callable predicate only.
 */
typedef int (*GeorgeSGIUpperCompare)(const void *, const void *);

#ifdef __cplusplus
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Compare __comp, _Distance*);
#endif

#endif
