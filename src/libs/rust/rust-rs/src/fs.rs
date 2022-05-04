use std::path::{Path, PathBuf};

use log::{error, warn};

use crate::common::DEFAULT_LOGGER;

/// Returns `Documents/My Games/Sea Dogs/` directory
/// or `user_data` if it's not available for some reason
pub fn home_directory() -> PathBuf {
    match dirs_next::document_dir() {
        Some(path) => path.join("My Games").join("Sea Dogs"),
        None => {
            error!(
                target: DEFAULT_LOGGER,
                "Couldn't find `Documents` directory, using <user_data> instead"
            );
            PathBuf::from("user_data")
        }
    }
}

/// Returns either `Documents/My Games/Sea Dogs/Logs` or `user_data/Logs` directory
pub fn logs_directory() -> PathBuf {
    home_directory().join("Logs")
}

/// Returns either `Documents/My Games/Sea Dogs/SaveData` or `user_data/SaveData` directory
pub fn save_directory() -> PathBuf {
    home_directory().join("SaveData")
}

/// Returns either `Documents/My Games/Sea Dogs/Screenshots` or `user_data/Screenshots` directory
pub fn screenshot_directory() -> PathBuf {
    home_directory().join("Screenshots")
}

/// Generates new screenshot name
pub fn screenshot_filename() -> String {
    chrono::Local::now().format("%Y-%m-%d_%H-%M-%S").to_string()
}

/// Returns executable directory or panics if it's not available or the app doesn't have permissions
pub fn executable_directory() -> PathBuf {
    match std::env::current_dir() {
        Ok(dir) => dir,
        Err(e) => {
            error!(
                target: DEFAULT_LOGGER,
                "Couldn't get current directory: {}", &e
            );
            panic!("Current dir is unavailable")
        }
    }
}

/// Returns file size in bytes or panics if the file doesn't exist or the app doesn't have permissions
pub fn file_size(path: &Path) -> u64 {
    match path.metadata() {
        Ok(meta) => meta.len(),
        Err(e) => {
            error!(
                target: DEFAULT_LOGGER,
                "Couldn't get <{}>'s metadata: {}",
                &path.to_string_lossy(),
                &e
            );
            panic!("File is not available")
        }
    }
}

/// Attempts to delete a file
pub fn delete_file(path: &Path) -> bool {
    match std::fs::remove_file(&path) {
        Ok(_r) => true,
        Err(e) => {
            warn!(target: DEFAULT_LOGGER, "Couldn't delete <{}> file", &e);
            true
        }
    }
}

mod export {
    use std::os::raw::c_char;

    use crate::common::ffi::{c_char_to_str, uint64_t, CCharArray, WCharArray};

    use super::{
        delete_file, executable_directory, file_size, home_directory, logs_directory,
        save_directory, screenshot_directory, screenshot_filename,
    };

    #[no_mangle]
    pub extern "C" fn ffi_home_directory() -> *mut WCharArray {
        let array: WCharArray = home_directory().into();
        array.into_raw()
    }

    #[no_mangle]
    pub extern "C" fn ffi_logs_directory() -> *mut WCharArray {
        let array: WCharArray = logs_directory().into();
        array.into_raw()
    }

    #[no_mangle]
    pub extern "C" fn ffi_save_directory() -> *mut WCharArray {
        let array: WCharArray = save_directory().into();
        array.into_raw()
    }

    #[no_mangle]
    pub extern "C" fn ffi_screenshot_directory() -> *mut WCharArray {
        let array: WCharArray = screenshot_directory().into();
        array.into_raw()
    }

    #[no_mangle]
    pub extern "C" fn ffi_screenshot_filename() -> *mut CCharArray {
        let filename: CCharArray = screenshot_filename().into();
        filename.into_raw()
    }

    #[no_mangle]
    pub extern "C" fn ffi_executable_directory() -> *mut WCharArray {
        let array: WCharArray = executable_directory().into();
        array.into_raw()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_file_size(path: *const c_char) -> uint64_t {
        let path = c_char_to_str(path).as_ref();
        file_size(path)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_delete_file(path: *const c_char) -> bool {
        let path = c_char_to_str(path).as_ref();
        delete_file(path)
    }
}
