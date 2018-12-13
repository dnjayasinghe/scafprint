/* mini-gmp, a minimalistic implementation of a GNU GMP subset.

Copyright 2011-2014 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

/* About mini-gmp: This is a minimal implementation of a subset of the
   GMP interface. It is intended for inclusion into applications which
   have modest bignums needs, as a fallback when the real GMP library
   is not installed.

   This file defines the public interface. */

#ifndef __MAIN_H__
#define __MAIN_H__

/* For size_t */
#include <stddef.h>

#if defined (__cplusplus)
extern "C" {


typedef struct ECPoint{
  mpz_t x;
  mpz_t y;
  mpz_t z;
} ECPoint;

void print(mpz_t X,char* str);
void print_ECPoint(ECPoint* e);
void EC_init(ECPoint* e,mpz_t x_coord, mpz_t y_coord, mpz_t z_coord);
void EC_copy(ECPoint* e,ECPoint* e2);
void EC_clear(ECPoint* e);
void addmod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod);
void submod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod);
void mulmod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod);
void mulmod_ui(mpz_t res,mpz_t a, unsigned int b, mpz_t mod);
void ecsm(ECPoint* res, ECPoint* P, mpz_t k, int size,mpz_t mod,mpz_t a);
void ecadd(ECPoint* res, ECPoint* P, ECPoint* Q,mpz_t mod);
void ecdbl(ECPoint* res, ECPoint* P,mpz_t mod,mpz_t a);
void EC_to_affine(ECPoint* e,mpz_t p,mpz_t mod);
int EC_is_equal_in_Fr(ECPoint* e, ECPoint* e1, mpz_t mod);
void EC_in(ECPoint* e, mpz_t mod);


#if defined (__cplusplus)
}
#endif
#endif /* __MAIN__ */
