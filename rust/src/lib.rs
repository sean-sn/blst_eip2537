// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#![allow(non_camel_case_types)]

pub type byte = u8;
pub type EIP2537_ERROR = u32;
const EIP2537_SUCCESS: EIP2537_ERROR = 0;
const EIP2537_POINT_NOT_ON_CURVE: EIP2537_ERROR = 1;
const EIP2537_POINT_NOT_IN_SUBGROUP: EIP2537_ERROR = 2;
const EIP2537_INVALID_ELEMENT: EIP2537_ERROR = 3;
const EIP2537_ENCODING_ERROR: EIP2537_ERROR = 4;
const EIP2537_INVALID_LENGTH: EIP2537_ERROR = 5;
const EIP2537_EMPTY_INPUT: EIP2537_ERROR = 6;
const EIP2537_MEMORY_ERROR: EIP2537_ERROR = 7;

extern "C" {
    pub fn bls12_g1add(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g1mul(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g1multiexp(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g1multiexp_naive(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g1multiexp_bc(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g2add(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g2mul(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g2multiexp(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g2multiexp_naive(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_g2multiexp_bc(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_pairing(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_map_fp_to_g1(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_map_fp2_to_g2(
        out: *mut byte,
        input: *const byte,
        in_len: usize,
    ) -> EIP2537_ERROR;
}

pub struct blstEIP2537Executor;

impl blstEIP2537Executor {
    fn decode_eip2537_error(err: EIP2537_ERROR) -> &'static str {
        match err {
            EIP2537_SUCCESS => "Success",
            EIP2537_POINT_NOT_ON_CURVE => "point not on curve",
            EIP2537_POINT_NOT_IN_SUBGROUP => "point not in subgroup",
            EIP2537_INVALID_ELEMENT => "invalid element",
            EIP2537_ENCODING_ERROR => "encoding error",
            EIP2537_INVALID_LENGTH => "invalid length",
            EIP2537_EMPTY_INPUT => "empty input",
            EIP2537_MEMORY_ERROR => "memory allocation error",
            _ => "unknown error condition",
        }
    }

    pub fn g1_add<'a>(input: &'a [u8]) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_g1add(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g1_mul<'a>(input: &'a [u8]) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_g1mul(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g1_multiexp<'a>(input: &'a [u8]) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_g1multiexp(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g1_multiexp_naive<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_g1multiexp_naive(
                output.as_mut_ptr(),
                input.as_ptr(),
                input.len(),
            )
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g1_multiexp_bc<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_g1multiexp_bc(
                output.as_mut_ptr(),
                input.as_ptr(),
                input.len(),
            )
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g2_add<'a>(input: &'a [u8]) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_g2add(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g2_mul<'a>(input: &'a [u8]) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_g2mul(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g2_multiexp<'a>(input: &'a [u8]) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_g2multiexp(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g2_multiexp_naive<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_g2multiexp_naive(
                output.as_mut_ptr(),
                input.as_ptr(),
                input.len(),
            )
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn g2_multiexp_bc<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_g2multiexp_bc(
                output.as_mut_ptr(),
                input.as_ptr(),
                input.len(),
            )
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn pairing<'a>(input: &'a [u8]) -> Result<[u8; 32], &'static str> {
        let mut output = [0u8; 32];

        let err = unsafe {
            bls12_pairing(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn map_fp_to_g1<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 128], &'static str> {
        let mut output = [0u8; 128];

        let err = unsafe {
            bls12_map_fp_to_g1(output.as_mut_ptr(), input.as_ptr(), input.len())
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }

    pub fn map_fp2_to_g2<'a>(
        input: &'a [u8],
    ) -> Result<[u8; 256], &'static str> {
        let mut output = [0u8; 256];

        let err = unsafe {
            bls12_map_fp2_to_g2(
                output.as_mut_ptr(),
                input.as_ptr(),
                input.len(),
            )
        };

        if err != EIP2537_SUCCESS {
            return Err(blstEIP2537Executor::decode_eip2537_error(err));
        }

        Ok(output)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    // Test framework taken from Alex Vlasov with gratitude
    // https://github.com/matter-labs/eip1962/blob/master/src/public_interface/eip2537/mod.rs

    fn run_on_test_inputs<F: Fn(&[u8]) -> Result<Vec<u8>, &'static str>>(
        file_path: &str,
        expect_success: bool,
        test_function: F,
    ) -> bool {
        let mut reader = csv::Reader::from_path(file_path).unwrap();
        for r in reader.records() {
            let r = r.unwrap();
            let input_str = r.get(0).unwrap();
            let input = hex::decode(input_str).unwrap();
            let mut expected_output = vec![];
            if let Some(s) = r.get(1) {
                if expect_success == true {
                    expected_output = hex::decode(s).unwrap()
                }
            }

            let value = test_function(&input);
            //println!("Actual: {:x?}", value);
            match value {
                Ok(result) => {
                    if expected_output != result {
                        println!(
                            "MISMATCH: expected {:x?} received {:x?}",
                            expected_output, result
                        );
                        return false;
                    }
                }
                Err(..) => {
                    if expect_success == true {
                        println!("ERROR: {:?}", value);
                        return false;
                    }
                }
            }
        }

        true
    }

    #[test]
    fn test_g1add() {
        let p = "../test_vectors/g1_add.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_add(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g1mul() {
        let p = "../test_vectors/g1_mul.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_mul(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g1_not_on_curve() {
        let p = "../test_vectors/g1_not_on_curve.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_mul(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, false, f);
        assert!(success);
    }

    #[test]
    fn test_g1multiexp() {
        let p = "../test_vectors/g1_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_multiexp(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g1multiexp_naive() {
        let p = "../test_vectors/g1_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_multiexp_naive(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g1multiexp_bc() {
        let p = "../test_vectors/g1_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g1_multiexp_bc(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g2add() {
        let p = "../test_vectors/g2_add.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_add(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g2mul() {
        let p = "../test_vectors/g2_mul.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_mul(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g2_not_on_curve() {
        let p = "../test_vectors/g2_not_on_curve.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_mul(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, false, f);
        assert!(success);
    }

    #[test]
    fn test_g2multiexp() {
        let p = "../test_vectors/g2_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_multiexp(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g2multiexp_naive() {
        let p = "../test_vectors/g2_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_multiexp_naive(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_g2multiexp_bc() {
        let p = "../test_vectors/g2_multiexp.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::g2_multiexp_bc(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_pairing() {
        let p = "../test_vectors/pairing.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::pairing(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_invalid_subgroup_pairing() {
        let p = "../test_vectors/invalid_subgroup_for_pairing.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::pairing(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, false, f);
        assert!(success);
    }

    #[test]
    fn test_map_fp_to_g1() {
        let p = "../test_vectors/fp_to_g1.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::map_fp_to_g1(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_invalid_fp_encoding() {
        let p = "../test_vectors/invalid_fp_encoding.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::map_fp_to_g1(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, false, f);
        assert!(success);
    }

    #[test]
    fn test_map_fp2_to_g2() {
        let p = "../test_vectors/fp2_to_g2.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::map_fp2_to_g2(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, true, f);
        assert!(success);
    }

    #[test]
    fn test_invalid_fp2_encoding() {
        let p = "../test_vectors/invalid_fp2_encoding.csv";
        let f = |input: &[u8]| {
            blstEIP2537Executor::map_fp2_to_g2(input).map(|r| r.to_vec())
        };
        let success = run_on_test_inputs(p, false, f);
        assert!(success);
    }
}
