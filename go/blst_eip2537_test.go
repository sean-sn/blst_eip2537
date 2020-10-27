// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package blst_eip2537

import (
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"testing"
	"time"
)

// Test framework taken from go-ethereum with gratitude
// https://github.com/ethereum/go-ethereum/blob/master/core/vm/contracts_test.go
type precompiledTest struct {
	Input, Expected string
	Gas             uint64
	Name            string
	NoBenchmark     bool
}

type precompiledFailureTest struct {
	Input         string
	ExpectedError string
	Name          string
}

func testJson(file_path string, expect_success bool,
	test_function Bls12Func, t *testing.T) {

	test_json, err := ioutil.ReadFile(file_path)
	if err != nil {
		t.Fatal(err)
	}

	if expect_success == true {
		var tests []precompiledTest
		err = json.Unmarshal(test_json, &tests)
		if err != nil {
			t.Fatal(err)
		}

		for _, test := range tests {
			t.Run(test.Name, func(t *testing.T) {
				input, err := hex.DecodeString(test.Input)
				if err != nil {
					t.Fatal(err)
				}

				output, test_err := test_function(input)

				if test_err != nil {
					t.Errorf("Test received unexpected error %v", test_err)
				} else {
					out_str := hex.EncodeToString(output)
					if out_str != test.Expected {
						t.Errorf("Expected %v, got %v", test.Expected, out_str)
					}
				}
			})
		}
	} else {
		var tests []precompiledFailureTest
		err = json.Unmarshal(test_json, &tests)
		if err != nil {
			t.Fatal(err)
		}

		for _, test := range tests {
			t.Run(test.Name, func(t *testing.T) {
				input, err := hex.DecodeString(test.Input)
				if err != nil {
					t.Fatal(err)
				}

				_, test_err := test_function(input)

				if test_err == nil {
					t.Errorf("Test should have failed with error %v",
						test.ExpectedError)
				}
			})
		}
	}
}

func benchJson(file_path string, test_function Bls12Func, bench *testing.B) {
	test_json, err := ioutil.ReadFile(file_path)
	if err != nil {
		bench.Fatal(err)
	}

	var tests []precompiledTest
	err = json.Unmarshal(test_json, &tests)
	if err != nil {
		bench.Fatal(err)
	}

	for _, test := range tests {
		if test.NoBenchmark == false {
			input, err := hex.DecodeString(test.Input)
			if err != nil {
				bench.Fatal(err)
			}

			var output []byte
			var data = make([]byte, len(input))

			bench.Run(fmt.Sprintf("%s-Gas=%d", test.Name, test.Gas),
				func(bench *testing.B) {
					bench.ReportAllocs()
					start := time.Now()
					bench.ResetTimer()
					for i := 0; i < bench.N; i++ {
						copy(data, input)
						output, err = test_function(data)
					}
					bench.StopTimer()
					elapsed := uint64(time.Since(start))
					if elapsed < 1 {
						elapsed = 1
					}
					gasUsed := test.Gas * uint64(bench.N)
					bench.ReportMetric(float64(test.Gas), "gas/op")
					// Keep it as uint64, mul 100 to get 2 digit float later
					mgasps := (100 * 1000 * gasUsed) / elapsed
					bench.ReportMetric(float64(mgasps)/100, "mgas/s")
					//Check if it is correct
					if err != nil {
						bench.Error(err)
						return
					}
					out_str := hex.EncodeToString(output)
					if out_str != test.Expected {
						bench.Error(fmt.Sprintf("Expected %v, got %v",
							test.Expected, out_str))
						return
					}
				})
		}
	}
}

// Tests
func TestG1Add(t *testing.T) {
	testJson("../test_vectors/blsG1Add.json", true, G1Add, t)
}

func TestG1Mul(t *testing.T) {
	testJson("../test_vectors/blsG1Mul.json", true, G1Mul, t)
}

func TestG1Multiexp(t *testing.T) {
	testJson("../test_vectors/blsG1MultiExp.json", true, G1Multiexp, t)
}

func TestG2Add(t *testing.T) {
	testJson("../test_vectors/blsG2Add.json", true, G2Add, t)
}

func TestG2Mul(t *testing.T) {
	testJson("../test_vectors/blsG2Mul.json", true, G2Mul, t)
}

func TestG2Multiexp(t *testing.T) {
	testJson("../test_vectors/blsG2MultiExp.json", true, G2Multiexp, t)
}

func TestPairing(t *testing.T) {
	testJson("../test_vectors/blsPairing.json", true, Pairing, t)
}

func TestMapFpToG1(t *testing.T) {
	testJson("../test_vectors/blsMapG1.json", true, MapFpToG1, t)
}

func TestMapFp2ToG2(t *testing.T) {
	testJson("../test_vectors/blsMapG2.json", true, MapFp2ToG2, t)
}

func TestG1AddFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG1Add.json", false, G1Add, t)
}

func TestG1MulFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG1Mul.json", false, G1Mul, t)
}

func TestG1MultiexpFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG1MultiExp.json", false, G1Multiexp, t)
}

func TestG2AddFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG2Add.json", false, G2Add, t)
}

func TestG2MulFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG2Mul.json", false, G2Mul, t)
}

func TestG2MultiexpFail(t *testing.T) {
	testJson("../test_vectors/fail-blsG2MultiExp.json", false, G2Multiexp, t)
}

func TestPairingFail(t *testing.T) {
	testJson("../test_vectors/fail-blsPairing.json", false, Pairing, t)
}

func TestMapFpToG1Fail(t *testing.T) {
	testJson("../test_vectors/fail-blsMapG1.json", false, MapFpToG1, t)
}

func TestMapFp2ToG2Fail(t *testing.T) {
	testJson("../test_vectors/fail-blsMapG2.json", false, MapFp2ToG2, t)
}

// Benchmarks
func BenchmarkG1Add(b *testing.B) {
	benchJson("../test_vectors/blsG1Add.json", G1Add, b)
}

func BenchmarkG1Mul(b *testing.B) {
	benchJson("../test_vectors/blsG1Mul.json", G1Mul, b)
}

func BenchmarkG1Multiexp(b *testing.B) {
	benchJson("../test_vectors/blsG1MultiExp.json", G1Multiexp, b)
}

func BenchmarkG2Add(b *testing.B) {
	benchJson("../test_vectors/blsG2Add.json", G2Add, b)
}

func BenchmarkG2Mul(b *testing.B) {
	benchJson("../test_vectors/blsG2Mul.json", G2Mul, b)
}

func BenchmarkG2Multiexp(b *testing.B) {
	benchJson("../test_vectors/blsG2MultiExp.json", G2Multiexp, b)
}

func BenchmarkPairing(b *testing.B) {
	benchJson("../test_vectors/blsPairing.json", Pairing, b)
}

func BenchmarkMapFpToG1(b *testing.B) {
	benchJson("../test_vectors/blsMapG1.json", MapFpToG1, b)
}

func BenchmarkMapFp2ToG2(b *testing.B) {
	benchJson("../test_vectors/blsMapG2.json", MapFp2ToG2, b)
}
