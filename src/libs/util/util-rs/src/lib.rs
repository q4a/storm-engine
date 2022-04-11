use std::ffi::CStr;
use std::path::PathBuf;
use std::{ffi::OsString, os::windows::prelude::OsStrExt};

use winapi::{ctypes::*, vc::vcruntime::size_t};

mod fs;
mod string_compare;

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
pub extern "C" fn ignore_case_equal(first: *const c_char, second: *const c_char) -> bool {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();
    string_compare::ignore_case_equal(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_less(first: *const c_char, second: *const c_char) -> bool {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();
    string_compare::ignore_case_less(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_less_or_equal(first: *const c_char, second: *const c_char) -> bool {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();
    string_compare::ignore_case_less_or_equal(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_greater(first: *const c_char, second: *const c_char) -> bool {
    !ignore_case_less_or_equal(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_greater_or_equal(
    first: *const c_char,
    second: *const c_char,
) -> bool {
    !ignore_case_less(first, second)
}

#[no_mangle]
pub extern "C" fn ignore_case_equal_first_n(
    first: *const c_char,
    second: *const c_char,
    count: size_t,
) -> bool {
    let fisrt_str = unsafe { CStr::from_ptr(first) };
    let first = fisrt_str.to_str().unwrap();

    let second_str = unsafe { CStr::from_ptr(second) };
    let second = second_str.to_str().unwrap();
    string_compare::ignore_case_equal_first_n(first, second, count)
}

fn pathbuf_to_wchar(input: PathBuf) -> *mut wchar_t {
    OsString::from(input)
        .encode_wide()
        .collect::<Vec<_>>()
        .as_mut_ptr()
}
