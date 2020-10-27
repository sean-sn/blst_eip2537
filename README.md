# blst_eip2537

C implementation of EIP-2537 BLS12-381 precompiles using blst

Provides Rust and Go bindings for C library

## Status
**Warning - this library is a work in progress**

All [EIP2537](https://eips.ethereum.org/EIPS/eip-2537) functions implemented using blst primitives

Built-in C tests passing based on test vectors come from https://github.com/MariusVanDerWijden/go-ethereum/tree/bls_fuzzer/tests/fuzzers/bls12381/csv

Performance work has not started.  A few places to note:
  * Parallelization of PAIRING
  * MULTIEXP algorithm (brutally naive on top of blst scalar mul in current state)
  * Parallelization of decodes

Rust crate is not published

## C 

### Build
./build.sh

### Re-run test
./test_eip2537

## Rust

Crate is named `blst_eip2537`

A test and benchmark is provided for each function.  Benchmarks for operations involving pairs (multiexp and pairing) are sized in powers of two up to maximum with block gas limits in mind.

## Go

Package is named `blst_eip2537`

A test and benchmark is provided for each function.  Both tests and benchmarks are taken directly from [go-ethereuem](https://github.com/ethereum/go-ethereum/blob/master/core/vm/contracts_test.go)
