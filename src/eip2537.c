/*
 * Copyright Supranational LLC
 * Licensed under the Apache License, Version 2.0, see LICENSE for details.
 * SPDX-License-Identifier: Apache-2.0
 */

 /* Implements https://eips.ethereum.org/EIPS/eip-2537 */

#include "blst.h"
#include "eip2537.h"
#include "math.h"

/* Utility debug print functions */
#include <stdio.h>
static void print_value(unsigned int *value, size_t n, const char* name) {
    if (name != NULL)
        printf("%s = 0x", name);
    else
        printf("0x");
    while (n--)
        printf("%08x", value[n]);
    printf("\n");
}

void print_blst_fp(const blst_fp p, const char* name) {
    unsigned int value[384/32];
    blst_uint32_from_fp(value, &p);
    print_value(value, 384/32, name);
}

void print_blst_p1(const blst_p1* p, const char* name) {
    blst_p1_affine p_aff;

    if (name != NULL)
        printf("%s:\n", name);

    blst_p1_to_affine(&p_aff, p);
    print_blst_fp(p_aff.x, "  x");

    print_blst_fp(p_aff.y, "  y");
    printf("\n");
}

void print_blst_p1_affine(const blst_p1_affine* p_aff, const char* name) {
    if (name != NULL)
        printf("%s:\n", name);

    print_blst_fp(p_aff->x, "  x");
    print_blst_fp(p_aff->y, "  y");
    printf("\n");
}









/* Extract encoded field element byte array into blst_fp object */
/* TODO - need restrict? */
static int fp_from_bytes(blst_fp* restrict fp, const byte* in) {
  /* Ensure first 16 bytes of field element are zero */
  byte acc_pad = 0;

  for (size_t i = 0; i < 16; ++i) {
    acc_pad |= *in++;
  }

  if (acc_pad != 0) {
    return -1;
  }

  /* Next 48 bytes are field element in big endian form */
  limb_t limb = 0;
  size_t n = 48;

  while(n--) {
    limb <<= 8;
    limb |= *in++;
    fp->l[n / sizeof(limb_t)] = limb;
  }

  /* Check field element is less than modulus (also check if it's zero) */
  blst_fp zero = { 0 };
  blst_fp_add(&zero, fp, &zero);

  limb_t acc_mod  = 0;
  limb_t acc_zero = 0;
  for (size_t i = 0; i < (48 / sizeof(limb_t)); ++i) {
    acc_mod  |= zero.l[i] ^ fp->l[i];
    acc_zero |= zero.l[i];
  }

  if (acc_mod != 0) {
    return -1;
  }
  
  /* Convert field element to Montgomery form */
  blst_fp_to(fp, fp);

  /* Return if field element is zero or not */
  if (acc_zero == 0) {
    return 0;
  }

  return 1;
}

/* Encode blst_fp object into byte array */
static void fp_to_bytes(byte* out, const blst_fp* fp) {
  for (size_t i = 0; i < 16; ++i) {
    out[i] = 0;
  }
  blst_bendian_from_fp(out + 16, fp);
}

/* Decode a G1 point from the encoded 128 byte field element array */
static EIP2537_ERROR decode_g1_point(blst_p1_affine* out, const byte* in) {
  /* Extract the x,y field elements from encoding */
  int fp_x_status = fp_from_bytes(&(out->x), in);
  int fp_y_status = fp_from_bytes(&(out->y), in + 64);

  /* Ensure both field elements are valid */
  if ((fp_x_status < 0) || (fp_y_status < 0)) {
    return EIP2537_INVALID_ELEMENT;
  }

  /* If infinity then simply return */
  if ((fp_x_status == 0) && (fp_y_status == 0)) {
    return EIP2537_SUCCESS;
  }

  /* Check if point is on the curve */
  if (!blst_p1_affine_on_curve(out)) {
    return EIP2537_POINT_NOT_ON_CURVE; 
  }

  /* TODO - do we need a subgroup check as well? */

  return EIP2537_SUCCESS;
}

/* Encode a G1 point into byte array from affine point representation */
static void encode_g1_point(byte* out, const blst_p1_affine* in) {
  /* TODO - need to check for infinity? */
  fp_to_bytes(out, &(in->x));
  fp_to_bytes(out + 64, &(in->y));
}






/* Extract encoded field element byte array into blst_fp2 object */
/* TODO - need restrict? */
static int fp2_from_bytes(blst_fp2* restrict fp2, const byte* in) {
  /* Extract the c0 and c1 field elements from encoding */
  int fp_c0_status = fp_from_bytes(&(fp2->fp[0]), in);
  int fp_c1_status = fp_from_bytes(&(fp2->fp[1]), in + 64);

  if ((fp_c0_status < 0) || (fp_c1_status < 0)) {
    return -1;
  }

  return (fp_c0_status | fp_c1_status);
}

/* Encode blst_fp2 object into byte array */
static void fp2_to_bytes(byte* out, const blst_fp2* fp2) {
  for (size_t i = 0; i < 16; ++i) {
    out[i] = 0;
    out[i + 64] = 0;
  }
  blst_bendian_from_fp(out + 16, &(fp2->fp[0]));
  blst_bendian_from_fp(out + 80, &(fp2->fp[1]));
}

/* Decode a G2 point from the encoded 256 byte field element array */
static EIP2537_ERROR decode_g2_point(blst_p2_affine* out, const byte* in) {
  /* Extract the x,y field elements from encoding */
  int fp_x_status = fp2_from_bytes(&(out->x), in);
  int fp_y_status = fp2_from_bytes(&(out->y), in + 128);

  /* Ensure both field elements are valid */
  if ((fp_x_status < 0) || (fp_y_status < 0)) {
    return EIP2537_INVALID_ELEMENT;
  }

  /* If infinity then simply return */
  if ((fp_x_status == 0) && (fp_y_status == 0)) {
    return EIP2537_SUCCESS;
  }

  /* Check if point is on the curve */
  if (!blst_p2_affine_on_curve(out)) {
    return EIP2537_POINT_NOT_ON_CURVE; 
  }

  /* TODO - do we need a subgroup check as well? */

  return EIP2537_SUCCESS;
}

/* Encode a G2 point into byte array from affine point representation */
static void encode_g2_point(byte* out, const blst_p2_affine* in) {
  /* TODO - need to check for infinity? */
  fp2_to_bytes(out, &(in->x));
  fp2_to_bytes(out + 128, &(in->y));
}




/* Decode a 32 byte scalar from the encoded 32 byte array */
static EIP2537_ERROR decode_scalar(blst_scalar* out, const byte* in) {
  blst_scalar_from_bendian(out, in);
  return EIP2537_SUCCESS;
}

/*
  ABI for G1 addition
  
  G1 addition call expects 256 bytes as an input that is interpreted as byte
    concatenation of two G1 points (128 bytes each). Output is an encoding of
    addition operation result - single G1 point (128 bytes).
  
  Error cases:
    Either of points being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length
*/
EIP2537_ERROR bls12_g1add(byte out[128], const byte in[256], size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 256) {
    return EIP2537_INVALID_LENGTH;
  }

  EIP2537_ERROR ret;

  /* Decode inputs */
  blst_p1_affine a_aff;
  ret = decode_g1_point(&a_aff, in);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  blst_p1_affine b_aff;
  ret = decode_g1_point(&b_aff, in + 128);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  /* One of the inputs needs to be projective for point addition function */
  blst_p1 b;
  blst_p1_from_affine(&b, &b_aff);

  /* P = A + B */
  blst_p1 p;
  blst_p1_add_affine(&p, &b, &a_aff);

  /* Convert point to affine */
  blst_p1_affine p_aff;
  blst_p1_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g1_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/* 
  ABI for G1 multiplication

  G1 multiplication call expects 160 bytes as an input that is interpreted as
    byte concatenation of encoding of G1 point (128 bytes) and encoding of a
    scalar value (32 bytes). Output is an encoding of multiplication operation
    result - single G1 point (128 bytes).

  Error cases:
    Point being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length

*/
EIP2537_ERROR bls12_g1mul(byte out[128], const byte in[160], size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 160) {
    return EIP2537_INVALID_LENGTH;
  }

  EIP2537_ERROR ret;

  /* Decode inputs */
  blst_p1_affine a_aff;
  ret = decode_g1_point(&a_aff, in);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  blst_scalar scalar;
  ret = decode_scalar(&scalar, in + 128);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  /* Input needs to be projective for scalar multiplication function */
  blst_p1 a;
  blst_p1_from_affine(&a, &a_aff);

  /* P = A * scalar */
  blst_p1 p;
  blst_p1_mult(&p, &a, &scalar, 256);

  /* Convert point to affine */
  blst_p1_affine p_aff;
  blst_p1_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g1_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for G1 multiexponentiation

  G1 multiexponentiation call expects 160*k bytes as an input that is
    interpreted as byte concatenation of k slices each of them being a byte
    concatenation of encoding of G1 point (128 bytes) and encoding of a scalar
    value (32 bytes). Output is an encoding of multiexponentiation operation
    result - single G1 point (128 bytes).

  Error cases:
    Any of G1 points being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length
    Input is empty
*/
/* TODO - modify from very naive approach, ideally use built-in blst */
EIP2537_ERROR bls12_g1multiexp(byte out[128], byte* in, size_t in_len) {
  /* Check length, is this even necessary? */
  if ((in_len == 0) || ((in_len % 160) != 0)) {
    return EIP2537_INVALID_LENGTH;
  }

  /* Get the number of point/scalar pairs to process */
  size_t k = in_len / 160;

  EIP2537_ERROR ret;
  blst_p1 result = { {0}, {0}, {0} }; /* Infinity */

  for (size_t i = 0; i < k; ++i) {
    /* Decode inputs */
    blst_p1_affine a_aff;
    ret = decode_g1_point(&a_aff, in);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }
  
    blst_scalar scalar;
    ret = decode_scalar(&scalar, in + 128);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }

    in += 160;
  
    /* Input needs to be projective for scalar multiplication function */
    blst_p1 a;
    blst_p1_from_affine(&a, &a_aff);
  
    /* P = A * scalar */
    blst_p1 p;
    blst_p1_mult(&p, &a, &scalar, 256);

    /* result = result + P */
    blst_p1_add_or_double(&result, &result, &p);
  }

  /* Convert result point to affine */
  blst_p1_affine p_aff;
  blst_p1_to_affine(&p_aff, &result);

  /* Encode affine point to EIP format */
  encode_g1_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for G2 addition

  G2 addition call expects 512 bytes as an input that is interpreted as byte
    concatenation of two G2 points (256 bytes each). Output is an encoding of
    addition operation result - single G2 point (256 bytes).

  Error cases:
    Either of points being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length
*/
EIP2537_ERROR bls12_g2add(byte out[256], const byte in[512], size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 512) {
    return EIP2537_INVALID_LENGTH;
  }

  EIP2537_ERROR ret;

  /* Decode inputs */
  blst_p2_affine a_aff;
  ret = decode_g2_point(&a_aff, in);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  blst_p2_affine b_aff;
  ret = decode_g2_point(&b_aff, in + 256);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  /* One of the inputs needs to be projective for point addition function */
  blst_p2 b;
  blst_p2_from_affine(&b, &b_aff);

  /* P = A + B */
  blst_p2 p;
  blst_p2_add_affine(&p, &b, &a_aff);

  /* Convert point to affine */
  blst_p2_affine p_aff;
  blst_p2_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g2_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for G2 multiplication

  G2 multiplication call expects 288 bytes as an input that is interpreted as
    byte concatenation of encoding of G2 point (256 bytes) and encoding of a
    scalar value (32 bytes). Output is an encoding of multiplication operation
    result - single G2 point (256 bytes).

  Error cases:
    Point being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length
*/

EIP2537_ERROR bls12_g2mul(byte out[256], const byte in[288], size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 288) {
    return EIP2537_INVALID_LENGTH;
  }

  EIP2537_ERROR ret;

  /* Decode inputs */
  blst_p2_affine a_aff;
  ret = decode_g2_point(&a_aff, in);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  blst_scalar scalar;
  ret = decode_scalar(&scalar, in + 256);
  if (ret != EIP2537_SUCCESS) {
    return ret;
  }

  /* Input needs to be projective for scalar multiplication function */
  blst_p2 a;
  blst_p2_from_affine(&a, &a_aff);

  /* P = A * scalar */
  blst_p2 p;
  blst_p2_mult(&p, &a, &scalar, 256);

  /* Convert point to affine */
  blst_p2_affine p_aff;
  blst_p2_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g2_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for G2 multiexponentiation

  G2 multiexponentiation call expects 288*k bytes as an input that is
    interpreted as byte concatenation of k slices each of them being a byte
    concatenation of encoding of G2 point (256 bytes) and encoding of a scalar
    value (32 bytes). Output is an encoding of multiexponentiation operation
    result - single G2 point (256 bytes).

  Error cases:
    Any of G2 points being not on the curve must result in error
    Field elements encoding rules apply (obviously)
    Input has invalid length
    Input is empty
*/
/* TODO - modify from very naive approach, ideally use built-in blst */
EIP2537_ERROR bls12_g2multiexp(byte out[256], byte* in, size_t in_len) {
  /* Check length, is this even necessary? */
  if ((in_len == 0) || ((in_len % 288) != 0)) {
    return EIP2537_INVALID_LENGTH;
  }

  /* Get the number of point/scalar pairs to process */
  size_t k = in_len / 288;

  EIP2537_ERROR ret;
  blst_p2 result = { {0, 0}, {0, 0}, {0, 0} }; /* Infinity */

  for (size_t i = 0; i < k; ++i) {
    /* Decode inputs */
    blst_p2_affine a_aff;
    ret = decode_g2_point(&a_aff, in);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }
  
    blst_scalar scalar;
    ret = decode_scalar(&scalar, in + 256);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }

    in += 288;
  
    /* Input needs to be projective for scalar multiplication function */
    blst_p2 a;
    blst_p2_from_affine(&a, &a_aff);
  
    /* P = A * scalar */
    blst_p2 p;
    blst_p2_mult(&p, &a, &scalar, 256);

    /* result = result + P */
    blst_p2_add_or_double(&result, &result, &p);
  }

  /* Convert result point to affine */
  blst_p2_affine p_aff;
  blst_p2_to_affine(&p_aff, &result);

  /* Encode affine point to EIP format */
  encode_g2_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for pairing

  Pairing call expects 384*k bytes as an inputs that is interpreted as byte
    concatenation of k slices. Each slice has the following structure:
      128 bytes of G1 point encoding
      256 bytes of G2 point encoding

  Output is a 32 bytes where first 31 bytes are equal to 0x00 and the last
    byte is 0x01 if pairing result is equal to multiplicative identity in a
    pairing target field and 0x00 otherwise.

  Error cases:
    Any of G1 or G2 points being not on the curve must result in error
    Any of G1 or G2 points are not in the correct subgroup
    Field elements encoding rules apply (obviously)
    Input has invalid length
    Input is empty
*/
/* TODO - parallelize */
EIP2537_ERROR bls12_pairing(byte out[32], byte* in, size_t in_len) {
  /* Check length, is this even necessary? */
  if ((in_len == 0) || ((in_len % 384) != 0)) {
    return EIP2537_INVALID_LENGTH;
  }

  /* Get the number of point pairs to process */
  size_t k = in_len / 384;

  EIP2537_ERROR ret;

  blst_fp12 result;

  for (size_t i = 0; i < k; ++i) {
    /* Decode inputs */
    blst_p1_affine p1_aff;
    ret = decode_g1_point(&p1_aff, in);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }

    if(!blst_p1_affine_in_g1(&p1_aff)) {
      return EIP2537_POINT_NOT_IN_SUBGROUP;
    }

    blst_p2_affine p2_aff;
    ret = decode_g2_point(&p2_aff, in + 128);
    if (ret != EIP2537_SUCCESS) {
      return ret;
    }

    if(!blst_p2_affine_in_g2(&p2_aff)) {
      return EIP2537_POINT_NOT_IN_SUBGROUP;
    }
  
    in += 384;

    if (i > 0) {
      blst_fp12 cur_ml;
      /* TODO - may not exist in SWIG instances */
      blst_miller_loop(&cur_ml, &p2_aff, &p1_aff);
      blst_fp12_mul(&result, &result, &cur_ml);
    }
    else {
      /* TODO - may not exist in SWIG instances */
      blst_miller_loop(&result, &p2_aff, &p1_aff);
    }
  }

  /* TODO - may not exist in SWIG instances */
  blst_final_exp(&result, &result);

  for (size_t i = 0; i < 32; ++i) {
    out[i] = 0;
  }

  if (blst_fp12_is_one(&result)) {
    out[31] = 1;
  }

  return EIP2537_SUCCESS;
}

/*
  ABI for mapping Fp element to G1 point

  Field-to-curve call expects 64 bytes an an input that is interpreted as a an
    element of the base field. Output of this call is 128 bytes and is G1 point
    following respective encoding rules.

  Error cases:
    Input has invalid length
    Input is not a valid field element
*/
EIP2537_ERROR bls12_map_fp_to_g1(byte out[128], const byte in[64],
                                 size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 64) {
    return EIP2537_INVALID_LENGTH;
  }

  /* Extract the field element from encoding */
  blst_fp fp;
  int fp_status = fp_from_bytes(&fp, in);

  /* Ensure field element is valid */
  if (fp_status <= 0) {
    return EIP2537_INVALID_ELEMENT;
  }

  /* Map To G1 */
  blst_p1 p;
  /* TODO - may not exist in SWIG instances */
  blst_map_to_g1(&p, &fp, NULL);

  /* Convert point to affine */
  blst_p1_affine p_aff;
  blst_p1_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g1_point(out, &p_aff);

  return EIP2537_SUCCESS;
}

/*
  ABI for mapping Fp2 element to G2 point

  Field-to-curve call expects 128 bytes an an input that is interpreted as a an
    element of the quadratic extension field. Output of this call is 256 bytes
    and is G2 point following respective encoding rules.

  Error cases:
    Input has invalid length
    Input is not a valid field element
*/
EIP2537_ERROR bls12_map_fp2_to_g2(byte out[256], const byte in[128],
                                  size_t in_len) {
  /* Check length, is this even necessary? */
  if (in_len != 128) {
    return EIP2537_INVALID_LENGTH;
  }

  /* Extract the field element from encoding */
  blst_fp2 fp2;
  int fp2_status = fp2_from_bytes(&fp2, in);

  /* Ensure field element is valid */
  if (fp2_status <= 0) {
    return EIP2537_INVALID_ELEMENT;
  }

  /* Map To G2 */
  blst_p2 p;
  /* TODO - may not exist in SWIG instances */
  blst_map_to_g2(&p, &fp2, NULL);

  /* Convert point to affine */
  blst_p2_affine p_aff;
  blst_p2_to_affine(&p_aff, &p);

  /* Encode affine point to EIP format */
  encode_g2_point(out, &p_aff);

  return EIP2537_SUCCESS;
}


/* Gas Costs */
const uint64_t BLS12_G1ADD_GAS         = 600;
const uint64_t BLS12_G1MUL_GAS         = 12000;
const uint64_t BLS12_G2ADD_GAS         = 4500;
const uint64_t BLS12_G2MUL_GAS         = 55000;
const uint64_t BLS12_PAIRING_BASE_GAS  = 115000;
const uint64_t BLS12_PAIRING_PAIR_GAS  = 23000;
const uint64_t BLS12_MAP_FP_TO_G1_GAS  = 5500;
const uint64_t BLS12_MAP_FP2_TO_G2_GAS = 110000;

const uint64_t BLS12_MULTIEXP_MULTIPLIER_GAS     = 1000;
const uint64_t BLS12_MULTIEXP_DISCOUNT_TABLE_LEN = 128;
const uint64_t BLS12_MULTIEXP_DISCOUNT[128] = {
  1200, 888, 764, 641, 594, 547, 500, 453,
   438, 423, 408, 394, 379, 364, 349, 334,
   330, 326, 322, 318, 314, 310, 306, 302,
   298, 294, 289, 285, 281, 277, 273, 269,
   268, 266, 265, 263, 262, 260, 259, 257,
   256, 254, 253, 251, 250, 248, 247, 245,
   244, 242, 241, 239, 238, 236, 235, 233,
   232, 231, 229, 228, 226, 225, 223, 222,
   221, 220, 219, 219, 218, 217, 216, 216,
   215, 214, 213, 213, 212, 211, 211, 210,
   209, 208, 208, 207, 206, 205, 205, 204,
   203, 202, 202, 201, 200, 199, 199, 198,
   197, 196, 196, 195, 194, 193, 193, 192,
   191, 191, 190, 189, 188, 188, 187, 186,
   185, 185, 184, 183, 182, 182, 181, 180,
   179, 179, 178, 177, 176, 176, 175, 174
};

uint64_t bls12_g1add_gas() {
  return BLS12_G1ADD_GAS;
};

uint64_t bls12_g1mul_gas() {
  return BLS12_G1MUL_GAS;
};

uint64_t bls12_g1multiexp_gas(uint64_t input_len) {
  uint64_t k = (uint64_t) floor(input_len / 160);

  if (k == 0) {
    return 0;
  }

  uint64_t discount; 

  if (k < BLS12_MULTIEXP_DISCOUNT_TABLE_LEN) {
    discount = BLS12_MULTIEXP_DISCOUNT[k - 1];
  }
  else {
    discount = BLS12_MULTIEXP_DISCOUNT[BLS12_MULTIEXP_DISCOUNT_TABLE_LEN - 1];
  }
  
  return (uint64_t)((k * BLS12_G1MUL_GAS * discount) /
                    BLS12_MULTIEXP_MULTIPLIER_GAS);
};

uint64_t bls12_g2add_gas() {
  return BLS12_G2ADD_GAS;
};

uint64_t bls12_g2mul_gas() {
  return BLS12_G2MUL_GAS;
};

uint64_t bls12_g2multiexp_gas(uint64_t input_len) {
  uint64_t k = (uint64_t) floor(input_len / 288);

  if (k == 0) {
    return 0;
  }

  uint64_t discount; 

  if (k < BLS12_MULTIEXP_DISCOUNT_TABLE_LEN) {
    discount = BLS12_MULTIEXP_DISCOUNT[k - 1];
  }
  else {
    discount = BLS12_MULTIEXP_DISCOUNT[BLS12_MULTIEXP_DISCOUNT_TABLE_LEN - 1];
  }
  
  return (uint64_t)((k * BLS12_G2MUL_GAS * discount) /
                    BLS12_MULTIEXP_MULTIPLIER_GAS);
};

uint64_t bls12_pairing_gas(uint64_t input_len) {
  uint64_t k = (uint64_t) floor(input_len / 384);

  if (k == 0) {
    return 0;
  }

  return (k * BLS12_PAIRING_PAIR_GAS) + BLS12_PAIRING_BASE_GAS;
};

uint64_t bls12_map_fp_to_g1_gas() {
  return BLS12_MAP_FP_TO_G1_GAS;
};

uint64_t bls12_map_fp2_to_g2_gas() {
  return BLS12_MAP_FP2_TO_G2_GAS;
};

