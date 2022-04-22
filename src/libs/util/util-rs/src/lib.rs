use std::borrow::{Borrow, Cow};
use std::ffi::CStr;
use std::os::raw::{c_char, c_int, c_ushort};
use std::os::windows::prelude::OsStrExt;
use std::path::PathBuf;

pub mod fs;
pub mod string_compare;

#[allow(non_camel_case_types)]
pub type size_t = usize;
#[allow(non_camel_case_types)]
pub type wchar_t = c_ushort;

#[repr(C)]
pub struct ArrayWchar {
    ptr: *mut wchar_t,
    len: size_t,
    capacity: size_t,
}

#[repr(C)]
pub struct ArrayCchar {
    ptr: *mut c_char,
    len: size_t,
    capacity: size_t,
}

impl From<PathBuf> for Box<ArrayWchar> {
    fn from(path: PathBuf) -> Self {
        let mut data = path
            .as_os_str()
            .encode_wide()
            .chain(Some(0))
            .collect::<Vec<_>>();

        // Vec already stores all data on heap, no need to box it.
        let ptr = data.as_mut_ptr();
        let len = data.len();
        let capacity = data.capacity();

        // Prevent `data` from deallocating by forcing Rust to "forget" about it
        std::mem::forget(data);

        let array = ArrayWchar { ptr, len, capacity };
        Box::new(array)
    }
}

impl From<String> for Box<ArrayCchar> {
    fn from(str: String) -> Self {
        let mut data = str.bytes()
            .chain(Some(0u8))
            .map(|b| b as c_char)
            .collect::<Vec<_>>();

        // Vec already stores all data on heap, no need to box it.
        let ptr = data.as_mut_ptr();
        let len = data.len();
        let capacity = data.capacity();

        // Prevent `data` from deallocating by forcing Rust to "forget" about it
        std::mem::forget(data);

        let array = ArrayCchar { ptr, len, capacity };
        Box::new(array)
    }
}

#[no_mangle]
pub extern "C" fn get_stash_path() -> *mut ArrayWchar {
    let array: Box<ArrayWchar> = fs::home_directory().into();
    Box::into_raw(array)
}

#[no_mangle]
pub extern "C" fn get_logs_path() -> *mut ArrayWchar {
    let array: Box<ArrayWchar> = fs::logs_directory().into();
    Box::into_raw(array)
}

#[no_mangle]
pub extern "C" fn get_save_data_path() -> *mut ArrayWchar {
    let array: Box<ArrayWchar> = fs::save_directory().into();
    Box::into_raw(array)
}

#[no_mangle]
pub extern "C" fn get_screenshots_path() -> *mut ArrayWchar {
    let array: Box<ArrayWchar> = fs::screenshot_directory().into();
    Box::into_raw(array)
}

#[no_mangle]
pub extern "C" fn get_screenshot_filename() -> *mut ArrayCchar {
    let filename: Box<ArrayCchar> = fs::screenshot_filename().into();
    Box::into_raw(filename)
}

#[no_mangle]
pub extern "C" fn free_array_wchar(ptr: *mut ArrayWchar) {
    unsafe {
        if ptr.is_null() {
            return;
        }
        let wrapper = Box::from_raw(ptr);
        Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
    };
}

#[no_mangle]
pub extern "C" fn free_array_cchar(ptr: *mut ArrayCchar) {
    unsafe {
        if ptr.is_null() {
            return;
        }
        let wrapper = Box::from_raw(ptr);
        Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
    };
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

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use crate::{free_array_wchar, ArrayWchar, ArrayCchar, free_array_cchar};

    // Testing correct allocation and deallocation using miri
    #[test]
    fn test_array_wchar() {
        let path = PathBuf::from("C:\\Users\\User\\Documents\\My Games\\Sea Dogs\\Logs");
        let array: Box<ArrayWchar> = path.into();
        let ptr = Box::into_raw(array);
        free_array_wchar(ptr);
    }

    #[test]
    fn test_array_cchar() {
        let path = "Test String".to_string();
        let array: Box<ArrayCchar> = path.into();
        let ptr = Box::into_raw(array);
        free_array_cchar(ptr);
    }
}
