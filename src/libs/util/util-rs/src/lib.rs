use std::path::PathBuf;
use std::{ffi::OsString, os::windows::prelude::OsStrExt};

mod fs;

#[no_mangle]
pub extern "C" fn get_stash_path() -> *mut winapi::ctypes::wchar_t {
    pathbuf_to_wchar(fs::home_directory())
}

#[no_mangle]
pub extern "C" fn get_logs_path() -> *mut winapi::ctypes::wchar_t {
    pathbuf_to_wchar(fs::logs_directory())
}

#[no_mangle]
pub extern "C" fn get_save_data_path() -> *mut winapi::ctypes::wchar_t {
    pathbuf_to_wchar(fs::save_directory())
}

#[no_mangle]
pub extern "C" fn get_screenshots_path() -> *mut winapi::ctypes::wchar_t {
    pathbuf_to_wchar(fs::screenshot_directory())
}

fn pathbuf_to_wchar(input: PathBuf) -> *mut winapi::ctypes::wchar_t {
    OsString::from(input)
        .encode_wide()
        .collect::<Vec<_>>()
        .as_mut_ptr()
}
