// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package blst_eip2537

// #cgo CFLAGS: -I${SRCDIR}/../src -I${SRCDIR}/../blst/bindings -I${SRCDIR}/../blst/build -I${SRCDIR}/../blst/src -D__BLST_CGO__
// #cgo amd64 CFLAGS: -D__ADX__ -mno-avx
// #include "eip2537.h"
import "C"
import (
	"errors"
)

type Bls12Func func([]byte) ([]byte, error)

func decodeEip2537Error(err C.EIP2537_ERROR) string {
	var err_str string

	switch err {
	case C.EIP2537_SUCCESS:
		err_str = "Success"
	case C.EIP2537_POINT_NOT_ON_CURVE:
		err_str = "point not on curve"
	case C.EIP2537_POINT_NOT_IN_SUBGROUP:
		err_str = "point not in subgroup"
	case C.EIP2537_INVALID_ELEMENT:
		err_str = "invalid element"
	case C.EIP2537_ENCODING_ERROR:
		err_str = "encoding error"
	case C.EIP2537_INVALID_LENGTH:
		err_str = "invalid length"
	case C.EIP2537_EMPTY_INPUT:
		err_str = "empty input"
	case C.EIP2537_MEMORY_ERROR:
		err_str = "memory allocation error"
	default:
		err_str = "unknown error condition"
	}

	return err_str
}

func G1Add(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_g1add((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G1Mul(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_g1mul((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G1Multiexp(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_g1multiexp((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G1MultiexpNaive(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_g1multiexp_naive((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G1MultiexpBosCoster(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_g1multiexp_bc((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G2Add(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_g2add((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G2Mul(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_g2mul((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G2Multiexp(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_g2multiexp((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G2MultiexpNaive(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_g2multiexp_naive((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func G2MultiexpBosCoster(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_g2multiexp_bc((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func Pairing(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 32)
	err := C.bls12_pairing((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func MapFpToG1(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 128)
	err := C.bls12_map_fp_to_g1((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}

func MapFp2ToG2(input []byte) ([]byte, error) {
	if len(input) == 0 {
		return nil, errors.New(decodeEip2537Error(C.EIP2537_INVALID_LENGTH))
	}
	output := make([]byte, 256)
	err := C.bls12_map_fp2_to_g2((*C.byte)(&output[0]), (*C.byte)(&input[0]),
		C.size_t(len(input)))
	if err != C.EIP2537_SUCCESS {
		return nil, errors.New(decodeEip2537Error(err))
	}
	return output, nil
}
