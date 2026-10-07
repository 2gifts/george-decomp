#ifndef GEORGE_GNU_EXCEPTION_QUERIES_H
#define GEORGE_GNU_EXCEPTION_QUERIES_H

/* Genuine GNU interfaces; no discovered retail incoming call is asserted.
 * The old hook requires a live current record and returns its value address.
 * Both interfaces obtain their slot through the mutable runtime provider. */
#ifdef __cplusplus
extern "C" {
#endif
void *__cp_exception_info(void);
#ifdef __cplusplus
}
namespace std { bool uncaught_exception(); }
#endif

#endif
