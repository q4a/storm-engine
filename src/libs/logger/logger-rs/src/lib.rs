use std::{ffi::CStr, os::raw::c_char};

use log::LevelFilter;

mod logger;

#[macro_use]
extern crate log;

#[repr(C)]
pub enum LogLevel {
    Error,
    Warn,
    Info,
    Debug,
    Trace,
}

impl From<LogLevel> for LevelFilter {
    fn from(level: LogLevel) -> Self {
        match level {
            LogLevel::Error => LevelFilter::Error,
            LogLevel::Warn => LevelFilter::Warn,
            LogLevel::Info => LevelFilter::Info,
            LogLevel::Debug => LevelFilter::Debug,
            LogLevel::Trace => LevelFilter::Trace,
        }
    }
}

#[no_mangle]
pub extern "C" fn add_console_logger(name: *const c_char, level: LogLevel) {
    let name = c_char_to_str(name);
    logger::add_console_logger(name, level.into())
}

#[no_mangle]
pub extern "C" fn add_file_logger(path: *const c_char, name: *const c_char, level: LogLevel) {
    let path = c_char_to_str(path);
    let name = c_char_to_str(name);
    logger::add_file_logger(path, name, level.into());
}

#[no_mangle]
pub extern "C" fn error(logger: *const c_char, message: *const c_char) {
    let logger = c_char_to_str(logger);
    let message = c_char_to_str(message);
    error!(target: logger, "{}", message);
}

#[no_mangle]
pub extern "C" fn warn(logger: *const c_char, message: *const c_char) {
    let logger = c_char_to_str(logger);
    let message = c_char_to_str(message);
    warn!(target: logger, "{}", message);
}

#[no_mangle]
pub extern "C" fn info(logger: *const c_char, message: *const c_char) {
    let logger = c_char_to_str(logger);
    let message = c_char_to_str(message);
    info!(target: logger, "{}", message);
}

#[no_mangle]
pub extern "C" fn debug(logger: *const c_char, message: *const c_char) {
    let logger = c_char_to_str(logger);
    let message = c_char_to_str(message);
    debug!(target: logger, "{}", message);
}

#[no_mangle]
pub extern "C" fn trace(logger: *const c_char, message: *const c_char) {
    let logger = c_char_to_str(logger);
    let message = c_char_to_str(message);
    trace!(target: logger, "{}", message);
}

fn c_char_to_str<'a>(s: *const c_char) -> &'a str {
    let s_str = unsafe { CStr::from_ptr(s) };
    s_str.to_str().unwrap()
}
