/*
 * Copyright Supranational LLC
 * Licensed under the Apache License, Version 2.0, see LICENSE for details.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "blst.h"
#include "eip2537.h"

/* Utility Functions */

/* Convert ASCII string to array of bytes */
static void string_to_bytes(byte* out, const char* in, size_t num) {
  char cur_byte[3] = "";
  while (num--) {
    cur_byte[0] = *in++;
    cur_byte[1] = *in++;
    *out++      = strtol(cur_byte, NULL, 16);
  }
}

/* Print array of bytes */
/*
static void print_bytes(const byte* in, size_t num) {
  for (size_t i = 0; i < num; ++i) {
    printf("%02x", in[i]);
  }
  printf("\n");
}
*/

static int bytes_are_equal(const byte* a, const byte* b, size_t num) {
  const limb_t *ap = (const limb_t *)a;
  const limb_t *bp = (const limb_t *)b;
  limb_t acc;
  size_t i;

  num /= sizeof(limb_t);

  for (acc = 0, i = 0; i < num; i++) {
    acc |= ap[i] ^ bp[i];
  }

  return (~acc & (acc - 1)) >> ((sizeof(limb_t) * 8)- 1);
}

/* Test Functions */
int test_g1_add() {
  FILE* f = fopen("test_vectors/g1_add.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[771];
  byte in[256];
  byte out[128];
  byte act_out[128];
  EIP2537_ERROR err;

  fgets(row, 771, f); /* Skip first row */
  while (fgets(row, 771, f)) {
    if (row[512] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 256);
    string_to_bytes(out, row+513, 128);

    err = bls12_g1add(act_out, in, 256);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 128)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  256);
  //printf("Expected output\n");
  //print_bytes(out, 128);
  //printf("Actual output\n");
  //print_bytes(act_out, 128);

  fclose(f);

  return 0;
}

int test_g1_mul() {
  FILE* f = fopen("test_vectors/g1_mul.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[579];
  byte in[160];
  byte out[128];
  byte act_out[128];
  EIP2537_ERROR err;

  fgets(row, 579, f); /* Skip first row */
  while (fgets(row, 579, f)) {
    if (row[320] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 160);
    string_to_bytes(out, row+321, 128);

    err = bls12_g1mul(act_out, in, 160);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 128)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  160);
  //printf("Expected output\n");
  //print_bytes(out, 128);
  //printf("Actual output\n");
  //print_bytes(act_out, 128);

  fclose(f);

  /* Test failure cases */
  f = fopen("test_vectors/g1_not_on_curve.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  fgets(row, 579, f); /* Skip first row */
  while (fgets(row, 579, f)) {
    if (row[320] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 160);

    err = bls12_g1mul(act_out, in, 160);
    if (err != EIP2537_POINT_NOT_ON_CURVE) {
      printf("ERROR - should be EIP2537_POINT_NOT_ON_CURVE - %d\n", err);
      return -1;
    }
  } 

  fclose(f);

  return 0;
}

int test_g1_multi_exp() {
  FILE* f = fopen("test_vectors/g1_multiexp.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  size_t row_size = 0;

  byte* in;
  byte out[128];
  byte act_out[128];
  EIP2537_ERROR err;

  ssize_t in_len = 0;

  char output_str[258];
  fgets(output_str, 258, f); /* Skip first row */

  while (1) {
    char*  row = NULL;
    in_len = getdelim(&row, &row_size, 44, f); /* Read variable input values */
    if (in_len == -1) {
      break;
    }
    if (row[(in_len - 1)] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }
    in = malloc(in_len - 1);
    string_to_bytes(in, row, (in_len - 1));
    free(row);
    fgets(output_str, 258, f); /* Get output value*/
    string_to_bytes(out, output_str, 128);

    err = bls12_g1multiexp(act_out, in, (in_len >> 1));
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 128)) {
      printf("ERROR not equal\n");
      return -1;
    }

    free(in);
  }

  //printf("Expected output\n");
  //print_bytes(out, 128);
  //printf("Actual output\n");
  //print_bytes(act_out, 128);

  fclose(f);

  return 0;
}

int test_g2_add() {
  FILE* f = fopen("test_vectors/g2_add.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[1539];
  byte in[512];
  byte out[256];
  byte act_out[256];
  EIP2537_ERROR err;

  fgets(row, 1539, f); /* Skip first row */
  while (fgets(row, 1539, f)) {
    if (row[1024] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 512);
    string_to_bytes(out, row+1025, 256);

    err = bls12_g2add(act_out, in, 512);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 256)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  512);
  //printf("Expected output\n");
  //print_bytes(out, 256);
  //printf("Actual output\n");
  //print_bytes(act_out, 256);

  fclose(f);

  return 0;
}

int test_g2_mul() {
  FILE* f = fopen("test_vectors/g2_mul.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[1091];
  byte in[288];
  byte out[256];
  byte act_out[256];
  EIP2537_ERROR err;

  fgets(row, 1091, f); /* Skip first row */
  while (fgets(row, 1091, f)) {
    if (row[576] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 288);
    string_to_bytes(out, row+577, 256);

    err = bls12_g2mul(act_out, in, 288);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 256)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  288);
  //printf("Expected output\n");
  //print_bytes(out, 256);
  //printf("Actual output\n");
  //print_bytes(act_out, 256);

  fclose(f);

  /* Test failure cases */
  f = fopen("test_vectors/g2_not_on_curve.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  fgets(row, 1091, f); /* Skip first row */
  while (fgets(row, 1091, f)) {
    if (row[576] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 288);

    err = bls12_g2mul(act_out, in, 288);
    if (err != EIP2537_POINT_NOT_ON_CURVE) {
      printf("ERROR - should be EIP2537_POINT_NOT_ON_CURVE - %d\n", err);
      return -1;
    }
  } 

  fclose(f);

  return 0;
}

int test_g2_multi_exp() {
  FILE* f = fopen("test_vectors/g2_multiexp.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  size_t row_size = 0;

  byte* in;
  byte out[256];
  byte act_out[256];
  EIP2537_ERROR err;

  ssize_t in_len = 0;

  char output_str[514];
  fgets(output_str, 514, f); /* Skip first row */

  while (1) {
    char*  row = NULL;
    in_len = getdelim(&row, &row_size, 44, f); /* Read variable input values */
    if (in_len == -1) {
      break;
    }
    if (row[(in_len - 1)] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }
    in = malloc(in_len - 1);
    string_to_bytes(in, row, (in_len - 1));
    free(row);
    fgets(output_str, 514, f); /* Get output value*/
    string_to_bytes(out, output_str, 256);

    err = bls12_g2multiexp(act_out, in, (in_len >> 1));
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 256)) {
      printf("ERROR not equal\n");
      return -1;
    }

    free(in);
  }

  //printf("Expected output\n");
  //print_bytes(out, 256);
  //printf("Actual output\n");
  //print_bytes(act_out, 256);

  fclose(f);

  return 0;
}

int test_pairing() {
  FILE* f = fopen("test_vectors/pairing.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  size_t row_size = 0;

  byte* in;
  byte out[32];
  byte act_out[32];
  EIP2537_ERROR err;

  ssize_t in_len = 0;

  char output_str[66];
  fgets(output_str, 66, f); /* Skip first row */

  while (1) {
    char*  row = NULL;
    in_len = getdelim(&row, &row_size, 44, f); /* Read variable input values */
    if (in_len == -1) {
      break;
    }
    if (row[(in_len - 1)] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }
    in = malloc(in_len - 1);
    string_to_bytes(in, row, (in_len - 1));
    free(row);
    fgets(output_str, 66, f); /* Get output value*/
    string_to_bytes(out, output_str, 32);

    err = bls12_pairing(act_out, in, (in_len >> 1));
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 32)) {
      printf("ERROR not equal\n");
      return -1;
    }

    free(in);
  }

  //printf("Expected output\n");
  //print_bytes(out, 32);
  //printf("Actual output\n");
  //print_bytes(act_out, 32);

  fclose(f);

  /* Test failure cases */
  f = fopen("test_vectors/invalid_subgroup_for_pairing.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  fgets(output_str, 66, f); /* Skip first row */
  while (1) {
    char*  row = NULL;
    in_len = getdelim(&row, &row_size, 44, f); /* Read variable input values */
    if (in_len == -1) {
      break;
    }
    if (row[(in_len - 1)] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }
    in = malloc(in_len - 1);
    string_to_bytes(in, row, (in_len - 1));

    fgets(row, in_len, f); /* Clear output value*/
    free(row);

    err = bls12_pairing(act_out, in, (in_len >> 1));
    if (err != EIP2537_POINT_NOT_IN_SUBGROUP) {
      printf("ERROR - should be EIP2537_POINT_NOT_IN_SUBGROUP - %d\n", err);
      return -1;
    }

    free(in);
  }

  fclose(f);

  return 0;
}

int test_map_fp_to_g1() {
  FILE* f = fopen("test_vectors/fp_to_g1.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[387];
  byte in[64];
  byte out[128];
  byte act_out[128];
  EIP2537_ERROR err;

  fgets(row, 387, f); /* Skip first row */
  while (fgets(row, 387, f)) {
    if (row[128] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 64);
    string_to_bytes(out, row+129, 128);

    err = bls12_map_fp_to_g1(act_out, in, 64);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 128)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  64);
  //printf("Expected output\n");
  //print_bytes(out, 128);
  //printf("Actual output\n");
  //print_bytes(act_out, 128);

  fclose(f);

  /* Test failure cases */
  f = fopen("test_vectors/invalid_fp_encoding.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  fgets(row, 387, f); /* Skip first row */
  while (fgets(row, 387, f)) {
    if (row[128] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 64);

    err = bls12_map_fp_to_g1(act_out, in, 64);
    if (err != EIP2537_INVALID_ELEMENT) {
      printf("ERROR - should be EIP2537_INVALID_ELEMENT - %d\n", err);
      return -1;
    }
  } 

  fclose(f);

  return 0;
}

int test_map_fp2_to_g2() {
  FILE* f = fopen("test_vectors/fp2_to_g2.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  char row[771];
  byte in[128];
  byte out[256];
  byte act_out[256];
  EIP2537_ERROR err;

  fgets(row, 771, f); /* Skip first row */
  while (fgets(row, 771, f)) {
    if (row[256] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 128);
    string_to_bytes(out, row+257, 256);

    err = bls12_map_fp2_to_g2(act_out, in, 128);
    if (err != EIP2537_SUCCESS) {
      printf("ERROR %d\n", err);
      return -1;
    }

    if (!bytes_are_equal(out, act_out, 256)) {
      printf("ERROR not equal\n");
      return -1;
    }
  } 

  //printf("Input\n");
  //print_bytes(in,  128);
  //printf("Expected output\n");
  //print_bytes(out, 256);
  //printf("Actual output\n");
  //print_bytes(act_out, 256);

  fclose(f);

  /* Test failure cases */
  f = fopen("test_vectors/invalid_fp2_encoding.csv", "r");
  if (f == NULL) {
    printf("ERROR reading file\n");
    return -1;
  }

  fgets(row, 771, f); /* Skip first row */
  while (fgets(row, 771, f)) {
    if (row[256] != ',') {
      printf("ERROR reading row\n");
      return -1;
    }

    /* Convert ascii to actual value */
    string_to_bytes(in, row, 128);

    err = bls12_map_fp2_to_g2(act_out, in, 128);
    if (err != EIP2537_INVALID_ELEMENT) {
      printf("ERROR - should be EIP2537_INVALID_ELEMENT - %d\n", err);
      return -1;
    }
  } 

  fclose(f);

  return 0;
}

int main() {
  //blst_fp x;
  //printf("size of x %ld\n", sizeof(x));
  //printf("size of x.l %ld\n", sizeof(x.l));
  //printf("size of limb_t %ld\n", sizeof(limb_t));

  printf("Testing all EIP-2537 precompile functions\n");

  int ret = 0;

  /* Test EIP 2537 functions using known answer vectors */
  ret |= test_g1_add();
  ret |= test_g1_mul();
  ret |= test_g1_multi_exp();
  ret |= test_g2_add();
  ret |= test_g2_mul();
  ret |= test_g2_multi_exp();
  ret |= test_pairing();
  ret |= test_map_fp_to_g1();
  ret |= test_map_fp2_to_g2();

  if (ret == 0) {
    printf("\nPASSED\n\n");
  }
  else {
    printf("\nFAILED - see ERROR printouts\n\n");
  }

  return 0;
}
