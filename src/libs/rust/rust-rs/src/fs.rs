use std::{
    io,
    path::{Path, PathBuf},
};

use log::error;

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
pub fn executable_directory() -> Result<PathBuf, io::Error> {
    std::env::current_dir()
}

/// Returns file size in bytes or panics if the file doesn't exist or the app doesn't have permissions
pub fn file_size(path: &Path) -> Result<u64, io::Error> {
    path.metadata().map(|m| m.len())
}

/// Delete a directory with all it's children
pub fn delete_directory(path: &Path) -> Result<(), io::Error> {
    std::fs::remove_dir_all(path)
}

/// Delete a file
pub fn delete_file(path: &Path) -> Result<(), io::Error> {
    std::fs::remove_file(&path)
}

/// Create a directory with all it's parents
pub fn create_directory(path: &Path) -> Result<(), io::Error> {
    std::fs::create_dir_all(path)
}

/// Check if file or directory this path points to exists. Any error will be coerced to `false`
pub fn path_exists(path: &Path) -> bool {
    path.exists()
}

mod export {
    use std::os::raw::c_char;

    use log::error;

    use crate::common::{
        ffi::{c_char_to_str, uint64_t, CCharArray, WCharArray},
        DEFAULT_LOGGER,
    };

    use super::{
        create_directory, delete_directory, delete_file, executable_directory, file_size,
        home_directory, logs_directory, save_directory, screenshot_directory, screenshot_filename, path_exists,
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
        let array: WCharArray = match executable_directory() {
            Ok(dir) => dir.into(),
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't get current directory: {}", &e
                );
                panic!("Current dir is unavailable")
            }
        };
        array.into_raw()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_file_size(path: *const c_char) -> uint64_t {
        let path = c_char_to_str(path).as_ref();
        match file_size(path) {
            Ok(size) => size,
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

    #[no_mangle]
    pub unsafe extern "C" fn ffi_delete_directory(path: *const c_char) {
        let path = c_char_to_str(path).as_ref();
        match delete_directory(path) {
            Ok(_r) => {}
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't delete <{}> directory: {}",
                    path.to_string_lossy(),
                    &e
                )
            }
        };
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_delete_file(path: *const c_char) -> bool {
        let path = c_char_to_str(path).as_ref();
        match delete_file(path) {
            Ok(_r) => true,
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't delete <{}> file: {}",
                    path.to_string_lossy(),
                    &e
                );
                true
            }
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_create_directory(path: *const c_char) -> bool {
        let path = c_char_to_str(path).as_ref();
        match create_directory(path) {
            Ok(_r) => true,
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't create <{}> directory: {}",
                    path.to_string_lossy(),
                    &e
                );
                false
            }
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_path_exists(path: *const c_char) -> bool {
        let path = c_char_to_str(path).as_ref();
        path_exists(path)
    }
}
