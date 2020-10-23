extern crate cc;

use std::env;
use std::path::Path;
use std::path::PathBuf;

#[cfg(all(target_env = "msvc", target_arch = "x86_64"))]
fn assembly(file_vec: &mut Vec<PathBuf>, base_dir: &str) {
    let files = glob::glob(&(base_dir.to_owned() + "win64/*-x86_64.asm"))
        .expect("disaster");
    for file in files {
        file_vec.push(file.unwrap());
    }
}

#[cfg(all(target_env = "msvc", target_arch = "aarch64"))]
fn assembly(file_vec: &mut Vec<PathBuf>, base_dir: &str) {
    let files = glob::glob(&(base_dir.to_owned() + "win64/*-armv8.asm"))
        .expect("disaster");
    for file in files {
        file_vec.push(file.unwrap());
    }
}

#[cfg(all(target_pointer_width = "64", not(target_env = "msvc")))]
fn assembly(file_vec: &mut Vec<PathBuf>, base_dir: &str) {
    file_vec.push(Path::new(base_dir).join("assembly.S"))
}

fn main() {
    /*
     * Use pre-built libblst.a if there is one. This is primarily
     * for trouble-shooting purposes. Idea is that libblst.a can be
     * compiled with flags independent from cargo defaults, e.g.
     * '../../build.sh -O1 ...'.
     */
    if Path::new("libblst_eip2537.a").exists() {
        println!("cargo:rustc-link-search=.");
        println!("cargo:rustc-link-lib=blst_eip2537");
        return;
    }

    let mut file_vec = Vec::new();

    let _out_dir = env::var_os("OUT_DIR").unwrap();

    let blst_base_dir = match env::var("BLST_SRC_DIR") {
        Ok(val) => val,
        Err(_) => {
            if Path::new("blst").exists() {
                "blst".to_string()
            } else {
                "../blst/".to_string()
            }
        }
    };
    println!("Using blst source directory {:?}", blst_base_dir);

    let blst_eip_src_dir = match env::var("BLST_EIP_SRC_DIR") {
        Ok(val) => val,
        Err(_) => "../src/".to_string(),
    };

    let c_src_dir = blst_base_dir.clone() + "/src/";
    let build_dir = blst_base_dir.clone() + "/build/";
    let include_dir = blst_base_dir + "/bindings";

    file_vec.push(Path::new(&c_src_dir).join("server.c"));
    file_vec.push(Path::new(&blst_eip_src_dir).join("eip2537.c"));
    assembly(&mut file_vec, &build_dir);

    // Set CC environment variable to choose alternative C compiler.
    // Optimization level depends on whether or not --release is passed
    // or implied.
    let mut cc = cc::Build::new();

    // account for cross-compilation
    let target_arch = env::var("CARGO_CFG_TARGET_ARCH").unwrap();
    match (cfg!(feature = "portable"), cfg!(feature = "force-adx")) {
        (true, false) => {
            println!("Compiling in portable mode without ISA extensions");
            cc.define("__BLST_PORTABLE__", None);
        }
        (false, true) => {
            if target_arch.eq("x86_64") {
                println!("Enabling ADX support via `force-adx` feature");
                cc.define("__ADX__", None);
            } else {
                println!("`force-adx` is ignored for non-x86_64 targets");
            }
        }
        (false, false) => {
            #[cfg(target_arch = "x86_64")]
            if target_arch.eq("x86_64") && std::is_x86_feature_detected!("adx")
            {
                println!("Enabling ADX because it was detected on the host");
                cc.define("__ADX__", None);
            }
        }
        (true, true) => panic!(
            "Cannot compile with both `portable` and `force-adx` features"
        ),
    }
    cc.flag_if_supported("-mno-avx") // avoid costly transitions
        .flag_if_supported("-Wno-unused-command-line-argument");
    if !cfg!(debug_assertions) {
        cc.opt_level(2);
    }
    cc.include(&include_dir);
    cc.files(&file_vec).compile("libblst_eip2537.a");
}
