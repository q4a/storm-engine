use core::slice;
use std::{
    borrow::Cow,
    ffi::{CStr, CString},
    os::{
        raw::{c_char, c_ushort},
        windows::prelude::OsStrExt,
    },
    path::PathBuf,
};

use log::error;

pub const DEFAULT_LOGGER: &str = "system";

#[allow(non_camel_case_types)]
pub type size_t = usize;
#[allow(non_camel_case_types)]
pub type wchar_t = c_ushort;

#[repr(C)]
pub struct WCharArray {
    ptr: *mut wchar_t,
    len: size_t,
    capacity: size_t,
}

impl WCharArray {
    pub fn into_raw(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

#[repr(C)]
pub struct CCharArray {
    ptr: *mut c_char,
    len: size_t,
    capacity: size_t,
}

impl CCharArray {
    pub fn into_raw(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

#[repr(C)]
pub struct ArrayOfCCharArrays {
    ptr: *mut CCharArray,
    len: size_t,
    capacity: size_t,
}

impl ArrayOfCCharArrays {
    pub fn into_raw(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
#[no_mangle]
pub unsafe extern "C" fn ffi_free_wchar_array(ptr: *mut WCharArray) {
    if ptr.is_null() {
        return;
    }
    let wrapper = Box::from_raw(ptr);
    Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
}

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
#[no_mangle]
pub unsafe extern "C" fn ffi_free_cchar_array(ptr: *mut CCharArray) {
    if ptr.is_null() {
        return;
    }
    let wrapper = Box::from_raw(ptr);
    Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
}

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
#[no_mangle]
pub unsafe extern "C" fn ffi_free_array_of_cchar_arrays(ptr: *mut ArrayOfCCharArrays) {
    if ptr.is_null() {
        return;
    }
    let wrapper = Box::from_raw(ptr);
    let vec = Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
    for c_char_array in vec {
        Vec::from_raw_parts(c_char_array.ptr, c_char_array.len, c_char_array.capacity);
    }
}

/// # Safety
///
/// Convert C/C++ `const *char` to Rust `&str`. Assumes that *char is not null
pub unsafe fn c_char_to_str<'a>(s: *const c_char) -> &'a str {
    let s_str = CStr::from_ptr(s);
    s_str.to_str().unwrap()
}

/// # Safety
///
/// Convert C/C++ `const *char` in Win1251 encoding to Rust `&str`. Assumes that *char is not null
pub unsafe fn win1251_char_to_str<'a>(s: *const c_char) -> Cow<'a, str> {
    let encoding = encoding_rs::WINDOWS_1251;
    let s = CStr::from_ptr(s).to_bytes_with_nul();
    let (s, _, _) = encoding.decode(s);
    s
}

/// # Safety
///
/// Copy value from Rust `&str` to C/C++ `*char`. Assumes that *char is not null and `size_t` is correct
pub unsafe fn copy_to_c_char(s: &str, ptr: *mut c_char, size: size_t) -> bool {
    let c_str = match CString::new(s) {
        Ok(s) => s,
        Err(e) => {
            error!(
                target: DEFAULT_LOGGER,
                "Couldn't copy &str to *c_char: {}", &e
            );
            return false;
        }
    };

    let bytes = c_str.as_bytes_with_nul();
    let out = slice::from_raw_parts_mut(ptr as *mut u8, size);
    out[..bytes.len()].copy_from_slice(bytes);

    true
}

impl From<PathBuf> for WCharArray {
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

        WCharArray { ptr, len, capacity }
    }
}

impl From<String> for CCharArray {
    fn from(str: String) -> Self {
        let mut data = str
            .bytes()
            .chain(Some(0u8))
            .map(|b| b as c_char)
            .collect::<Vec<_>>();

        // Vec already stores all data on heap, no need to box it.
        let ptr = data.as_mut_ptr();
        let len = data.len();
        let capacity = data.capacity();

        // Prevent `data` from deallocating by forcing Rust to "forget" about it
        std::mem::forget(data);

        CCharArray { ptr, len, capacity }
    }
}

impl From<Vec<String>> for ArrayOfCCharArrays {
    fn from(data: Vec<String>) -> Self {
        let mut data: Vec<_> = data.into_iter().map(|string| string.into()).collect();

        // Vec already stores all data on heap, no need to box it.
        let ptr = data.as_mut_ptr();
        let len = data.len();
        let capacity = data.capacity();

        // Prevent `data` from deallocating by forcing Rust to "forget" about it
        std::mem::forget(data);

        ArrayOfCCharArrays { ptr, len, capacity }
    }
}

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use super::{
        c_char_to_str, copy_to_c_char, ffi_free_array_of_cchar_arrays, ffi_free_cchar_array,
        ffi_free_wchar_array, ArrayOfCCharArrays, CCharArray, WCharArray,
    };

    /// Testing correct allocation and deallocation using miri
    #[test]
    fn test_wchar_array() {
        let data = PathBuf::from("C:\\Users\\UserName\\Documents\\My Games\\Sea Dogs\\Logs");
        let array: WCharArray = data.into();
        let ptr = array.into_raw();
        unsafe { ffi_free_wchar_array(ptr) };
    }

    /// Testing correct allocation and deallocation using miri
    #[test]
    fn test_cchar_array() {
        let data = "Test String".to_string();
        let array: CCharArray = data.into();
        let ptr = array.into_raw();
        unsafe { ffi_free_cchar_array(ptr) };
    }

    /// Testing correct allocation and deallocation using miri
    #[test]
    fn test_array_of_cchar_arrays() {
        let data = vec!["One".to_string(), "Two".to_string(), "Three".to_string()];
        let array: ArrayOfCCharArrays = data.into();
        let ptr = array.into_raw();
        unsafe { ffi_free_array_of_cchar_arrays(ptr) };
    }

    /// Testing correct copying from `&str` to `*char` and reading it back to `&str`
    #[test]
    fn test_copy_to_c_char() {
        let mut buffer = [1i8; 256];
        let ptr = buffer.as_mut_ptr();
        let data = "One いち один";

        unsafe {
            assert!(copy_to_c_char(data, ptr, 256));
            let result = c_char_to_str(ptr);
            assert_eq!(result, data);
        }
    }
}
