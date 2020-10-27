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

  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG1Add.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG1Mul.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG1MultiExp.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG2Add.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG2Mul.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsG2MultiExp.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsMapG1.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsMapG2.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/blsPairing.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG1Add.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG1Mul.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG1MultiExp.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG2Add.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG2Mul.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsG2MultiExp.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsMapG1.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsMapG2.json
  wget https://raw.githubusercontent.com/ethereum/go-ethereum/master/core/vm/testdata/precompiles/fail-blsPairing.json

  cd ..
fi

gcc -Wall -Iblst/bindings src/eip2537.c src/test.c blst/libblst.a -o test_eip2537

./test_eip2537

