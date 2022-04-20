use std::borrow::{Borrow, Cow};
use std::ffi::CStr;
use std::os::raw::{c_char, c_int, c_ushort};
use std::path::PathBuf;
use std::{ffi::OsString, os::windows::prelude::OsStrExt};

pub mod fs;
pub mod string_compare;

#[allow(non_camel_case_types)]
pub type size_t = usize;
#[allow(non_camel_case_types)]
pub type wchar_t = c_ushort;

#[no_mangle]
pub extern "C" fn get_stash_path() -> *const wchar_t {
    pathbuf_to_wchar(fs::home_directory())
}

#[no_mangle]
pub extern "C" fn get_logs_path() -> *const wchar_t {
    pathbuf_to_wchar(fs::logs_directory())
}

#[no_mangle]
pub extern "C" fn get_save_data_path() -> *const wchar_t {
    pathbuf_to_wchar(fs::save_directory())
}

#[no_mangle]
pub extern "C" fn get_screenshots_path() -> *const wchar_t {
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

fn pathbuf_to_wchar(input: PathBuf) -> *const wchar_t {
    OsString::from(input)
        .encode_wide()
        .chain(Some(0))
        .collect::<Vec<_>>()
        .as_ptr()
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
