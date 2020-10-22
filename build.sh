#!/bin/bash

if [ ! -d blst ]; then
  git clone https://github.com/supranational/blst
fi

if [ ! -f blst/libblst.a ]; then
  cd blst
  ./build.sh
  cd ..
fi

if [ ! -d test_vectors ]; then
  mkdir test_vectors
  cd test_vectors
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/fp2_to_g2.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/fp_to_g1.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g1_add.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g1_mul.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g1_multiexp.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g1_not_on_curve.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g2_add.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g2_mul.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g2_multiexp.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/g2_not_on_curve.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/invalid_fp2_encoding.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/invalid_fp_encoding.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/invalid_subgroup_for_pairing.csv
  wget https://raw.githubusercontent.com/MariusVanDerWijden/go-ethereum/bls_fuzzer/tests/fuzzers/bls12381/csv/pairing.csv
  cd ..
fi

gcc -Iblst/bindings src/eip2537.c src/test.c blst/libblst.a -o test_eip2537

./test_eip2537

