use std::{
    fs::File,
    io::{self, BufRead, BufReader, Cursor, Error, ErrorKind},
    path::Path,
};

use super::dyn_patches::{self, FileID, CHANGE_SET};

/// Returns `BufReader` with patches applied to its input, if any.
/// If the file needs to be patched, this method will load the file content in the memory before applying any patches.
/// Can lead to a decrease in performance and an increase in memory consumption compared to streaming processing.
/// If there are no patches to apply, returns regular `BufReader<File>`.
pub fn get_reader(path: &Path) -> Result<Box<dyn BufRead>, std::io::Error> {
    let file_name = path
        .file_name()
        .ok_or_else(|| Error::new(ErrorKind::NotFound, "path doesn't point to a file"))?;
    let file_size = path.metadata()?.len();

    let file_id = FileID {
        file_name: file_name.to_os_string(),
        file_size,
    };

    let patches = match CHANGE_SET.patches.get(&file_id) {
        Some(patches) => patches,
        None => {
            let file = File::open(path)?;
            return Ok(Box::new(BufReader::new(file)));
        }
    };

    let mut data = std::fs::read(path)?;
    for patch in patches {
        match patch {
            dyn_patches::Patch::Decode { encoding } => {
                let (decoded_data, _, _) = encoding.decode(&data);
                data = decoded_data.as_bytes().to_vec();
            }
            dyn_patches::Patch::Replace {
                start,
                end,
                replacement,
            } => {
                let _: Vec<_> = data.splice(start..=end, replacement.clone()).collect();
            }
        }
    }

    Ok(Box::new(Cursor::new(data)))
}

/// Generates new screenshot name
pub fn screenshot_filename() -> String {
    chrono::Local::now().format("%Y-%m-%d_%H-%M-%S").to_string()
}

/// Read the entire content of a file as a string
pub fn read_file_as_string(path: &Path) -> Result<String, io::Error> {
    let mut reader = get_reader(path)?;
    let mut result = String::new();
    reader.read_to_string(&mut result)?;
    Ok(result)
}

/// Read the entire content of a file as byte vector
pub fn read_file_as_bytes(path: &Path) -> Result<Vec<u8>, io::Error> {
    let mut reader = get_reader(path)?;
    let mut result = Vec::new();
    reader.read_to_end(&mut result)?;
    Ok(result)
}

mod export {
    use std::os::raw::c_char;

    use log::error;

    use crate::common::{
        ffi::{c_char_to_str, CCharArray, U8Array},
        DEFAULT_LOGGER,
    };

    use super::{read_file_as_bytes, read_file_as_string, screenshot_filename};

    #[no_mangle]
    pub extern "C" fn ffi_screenshot_filename() -> *mut CCharArray {
        let filename: CCharArray = screenshot_filename().into();
        filename.into_raw()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_read_file_as_string(path: *const c_char) -> *mut CCharArray {
        let path = c_char_to_str(path).as_ref();
        match read_file_as_string(path) {
            Ok(data) => {
                let data: CCharArray = data.into();
                data.into_raw()
            }
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't read <{}> file: {}",
                    path.to_string_lossy(),
                    &e
                );
                std::ptr::null_mut()
            }
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_read_file_as_bytes(path: *const c_char) -> *mut U8Array {
        let path = c_char_to_str(path).as_ref();
        match read_file_as_bytes(path) {
            Ok(data) => {
                let data: U8Array = data.into();
                data.into_raw()
            }
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't read <{}> file: {}",
                    path.to_string_lossy(),
                    &e
                );
                std::ptr::null_mut()
            }
        }
    }
}
