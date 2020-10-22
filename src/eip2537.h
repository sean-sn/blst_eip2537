/*
 * Copyright Supranational LLC
 * Licensed under the Apache License, Version 2.0, see LICENSE for details.
 * SPDX-License-Identifier: Apache-2.0
 */

 /* Implements https://eips.ethereum.org/EIPS/eip-2537 */

#ifndef __EIP2537_H__
#define __EIP2537_H__

#include "blst.h"

/* TODO - not in blst.h bindings for some reason, in exports.c */
extern void blst_fp_to(blst_fp* ret, const blst_fp* a);
extern void blst_fp_from(blst_fp* ret, const blst_fp* a);

/*
Addresses - do we need to do anything here?
BLS12_G1ADD         0x0a
BLS12_G1MUL         0x0b
BLS12_G1MULTIEXP    0x0c
BLS12_G2ADD         0x0d
BLS12_G2MUL         0x0e
BLS12_G2MULTIEXP    0x0f
BLS12_PAIRING       0x10
BLS12_MAP_FP_TO_G1 	0x11
BLS12_MAP_FP2_TO_G2 0x12
*/

typedef enum {
  EIP2537_SUCCESS = 0,
  EIP2537_POINT_NOT_ON_CURVE,
  EIP2537_POINT_NOT_IN_SUBGROUP,
  EIP2537_INVALID_ELEMENT,
  EIP2537_ENCODING_ERROR,
  EIP2537_INVALID_LENGTH,
  EIP2537_EMPTY_INPUT,
} EIP2537_ERROR;

EIP2537_ERROR bls12_g1add(byte out[128], const byte in[256], size_t in_len);
EIP2537_ERROR bls12_g1mul(byte out[128], const byte in[160], size_t in_len);
EIP2537_ERROR bls12_g1multiexp(byte out[128], byte* in, size_t in_len);

EIP2537_ERROR bls12_g2add(byte out[256], const byte in[512], size_t in_len);
EIP2537_ERROR bls12_g2mul(byte out[256], const byte in[288], size_t in_len);
EIP2537_ERROR bls12_g2multiexp(byte out[256], byte* in, size_t in_len);

EIP2537_ERROR bls12_pairing(byte out[32], byte* in, size_t in_len);

EIP2537_ERROR bls12_map_fp_to_g1(byte out[128], const byte in[64],
                                 size_t in_len);
EIP2537_ERROR bls12_map_fp2_to_g2(byte out[256], const byte in[128],
                                  size_t in_len);


extern const uint64_t BLS12_G1ADD_GAS;
extern const uint64_t BLS12_G1MUL_GAS;
extern const uint64_t BLS12_G2ADD_GAS;
extern const uint64_t BLS12_G2MUL_GAS;
extern const uint64_t BLS12_PAIRING_BASE_GAS;
extern const uint64_t BLS12_PAIRING_PAIR_GAS;
extern const uint64_t BLS12_MAP_FP_TO_G1_GAS;
extern const uint64_t BLS12_MAP_FP2_TO_G2_GAS;
extern const uint64_t BLS12_MULTIEXP_MULTIPLIER_GAS;
extern const uint64_t BLS12_MULTIEXP_DISCOUNT_TABLE_LEN;
extern const uint64_t BLS12_MULTIEXP_DISCOUNT[128]; 

uint64_t bls12_g1add_gas();
uint64_t bls12_g1mul_gas();
uint64_t bls12_g1multiexp_gas(uint64_t input_len);
uint64_t bls12_g2add_gas();
uint64_t bls12_g2mul_gas();
uint64_t bls12_g2multiexp_gas(uint64_t input_len);
uint64_t bls12_pairing_gas(uint64_t input_len);
uint64_t bls12_map_fp_to_g1_gas();
uint64_t bls12_map_fp2_to_g2_gas();

#endif /* __EIP2537_H__ */
