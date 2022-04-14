use std::ffi::CStr;
use std::path::PathBuf;
use std::{ffi::OsString, os::windows::prelude::OsStrExt};

use env_logger::{Builder, Target};
use winapi::{ctypes::*, vc::vcruntime::size_t};

#[macro_use]
extern crate log;

mod fs;
mod string_compare;

#[no_mangle]
pub extern "C" fn init_logger() {
    let mut builder = Builder::from_default_env();
    builder.target(Target::Stdout);
    builder.filter_level(log::LevelFilter::Debug);
    builder.init();
    error!("Test error");
    warn!("Test warn");
    info!("Test info");
    debug!("Test debug");
}

#[no_mangle]
pub extern "C" fn error(message: *const c_char) {
    let message_c_str = unsafe { CStr::from_ptr(message) };
    let message_str = message_c_str.to_str().unwrap();
    error!("{}", message_str);
}

#[no_mangle]
pub extern "C" fn warn(message: *const c_char) {
    let message_c_str = unsafe { CStr::from_ptr(message) };
    let message_str = message_c_str.to_str().unwrap();
    warn!("{}", message_str);
}

#[no_mangle]
pub extern "C" fn info(message: *const c_char) {
    let message_c_str = unsafe { CStr::from_ptr(message) };
    let message_str = message_c_str.to_str().unwrap();
    info!("{}", message_str);
}

#[no_mangle]
pub extern "C" fn debug(message: *const c_char) {
    let message_c_str = unsafe { CStr::from_ptr(message) };
    let message_str = message_c_str.to_str().unwrap();
    debug!("{}", message_str);
}

#[no_mangle]
pub extern "C" fn get_stash_path() -> *mut wchar_t {
    pathbuf_to_wchar(fs::home_directory())
}

#[no_mangle]
pub extern "C" fn get_logs_path() -> *mut wchar_t {
    pathbuf_to_wchar(fs::logs_directory())
}

#[no_mangle]
pub extern "C" fn get_save_data_path() -> *mut wchar_t {
    pathbuf_to_wchar(fs::save_directory())
}

#[no_mangle]
pub extern "C" fn get_screenshots_path() -> *mut wchar_t {
    pathbuf_to_wchar(fs::screenshot_directory())
}

#[no_mangle]
pub extern "C" fn ignore_case_find(
    first: *const c_char,
    second: *const c_char,
    start: size_t,
) -> c_int {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();
    string_compare::ignore_case_find(first, second, start)
        .map(|index| index as i32)
        .unwrap_or(-1)
}

#[no_mangle]
pub extern "C" fn ignore_case_starts_with(first: *const c_char, second: *const c_char) -> bool {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();

    string_compare::ignore_case_starts_with(first, second)
}

fn pathbuf_to_wchar(input: PathBuf) -> *mut wchar_t {
    OsString::from(input)
        .encode_wide()
        .chain(Some(0))
        .collect::<Vec<_>>()
        .as_mut_ptr()
}
