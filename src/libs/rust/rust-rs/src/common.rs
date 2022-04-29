use std::{
    borrow::Cow,
    ffi::CStr,
    os::{
        raw::{c_char, c_ushort},
        windows::prelude::OsStrExt,
    },
    path::PathBuf,
};

pub const DEFAULT_LOGGER: &str = "system";

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

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
#[no_mangle]
pub unsafe extern "C" fn free_array_wchar(ptr: *mut ArrayWchar) {
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
pub unsafe extern "C" fn free_array_cchar(ptr: *mut ArrayCchar) {
    if ptr.is_null() {
        return;
    }
    let wrapper = Box::from_raw(ptr);
    Vec::from_raw_parts(wrapper.ptr, wrapper.len, wrapper.capacity);
}

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
pub unsafe fn c_char_to_str<'a>(s: *const c_char) -> &'a str {
    let s_str = CStr::from_ptr(s);
    s_str.to_str().unwrap()
}

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
pub unsafe fn win1251_char_to_str<'a>(s: *const c_char) -> Cow<'a, str> {
    let encoding = encoding_rs::WINDOWS_1251;
    let s = CStr::from_ptr(s).to_bytes_with_nul();
    let (s, _, _) = encoding.decode(s);
    s
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

        let array = ArrayCchar { ptr, len, capacity };
        Box::new(array)
    }
}

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use super::{free_array_cchar, free_array_wchar, ArrayCchar, ArrayWchar};

    /// Testing correct allocation and deallocation using miri
    #[test]
    fn test_array_wchar() {
        let path = PathBuf::from("C:\\Users\\UserName\\Documents\\My Games\\Sea Dogs\\Logs");
        let array: Box<ArrayWchar> = path.into();
        let ptr = Box::into_raw(array);
        unsafe { free_array_wchar(ptr) };
    }

    /// Testing correct allocation and deallocation using miri
    #[test]
    fn test_array_cchar() {
        let path = "Test String".to_string();
        let array: Box<ArrayCchar> = path.into();
        let ptr = Box::into_raw(array);
        unsafe { free_array_cchar(ptr) };
    }
}
