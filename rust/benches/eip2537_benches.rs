// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

use blst_eip2537::*;
use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion};
use rand::{RngCore, SeedableRng};
use rand_chacha::ChaCha20Rng;

fn gen_fp(rng: &mut rand_chacha::ChaCha20Rng) -> [u8; 64] {
    let mut fp = [0u8; 64];
    rng.fill_bytes(&mut fp);

    for i in 0..16 {
        fp[i] = 0;
    }

    // Very cheap hack to be below modulus
    if fp[16] >= 0x1A {
        fp[16] &= 0x0F;
    }

    return fp;
}

fn gen_g1_point(rng: &mut rand_chacha::ChaCha20Rng) -> [u8; 128] {
    let fp = gen_fp(rng);
    let g1 = blstEIP2537Executor::map_fp_to_g1(&fp);
    assert_eq!(g1.is_ok(), true);
    return g1.unwrap();
}

fn gen_g2_point(rng: &mut rand_chacha::ChaCha20Rng) -> [u8; 256] {
    let mut fp2 = Vec::with_capacity(128);
    fp2.extend(&gen_fp(rng));
    fp2.extend(&gen_fp(rng));
    let g2 = blstEIP2537Executor::map_fp2_to_g2(&fp2);
    assert_eq!(g2.is_ok(), true);
    return g2.unwrap();
}

fn bench_g1(c: &mut Criterion) {
    let mut group = c.benchmark_group("g1");

    let seed = [0u8; 32];
    let mut rng = ChaCha20Rng::from_seed(seed);

    let g1_point_a = gen_g1_point(&mut rng);
    let g1_point_b = gen_g1_point(&mut rng);
    let mut g1_for_add = Vec::with_capacity(256);
    g1_for_add.extend(&g1_point_a);
    g1_for_add.extend(&g1_point_b);

    group.bench_function("g1_add", |b| {
        b.iter(|| blstEIP2537Executor::g1_add(&g1_for_add));
    });

    let mut g1_for_mul = Vec::with_capacity(160);
    g1_for_mul.extend(&g1_point_a);

    let mut scalar = [0u8; 32];
    rng.fill_bytes(&mut scalar);
    g1_for_mul.extend(&scalar);

    group.bench_function("g1_mul", |b| {
        b.iter(|| blstEIP2537Executor::g1_mul(&g1_for_mul));
    });

    let multiexp_sizes =
        vec![2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096];
    for n in multiexp_sizes.iter() {
        let mut pairs_for_multiexp = Vec::with_capacity(160 * n);

        for _ in 0..*n {
            let g1_point = gen_g1_point(&mut rng);
            pairs_for_multiexp.extend(&g1_point);
            let mut scalar = [0u8; 32];
            rng.fill_bytes(&mut scalar);
            pairs_for_multiexp.extend(&scalar);
        }
        group.bench_with_input(
            BenchmarkId::new("g1_multiexp", n),
            &pairs_for_multiexp,
            |b, p| {
                b.iter(|| blstEIP2537Executor::g1_multiexp(&p));
            },
        );
    }

    group.finish();
}

fn bench_g2(c: &mut Criterion) {
    let mut group = c.benchmark_group("g2");

    let seed = [0u8; 32];
    let mut rng = ChaCha20Rng::from_seed(seed);

    let g2_point_a = gen_g2_point(&mut rng);
    let g2_point_b = gen_g2_point(&mut rng);
    let mut g2_for_add = Vec::with_capacity(512);
    g2_for_add.extend(&g2_point_a);
    g2_for_add.extend(&g2_point_b);

    group.bench_function("g2_add", |b| {
        b.iter(|| blstEIP2537Executor::g2_add(&g2_for_add));
    });

    let mut g2_for_mul = Vec::with_capacity(288);
    g2_for_mul.extend(&g2_point_a);

    let mut scalar = [0u8; 32];
    rng.fill_bytes(&mut scalar);
    g2_for_mul.extend(&scalar);

    group.bench_function("g2_mul", |b| {
        b.iter(|| blstEIP2537Executor::g2_mul(&g2_for_mul));
    });

    let multiexp_sizes =
        vec![2, 4, 8, 16, 32, 64, 128, 256, 512, 1024];
    for n in multiexp_sizes.iter() {
        let mut pairs_for_multiexp = Vec::with_capacity(288 * n);

        for _ in 0..*n {
            let g2_point = gen_g2_point(&mut rng);
            pairs_for_multiexp.extend(&g2_point);
            let mut scalar = [0u8; 32];
            rng.fill_bytes(&mut scalar);
            pairs_for_multiexp.extend(&scalar);
        }
        group.bench_with_input(
            BenchmarkId::new("g2_multiexp", n),
            &pairs_for_multiexp,
            |b, p| {
                b.iter(|| blstEIP2537Executor::g2_multiexp(&p));
            },
        );
    }

    group.finish();
}

fn bench_pairing(c: &mut Criterion) {
    let mut group = c.benchmark_group("pairing");

    let seed = [0u8; 32];
    let mut rng = ChaCha20Rng::from_seed(seed);

    let pairing_sizes = vec![2, 4, 8, 16, 32, 64, 128, 256, 512];
    for n in pairing_sizes.iter() {
        let mut pairs = Vec::with_capacity(384 * n);
        for _ in 0..*n {
            let g1_point = gen_g1_point(&mut rng);
            let g2_point = gen_g2_point(&mut rng);
            pairs.extend(&g1_point);
            pairs.extend(&g2_point);
        }

        group.bench_with_input(
            BenchmarkId::new("pairing", n),
            &pairs,
            |b, p| {
                b.iter(|| blstEIP2537Executor::pairing(&p));
            },
        );
    }

    group.finish();
}

fn bench_map(c: &mut Criterion) {
    let mut group = c.benchmark_group("map");

    let seed = [0u8; 32];
    let mut rng = ChaCha20Rng::from_seed(seed);

    let fp = gen_fp(&mut rng);
    group.bench_function("map_fp_to_g1", |b| {
        b.iter(|| blstEIP2537Executor::map_fp_to_g1(&fp));
    });

    let mut fp2 = Vec::with_capacity(128);
    fp2.extend(&gen_fp(&mut rng));
    fp2.extend(&gen_fp(&mut rng));
    group.bench_function("map_fp2_to_g2", |b| {
        b.iter(|| blstEIP2537Executor::map_fp2_to_g2(&fp2));
    });

    group.finish();
}

criterion_group!(
    benches,
    bench_g1,
    bench_g2,
    bench_pairing,
    bench_map
);
criterion_main!(benches);
