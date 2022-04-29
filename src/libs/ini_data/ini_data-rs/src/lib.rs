use std::{ffi::CStr, os::raw::c_char};
use ini_data::IniData;

mod ini_data;

#[no_mangle]
pub extern "C" fn load_ini_data(logger_name: *const c_char, path: *const c_char) -> *mut IniData {
    let logger_name = c_char_to_str(logger_name);
    let path = c_char_to_str(path);
    match IniData::new(logger_name, path) {
        Some(data) => {
            let boxed_data = Box::new(data);
            Box::into_raw(boxed_data)
        },
        None => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn free_ini_data(ptr: *mut IniData) {
    unsafe {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    };
}

fn c_char_to_str<'a>(s: *const c_char) -> &'a str {
    let s_str = unsafe { CStr::from_ptr(s) };
    s_str.to_str().unwrap()
}