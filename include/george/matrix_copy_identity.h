/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# (c) 2005 Naomi Peori <naomi@peori.ca>
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
*/

/* Modified work: extracted matrix_unit and its MATRIX declaration from PS2SDK
 * commit 120aaba7d4df42e251840fc3e46b4b57308307a8 for George Decomp.
 * The complete method/typedef are unchanged; this is a new translation unit,
 * not the unchanged whole upstream file. Licensed under Academic Free License
 * version 2.0. Full unmodified terms: LICENSES/PS2SDK-AFL-2.0.txt.
 * Exercise/distribution of this licensed source indicates acceptance of those
 * terms; retain the copyright, license and modified-work attribution notices.
 */

#ifndef GEORGE_MATRIX_COPY_IDENTITY_H
#define GEORGE_MATRIX_COPY_IDENTITY_H

typedef float MATRIX[16] __attribute__((__aligned__(16)));

void func_002A1C08(void *output, const void *input);
void matrix_unit(MATRIX output);

#endif
