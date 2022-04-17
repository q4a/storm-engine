use std::borrow::{Borrow, Cow};
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
    let message = c_char_to_str(message);
    error!("{}", message);
}

#[no_mangle]
pub extern "C" fn warn(message: *const c_char) {
    let message = c_char_to_str(message);
    warn!("{}", message);
}

#[no_mangle]
pub extern "C" fn info(message: *const c_char) {
    let message = c_char_to_str(message);
    info!("{}", message);
}

#[no_mangle]
pub extern "C" fn debug(message: *const c_char) {
    let message = c_char_to_str(message);
    debug!("{}", message);
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
    s: *const c_char,
    pattern: *const c_char,
    start: size_t,
) -> c_int {
    let s = c_char_to_str(s);
    let pattern = c_char_to_str(pattern);
    string_compare::ignore_case_find(s, pattern, start)
        .map(|index| index as i32)
        .unwrap_or(-1)
}

#[no_mangle]
pub extern "C" fn ignore_case_starts_with(s: *const c_char, pattern: *const c_char) -> bool {
    let s = c_char_to_str(s);
    let pattern = c_char_to_str(pattern);

    string_compare::ignore_case_starts_with(s, pattern)
}

#[no_mangle]
pub extern "C" fn ignore_case_equal(first: *const c_char, second: *const c_char) -> bool {
    let first = c_char_to_str(first);
    let second = c_char_to_str(second);

    string_compare::ignore_case_equal(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_equal_win1251(first: *const c_char, second: *const c_char) -> bool {
    let first = win1251_char_to_str(first);
    let second = win1251_char_to_str(second);

    string_compare::ignore_case_equal(first.borrow(), second.borrow())
}

#[no_mangle]
pub extern "C" fn equal(first: *const c_char, second: *const c_char) -> bool {
    let first = c_char_to_str(first);
    let second = c_char_to_str(second);

    first.eq(second)
}

#[no_mangle]
pub extern "C" fn ignore_case_less(first: *const c_char, second: *const c_char) -> bool {
    let first = c_char_to_str(first);
    let second = c_char_to_str(second);

    string_compare::ignore_case_less(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_less_or_equal(first: *const c_char, second: *const c_char) -> bool {
    let first = c_char_to_str(first);
    let second = c_char_to_str(second);

    string_compare::ignore_case_less_or_equal(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_glob(s: *const c_char, pattern: *const c_char) -> bool {
    let s = c_char_to_str(s);
    let pattern = c_char_to_str(pattern);

    string_compare::ignore_case_glob(s, pattern)
}

fn pathbuf_to_wchar(input: PathBuf) -> *mut wchar_t {
    OsString::from(input)
        .encode_wide()
        .chain(Some(0))
        .collect::<Vec<_>>()
        .as_mut_ptr()
}

fn c_char_to_str<'a>(s: *const c_char) -> &'a str {
    let s_str = unsafe { CStr::from_ptr(s) };
    s_str.to_str().unwrap()
}

fn win1251_char_to_str<'a>(s: *const c_char) -> Cow<'a, str> {
    let encoding = encoding_rs::WINDOWS_1251;
    let s = unsafe { CStr::from_ptr(s).to_bytes_with_nul() };
    let (s, _, _) = encoding.decode(s);
    s
}
