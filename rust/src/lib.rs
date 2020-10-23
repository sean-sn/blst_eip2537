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
        input: *mut byte,
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
        input: *mut byte,
        in_len: usize,
    ) -> EIP2537_ERROR;

    pub fn bls12_pairing(
        out: *mut byte,
        input: *mut byte,
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
            let expected_output = if let Some(s) = r.get(1) {
                hex::decode(s).unwrap()
            } else {
                vec![]
            };

            let value = test_function(&input);
            //println!("Actual: {:x?}", value);
            match value {
                Ok(result) => {
                    if expected_output != result {
                        return false;
                    }
                }
                Err(..) => {
                    println!("ERROR: {:?}", value);
                    if expect_success == true {
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

    // TODO - need to fix run_on_test_inputs to handle expected err msg
    /*
        #[test]
        fn test_g1_not_on_curve() {
            let p = "../test_vectors/g1_not_on_curve.csv";
            let f = |input: &[u8]| {
                blstEIP2537Executor::g1_mul(input).map(|r| r.to_vec())
            };
            let success = run_on_test_inputs(p, false, f);
            assert!(success);
        }
    */
}
