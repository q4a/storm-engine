use configparser::ini::Ini;

use crate::common::DEFAULT_LOGGER;

pub struct IniData {
    data: Ini,
}

impl IniData {
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

        Some(IniData { data })
    }
}

mod export {
    use std::os::raw::c_char;

    use crate::common::c_char_to_str;

    use super::IniData;

    #[no_mangle]
    pub unsafe extern "C" fn load_ini_data(path: *const c_char) -> *mut IniData {
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
    pub unsafe extern "C" fn free_ini_data(ptr: *mut IniData) {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    }
}
