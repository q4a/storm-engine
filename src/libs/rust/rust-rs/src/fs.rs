use std::path::PathBuf;

use crate::common::DEFAULT_LOGGER;

/// Returns `Documents/My Games/Sea Dogs/` directory
/// or `user_data` if it's not available for some reason
fn home_directory() -> PathBuf {
    match dirs_next::document_dir() {
        Some(path) => path.join("My Games").join("Sea Dogs"),
        None => {
            log::error!(
                target: DEFAULT_LOGGER,
                "Couldn't find `Documents` directory, using <user_data> instead"
            );
            PathBuf::from("user_data")
        }
    }
}

/// Returns either `Documents/My Games/Sea Dogs/Logs` or `user_data/Logs` directory
fn logs_directory() -> PathBuf {
    home_directory().join("Logs")
}

/// Returns either `Documents/My Games/Sea Dogs/SaveData` or `user_data/SaveData` directory
fn save_directory() -> PathBuf {
    home_directory().join("SaveData")
}

/// Returns either `Documents/My Games/Sea Dogs/Screenshots` or `user_data/Screenshots` directory
fn screenshot_directory() -> PathBuf {
    home_directory().join("Screenshots")
}

/// Generates new screenshot name
fn screenshot_filename() -> String {
    chrono::Local::now().format("%Y-%m-%d_%H-%M-%S").to_string()
}

mod export {
    use crate::common::{ArrayCchar, ArrayWchar};

    use super::{
        home_directory, logs_directory, save_directory, screenshot_directory, screenshot_filename,
    };

    #[no_mangle]
    pub extern "C" fn ffi_home_directory() -> *mut ArrayWchar {
        let array: Box<ArrayWchar> = home_directory().into();
        Box::into_raw(array)
    }

    #[no_mangle]
    pub extern "C" fn ffi_logs_directory() -> *mut ArrayWchar {
        let array: Box<ArrayWchar> = logs_directory().into();
        Box::into_raw(array)
    }

    #[no_mangle]
    pub extern "C" fn ffi_save_directory() -> *mut ArrayWchar {
        let array: Box<ArrayWchar> = save_directory().into();
        Box::into_raw(array)
    }

    #[no_mangle]
    pub extern "C" fn ffi_screenshot_directory() -> *mut ArrayWchar {
        let array: Box<ArrayWchar> = screenshot_directory().into();
        Box::into_raw(array)
    }

    #[no_mangle]
    pub extern "C" fn ffi_screenshot_filename() -> *mut ArrayCchar {
        let filename: Box<ArrayCchar> = screenshot_filename().into();
        Box::into_raw(filename)
    }
}
