use log::error;
use std::{
    collections::HashMap,
    fs::File,
    io::{BufRead, BufReader},
    path::{Path, PathBuf},
    str::FromStr,
};
use thiserror::Error;

use crate::common::DEFAULT_LOGGER;

const DEFAULT_SECTION: &str = "default";

#[derive(Error, Debug)]
pub enum IniParserError {
    #[error("Encountered IO error: {0}")]
    IoError(#[from] std::io::Error),
    #[error("Section doesn't have closing ] bracket: {0}")]
    SectionBracket(String),
    #[error("Section has closing ] bracket, but it's inside a comment: {0}")]
    SectionComment(String),
    #[error("Don't know how to parse the string: {0}")]
    UnknownString(String),
    #[error("Can't split the string into key and value: {0}")]
    NoKey(String),
}

/// Parser for ini-configurations to be used with the storm-engine.
/// Supports comments, sections and values without sections (`default` section will be used in such cases).
/// This parser also supports multiple values for the same `key` (values will be merged in a single `Vec`).
/// Comments at the end of value are NOT supported
pub struct IniData {
    sections: HashMap<String, HashMap<String, Vec<String>>>,
    path: PathBuf,
}

impl IniData {
    /// Parse data from `BufRead` into `IniData`.
    fn parse(reader: &mut dyn BufRead, path: PathBuf) -> Result<Self, IniParserError> {
        let mut data = Self {
            sections: HashMap::new(),
            path,
        };

        let mut last_section = DEFAULT_SECTION.to_string();
        for line in reader.lines() {
            let original_line = line?;
            let trimmed_line = original_line.trim();
            if trimmed_line.starts_with(';') || trimmed_line.starts_with("//") || trimmed_line.is_empty() {
                continue;
            }

            if trimmed_line.starts_with('[') {
                let closing_bracket_index = trimmed_line.find(']');
                let comment = trimmed_line.find(';');

                let bracket_index = match (closing_bracket_index, comment) {
                    (None, _) => return Err(IniParserError::SectionBracket(original_line)),
                    (Some(n), None) => n,
                    (Some(n1), Some(n2)) => {
                        if n1 < n2 {
                            n1
                        } else {
                            return Err(IniParserError::SectionComment(original_line));
                        }
                    }
                };

                last_section = trimmed_line[1..bracket_index].trim().to_lowercase();
                continue;
            }

            match trimmed_line.split_once('=') {
                Some((key, new_value)) => {
                    let section_data = data
                        .sections
                        .entry(last_section.clone())
                        .or_insert_with(HashMap::new);

                    let key_data = section_data
                        .entry(key.trim().to_lowercase())
                        .or_insert_with(Vec::new);

                    let new_value = if let Some(comment) = new_value.find(';') {
                        &new_value[..comment]
                    } else {
                        new_value
                    };
                    key_data.push(new_value.trim().to_string());
                }
                None => return Err(IniParserError::NoKey(original_line)),
            }
        }

        Ok(data)
    }

    /// Read file and parse it into `IniData`
    pub fn load(path: &Path) -> Result<Self, IniParserError> {
        let file = File::open(path)?;
        let mut reader = BufReader::new(file);

        Self::parse(&mut reader, path.to_path_buf())
    }

    /// Get single string value by it's `section` and `key`.
    /// If `section` is `None`, the `default` section is used.
    pub fn get_string(&self, section: Option<&str>, key: &str) -> Option<&str> {
        let section = section.unwrap_or(DEFAULT_SECTION).to_lowercase();
        let value = self
            .sections
            .get(&section)?
            .get(&key.to_lowercase())?
            .first()?;

        Some(value)
    }

    /// Get all values associated with this `section` and `key`.
    /// If `section` is `None`, the `default` section is used.
    pub fn get_multiple_strings(&self, section: Option<&str>, key: &str) -> Option<&Vec<String>> {
        let section = section.unwrap_or(DEFAULT_SECTION).to_lowercase();
        self.sections.get(&section)?.get(&key.to_lowercase())
    }

    /// Count the amount of values associated with this `section` and `key`.
    /// If `section` is `None`, the `default` section is used.
    pub fn get_amount_of_values(&self, section: Option<&str>, key: &str) -> usize {
        let section = section.unwrap_or(DEFAULT_SECTION).to_lowercase();
        let section = match self.sections.get(&section) {
            Some(section) => section,
            None => return 0,
        };

        section.get(&key.to_lowercase()).map_or(0, |v| v.len())
    }

    /// Try to get the value and parse it as an integer
    pub fn get_value<T: FromStr>(&self, section: Option<&str>, key: &str) -> Option<T> {
        let value = self.get_string(section, key)?;
        match value.parse::<T>() {
            Ok(v) => Some(v),
            Err(_) => {
                error!(target: DEFAULT_LOGGER, "Couldn't parse <{}> value", value);
                None
            }
        }
    }
}

mod export {
    use std::{
        os::raw::{c_char, c_double, c_float, c_int},
        path::Path,
        str::FromStr,
    };

    use log::{error, warn};

    use crate::common::{
        ffi::{c_char_to_str, copy_to_c_char, size_t, ArrayOfCCharArrays},
        DEFAULT_LOGGER,
    };

    use super::IniData;

    #[no_mangle]
    pub unsafe extern "C" fn ffi_load_ini_data(path: *const c_char) -> *mut IniData {
        let path = c_char_to_str(path);
        match IniData::load(Path::new(path)) {
            Ok(data) => {
                let boxed = Box::new(data);
                Box::into_raw(boxed)
            }
            Err(e) => {
                error!(
                    target: DEFAULT_LOGGER,
                    "Couldn't load ini-file <{}>: {}", path, &e
                );
                std::ptr::null_mut()
            }
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
        buffer: *mut c_char,
        buffer_size: size_t,
    ) -> bool {
        match ptr.as_ref() {
            Some(ini) => {
                let section = get_section_name(section);
                let key = c_char_to_str(key);
                match ini.get_string(section, key) {
                    Some(v) => {
                        copy_to_c_char(v, buffer, buffer_size);
                        true
                    }
                    None => {
                        warn!(
                            target: DEFAULT_LOGGER,
                            "File <{}>, section <{:?}>, key <{}>: doesn't exist",
                            &ini.path.to_string_lossy(),
                            section,
                            key
                        );
                        false
                    }
                }
            }
            None => false,
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_multiple_strings(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
    ) -> *mut ArrayOfCCharArrays {
        match ptr.as_ref() {
            Some(ini) => {
                let section = get_section_name(section);
                let key = c_char_to_str(key);
                match ini.get_multiple_strings(section, key) {
                    Some(val) => {
                        let array: ArrayOfCCharArrays = val.to_owned().into();
                        array.into_raw()
                    }
                    None => std::ptr::null_mut(),
                }
            }
            None => std::ptr::null_mut(),
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_amount_of_values(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
    ) -> size_t {
        match ptr.as_ref() {
            Some(ini) => {
                let section = get_section_name(section);
                let key = c_char_to_str(key);
                ini.get_amount_of_values(section, key)
            }
            None => 0,
        }
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_int(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
        default_value: c_int,
    ) -> c_int {
        get_value(ptr, section, key, default_value)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_double(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
        default_value: c_double,
    ) -> c_double {
        get_value(ptr, section, key, default_value)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_float(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
        default_value: c_float,
    ) -> c_float {
        get_value(ptr, section, key, default_value)
    }

    unsafe fn get_section_name<'a>(section: *const c_char) -> Option<&'a str> {
        if section.is_null() {
            None
        } else {
            Some(c_char_to_str(section))
        }
    }

    unsafe fn get_value<T: FromStr>(
        ptr: *mut IniData,
        section: *const c_char,
        key: *const c_char,
        default_value: T,
    ) -> T {
        match ptr.as_ref() {
            Some(ini) => {
                let section = get_section_name(section);
                let key = c_char_to_str(key);
                match ini.get_value(section, key) {
                    Some(v) => v,
                    None => {
                        warn!(
                            target: DEFAULT_LOGGER,
                            "File <{}>, section <{:?}>, key <{}>: doesn't exist",
                            &ini.path.to_string_lossy(),
                            section,
                            key
                        );
                        default_value
                    }
                }
            }
            None => default_value,
        }
    }
}

#[cfg(test)]
mod tests {
    use std::path::PathBuf;

    use super::IniData;

    fn mock_path() -> PathBuf {
        PathBuf::from("path.txt")
    }

    #[test]
    fn empty_data_test() {
        let mut data = r"".as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 0);
    }

    #[test]
    fn comment_test() {
        let mut data = r"; this is a comment".as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 0);
    }

    #[test]
    fn empty_section_test() {
        let mut data = r"
[models]
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 0);
    }

    #[test]
    fn default_section_test() {
        let mut data = r"
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(None, "model"), 1);
        assert_eq!(result.get_string(None, "model"), Some(r"resource\land"));
    }

    #[test]
    fn no_section_with_value_with_comment_test() {
        let mut data = r"
; This is a comment
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(None, "model"), 1);
        assert_eq!(result.get_string(None, "model"), Some(r"resource\land"));
    }

    #[test]
    fn with_section_with_value_test() {
        let mut data = r"
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn with_section_with_value_with_comment_test() {
        let mut data = r"
;this is a comment
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn value_with_comment_test() {
        let mut data = r"
[models]
model	= resource\land ;this is a comment
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn non_existing_section_test() {
        let mut data = r"
;this is a comment
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models1"), "model"), 0);
        assert_eq!(result.get_string(Some("models1"), "model"), None);
    }

    #[test]
    fn non_existing_key_test() {
        let mut data = r"
;this is a comment
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model1"), 0);
        assert_eq!(result.get_string(Some("models"), "model1"), None);
    }

    #[test]
    fn section_with_comment_same_line_test() {
        let mut data = r"
[models] ;this is a comment
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn ignore_case_section_name1_test() {
        let mut data = r"
;this is a comment
[MODELS]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn ignore_case_section_name2_test() {
        let mut data = r"
;this is a comment
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("MODELS"), "model"), 1);
        assert_eq!(
            result.get_string(Some("MODELS"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn ignore_case_key_name1_test() {
        let mut data = r"
;this is a comment
[models]
MODEL	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "model"), 1);
        assert_eq!(
            result.get_string(Some("models"), "model"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn ignore_case_key_name2_test() {
        let mut data = r"
;this is a comment
[models]
model	= resource\land
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("models"), "MODEL"), 1);
        assert_eq!(
            result.get_string(Some("models"), "MODEL"),
            Some(r"resource\land")
        );
    }

    #[test]
    fn multiple_keys_test() {
        let mut data = r"
[TABLE_ICONSELECTOR]
position = 133,189,263,276
rowquantity = 4
colquantity = 1
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(
            result.get_amount_of_values(Some("TABLE_ICONSELECTOR"), "position"),
            1
        );
        assert_eq!(
            result.get_string(Some("TABLE_ICONSELECTOR"), "position"),
            Some(r"133,189,263,276")
        );
        assert_eq!(
            result.get_amount_of_values(Some("TABLE_ICONSELECTOR"), "rowquantity"),
            1
        );
        assert_eq!(
            result.get_string(Some("TABLE_ICONSELECTOR"), "rowquantity"),
            Some(r"4")
        );
        assert_eq!(
            result.get_amount_of_values(Some("TABLE_ICONSELECTOR"), "colquantity"),
            1
        );
        assert_eq!(
            result.get_string(Some("TABLE_ICONSELECTOR"), "colquantity"),
            Some(r"1")
        );
    }

    #[test]
    fn multiple_sections_test() {
        let mut data = r"
[MAIN]
item	= VIDEO,VIDEOBASE
[EXIT_BTN]
position	= 777,0,799,22
[SCROLLPICT]
bAbsoluteRectangle = 5
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 3);
        assert_eq!(result.get_amount_of_values(Some("MAIN"), "item"), 1);
        assert_eq!(
            result.get_string(Some("MAIN"), "item"),
            Some(r"VIDEO,VIDEOBASE")
        );
        assert_eq!(result.get_amount_of_values(Some("EXIT_BTN"), "position"), 1);
        assert_eq!(
            result.get_string(Some("EXIT_BTN"), "position"),
            Some(r"777,0,799,22")
        );
        assert_eq!(
            result.get_amount_of_values(Some("SCROLLPICT"), "bAbsoluteRectangle"),
            1
        );
        assert_eq!(
            result.get_string(Some("SCROLLPICT"), "bAbsoluteRectangle"),
            Some(r"5")
        );
    }

    #[test]
    fn first_of_multiple_values_test() {
        let mut data = r"
[MAIN]
item	= VIDEO,VIDEOBASE
item    = PICTURE,SCROLLPICT
item	= 100,pc,BUTTON,EXIT_BTN
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("MAIN"), "item"), 3);
        assert_eq!(
            result.get_string(Some("MAIN"), "item"),
            Some(r"VIDEO,VIDEOBASE")
        );
    }

    #[test]
    fn multiple_values_test() {
        let mut data = r"
[MAIN]
item	= VIDEO,VIDEOBASE
item    = PICTURE,SCROLLPICT
item	= 100,pc,BUTTON,EXIT_BTN
        "
        .as_bytes();

        let result = IniData::parse(&mut data, mock_path());
        assert!(result.is_ok());

        let result = result.unwrap();
        assert_eq!(result.sections.len(), 1);
        assert_eq!(result.get_amount_of_values(Some("MAIN"), "item"), 3);

        let expected = vec![
            "VIDEO,VIDEOBASE".to_string(),
            "PICTURE,SCROLLPICT".to_string(),
            "100,pc,BUTTON,EXIT_BTN".to_string(),
        ];
        assert_eq!(
            result.get_multiple_strings(Some("MAIN"), "item"),
            Some(&expected)
        );
    }
}
