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

#include "george/matrix_copy_identity.h"
#include <string.h>

 void matrix_unit(MATRIX output) {

  // Create a unit matrix.
  memset(output, 0, sizeof(MATRIX));
  output[0x00] = 1.00f;
  output[0x05] = 1.00f;
  output[0x0A] = 1.00f;
  output[0x0F] = 1.00f;

 }
