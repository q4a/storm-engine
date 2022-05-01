use configparser::ini::Ini;
use log::info;

use crate::common::DEFAULT_LOGGER;

pub struct IniData {
    data: Ini,
    path: String,
}

impl IniData {
    /// Read data from ini file
    pub fn new(path: &str) -> Option<IniData> {
        let mut data = Ini::new();
        let _ = match data.load(path) {
            Ok(_) => {}
            Err(e) => {
                log::error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't load ini file <{}>, reason: {}",
                    path,
                    e
                );
                return None;
            }
        };

        Some(IniData {
            data,
            path: path.to_string(),
        })
    }

    /// Get `key` parameter from `section`
    pub fn get_string(&self, section: &str, key: &str) -> Option<String> {
        let val = self.data.get(section, key);
        if val.is_none() {
            info!(
                target: DEFAULT_LOGGER,
                "Ini file <{}> doesn't have <{}> key in <{}> section", &self.path, key, section
            );
        }

        val
    }
}

mod export {
    use std::os::raw::c_char;

    use crate::common::{c_char_to_str, ArrayCchar};

    use super::IniData;

    #[no_mangle]
    pub unsafe extern "C" fn ffi_load_ini_data(path: *const c_char) -> *mut IniData {
        let path = c_char_to_str(path);
        match IniData::new(path) {
            Some(data) => {
                let boxed_data = Box::new(data);
                Box::into_raw(boxed_data)
            }
            None => std::ptr::null_mut(),
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_free_ini_data(ptr: *mut IniData) {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_string(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
    ) -> *mut ArrayCchar {
        match ptr.as_ref() {
            Some(ini) => {
                let section = c_char_to_str(section);
                let key = c_char_to_str(key);
                match ini.get_string(section, key) {
                    Some(val) => {
                        let val: Box<ArrayCchar> = val.into();
                        Box::into_raw(val)
                    }
                    None => std::ptr::null_mut(),
                }
            }
            None => std::ptr::null_mut(),
        }
    }
}
