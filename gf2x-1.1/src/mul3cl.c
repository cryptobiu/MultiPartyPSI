/* This file is part of the gf2x library.

   Copyright 2007, 2008, 2009
   Richard Brent, Pierrick Gaudry, Emmanuel Thome', Paul Zimmermann

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program; see the file COPYING.  If not, write to the Free
   Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
   02111-1307, USA.
*/


#ifndef GF2X_MUL3_H_
#define GF2X_MUL3_H_

#include "gf2x.h"
/* All gf2x source files for lowlevel functions must include gf2x-small.h
 * This is mandatory for the tuning mechanism. */
#include "gf2x/gf2x-small.h"

#include <wmmintrin.h>

#if GF2X_WORDSIZE != 64
#error "This code is for 64-bit only"
#endif

#include "gf2x/gf2x-config.h"

#ifndef HAVE_PCLMUL_SUPPORT
#error "This code needs pclmul support"
#endif

/* TODO: if somebody comes up with a neat way to improve the interface so
 * as to remove the false dependency on pclmul, that would be nice.
 */
static inline __v2di
GF2X_FUNC(mul3cl_mul1) (unsigned long a, unsigned long b)
{
    __v2di aa = (__v2di) { a, 0 };
    __v2di bb = (__v2di) { b, 0 };
    return _mm_clmulepi64_si128(aa, bb, 0);
}

/* uses the variant of Karatsuba with 6 multiplications */
GF2X_STORAGE_CLASS_mul3
void gf2x_mul3 (unsigned long *c, const unsigned long *a, const unsigned long *b)
{
  unsigned long aa[3], bb[3];
  __v2di p0, p1, p2;
  __v2di pp0, pp1, pp2;
  aa[0] = a[1]^a[2];
  aa[1] = a[0]^a[2];
  aa[2] = a[0]^a[1];
  bb[0] = b[1]^b[2];
  bb[1] = b[0]^b[2];
  bb[2] = b[0]^b[1];
  p0  = GF2X_FUNC(mul3cl_mul1)(a[0], b[0]);
  p1  = GF2X_FUNC(mul3cl_mul1)(a[1], b[1]);
  p2  = GF2X_FUNC(mul3cl_mul1)(a[2], b[2]);
  pp0 = GF2X_FUNC(mul3cl_mul1)(aa[0], bb[0]);
  pp1 = GF2X_FUNC(mul3cl_mul1)(aa[1], bb[1]);
  pp2 = GF2X_FUNC(mul3cl_mul1)(aa[2], bb[2]);

  __v2di ce0, ce2, ce4;
  __v2di co1, co3;

  ce0 = p0;
  ce2 = p0^p1^p2^pp1;
  ce4 = p2;

  co1 = p0^p1^pp2;
  co3 = pp0^p1^p2;

  _mm_storeu_si128((__v2di*)(c),   ce0 ^ _mm_slli_si128(co1, 8));
  _mm_storeu_si128((__v2di*)(c+2), ce2 ^ _mm_srli_si128(co1, 8) ^ _mm_slli_si128(co3, 8));
  _mm_storeu_si128((__v2di*)(c+4), ce4 ^ _mm_srli_si128(co3, 8));
}

#endif  /* GF2X_MUL3_H_ */
