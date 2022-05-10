use once_cell::sync::Lazy;
use std::{collections::HashMap, path::PathBuf, sync::Mutex};
use thiserror::Error;

use log::LevelFilter;
use log4rs::{
    append::{
        console::ConsoleAppender,
        rolling_file::{
            policy::compound::{
                roll::fixed_window::FixedWindowRoller, trigger::size::SizeTrigger, CompoundPolicy,
            },
            RollingFileAppender,
        },
    },
    config::{runtime::ConfigErrors, Appender, Logger, Root},
    encode::pattern::PatternEncoder,
    Config, Handle,
};

use crate::{common::DEFAULT_LOGGER, fs};

static ROOT_LOGGER_NAME: &str = "stdout";
static LOG_LINE_PATTERN: &str = "{d(%Y-%m-%d %H:%M:%S)} | {t} | {({l}):5.5} | {m}{n}";
static TRIGGER_SIZE: u64 = size_in_mb(30);
static ROLLER_COUNT: u32 = 5;
static ROLLER_BASE: u32 = 1;

static LOGGER_HANDLE: Lazy<Mutex<(Handle, GlobalConfig)>> =
    Lazy::new(|| init_default_logger(LevelFilter::Trace).unwrap());

const fn size_in_mb(mb: u16) -> u64 {
    (mb as u64) * 1024 * 1024
}

#[derive(Error, Debug)]
pub enum LoggerError {
    #[error("Couldn't convert PathBuf to UTF-8 string: {0}")]
    PathBufConversion(PathBuf),
    #[error("Couldn't create fixed window roller: {0}")]
    FixedWindowRoller(#[from] Box<dyn std::error::Error>),
    #[error("Couldn't create appender: {0}")]
    CreateAppender(#[from] std::io::Error),
    #[error("Couldn't create logger config: {0}")]
    CreateConfig(#[from] ConfigErrors),
    #[error("Couldn't initialize config: {0}")]
    InitConfig(#[from] log::SetLoggerError),
    #[error("Couldn't acquire mutex")]
    MutexLock,
}

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

#[derive(Clone, Debug)]
enum AppenderType {
    Console,
    File {
        roller_pattern: String,
        file_path: String,
    },
}

impl AppenderType {
    fn get_appender(&self, name: &str) -> Result<Appender, LoggerError> {
        match self {
            AppenderType::Console => {
                let appender = ConsoleAppender::builder()
                    .encoder(Box::new(PatternEncoder::new(LOG_LINE_PATTERN)))
                    .build();
                Ok(Appender::builder().build(name, Box::new(appender)))
            }
            AppenderType::File {
                roller_pattern,
                file_path,
            } => {
                let trigger = Box::new(SizeTrigger::new(TRIGGER_SIZE));
                let roller = Box::new(
                    FixedWindowRoller::builder()
                        .base(ROLLER_BASE)
                        .build(roller_pattern, ROLLER_COUNT)
                        .map_err(|e| LoggerError::FixedWindowRoller(e.into()))?,
                );

                let compound_policy = Box::new(CompoundPolicy::new(trigger, roller));
                let appender = RollingFileAppender::builder()
                    .encoder(Box::new(PatternEncoder::new(LOG_LINE_PATTERN)))
                    .build(&file_path, compound_policy)?;

                Ok(Appender::builder().build(name, Box::new(appender)))
            }
        }
    }
}

#[derive(Clone, Debug)]
struct LoggerConfig {
    level: LevelFilter,
    appender_name: String,
    appender_type: AppenderType,
}

impl LoggerConfig {
    fn file_config(name: &str, level: LevelFilter) -> Result<LoggerConfig, LoggerError> {
        let roller_pattern = format!("{}_{{}}.gz", name);

        let mut log_path = fs::dirs::logs_directory();
        log_path.push(roller_pattern);
        let roller_pattern = log_path
            .to_str()
            .ok_or_else(|| LoggerError::PathBufConversion(log_path.clone()))?
            .to_string();

        let log_file = format!("{}.log", name);
        log_path.pop();
        log_path.push(log_file);
        let file_path = log_path
            .to_str()
            .ok_or_else(|| LoggerError::PathBufConversion(log_path.clone()))?
            .to_string();

        let appender_type = AppenderType::File {
            roller_pattern,
            file_path,
        };

        let appender_name = format!("{}_ap", name);
        Ok(LoggerConfig {
            level,
            appender_name,
            appender_type,
        })
    }

    fn console_config(name: &str, level: LevelFilter) -> LoggerConfig {
        let appender_type = AppenderType::Console;
        let appender_name = format!("{}_ap", name);
        LoggerConfig {
            level,
            appender_name,
            appender_type,
        }
    }
}

#[derive(Clone, Debug)]
struct GlobalConfig {
    loggers: HashMap<String, LoggerConfig>,
}

impl TryFrom<GlobalConfig> for Config {
    type Error = LoggerError;

    fn try_from(config: GlobalConfig) -> Result<Self, Self::Error> {
        let root_appender = AppenderType::Console.get_appender(ROOT_LOGGER_NAME)?;

        let mut builder = Config::builder().appender(root_appender);
        for (name, logger) in &config.loggers {
            let appender = logger.appender_type.get_appender(&logger.appender_name)?;
            builder = builder.appender(appender).logger(
                Logger::builder()
                    .appender(&logger.appender_name)
                    .additive(false)
                    .build(name, logger.level),
            );
        }

        builder
            .build(
                Root::builder()
                    .appender(ROOT_LOGGER_NAME)
                    .build(LevelFilter::Trace),
            )
            .map_err(|e| e.into())
    }
}

/// Create default logger with an output to `Documents/My Games/Sea Dogs/Logs/system.log`
fn init_default_logger(level: LevelFilter) -> Result<Mutex<(Handle, GlobalConfig)>, LoggerError> {
    let logger_config = LoggerConfig::file_config(DEFAULT_LOGGER, level)?;

    let mut global_config = GlobalConfig {
        loggers: HashMap::new(),
    };
    global_config
        .loggers
        .insert(DEFAULT_LOGGER.to_string(), logger_config);

    let init_config = global_config.clone().try_into()?;
    let handle = log4rs::init_config(init_config)?;

    Ok(Mutex::new((handle, global_config)))
}

/// Create new logger from `logger_config` configuration
fn new_logger(name: &str, logger_config: LoggerConfig) -> Result<(), LoggerError> {
    match LOGGER_HANDLE.lock() {
        Ok(ref mut guard) => {
            let config = &mut guard.1;
            if config.loggers.contains_key(name) {
                return Ok(());
            }

            config.loggers.insert(name.to_string(), logger_config);
            let config_clone = config.clone().try_into()?;

            let handle = &guard.0;
            handle.set_config(config_clone);
            Ok(())
        }
        Err(_) => Err(LoggerError::MutexLock),
    }
}

/// Register new file logger with an output to `Documents/My Games/Sea Dogs/Logs/<name>.log`
pub fn new_file_logger(name: &str, level: LevelFilter) -> Result<(), LoggerError> {
    let logger_config = LoggerConfig::file_config(name, level)?;
    new_logger(name, logger_config)
}

/// Register new console logger
pub fn new_console_logger(name: &str, level: LevelFilter) -> Result<(), LoggerError> {
    let logger_config = LoggerConfig::console_config(name, level);
    new_logger(name, logger_config)
}

mod export {
    use std::os::raw::c_char;

    use log::{debug, error, info, trace, warn};

    use crate::common::{ffi::c_char_to_str, DEFAULT_LOGGER};

    use super::{new_console_logger, new_file_logger, LogLevel};

    #[no_mangle]
    pub unsafe extern "C" fn ffi_new_file_logger(name: *const c_char, level: LogLevel) {
        let name = c_char_to_str(name);
        new_file_logger(name, level.into()).unwrap();
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_new_console_logger(name: *const c_char, level: LogLevel) {
        let name = c_char_to_str(name);
        new_console_logger(name, level.into()).unwrap();
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_error(logger: *const c_char, message: *const c_char) {
        let logger = c_char_to_str(logger);
        let message = c_char_to_str(message);
        error!(target: logger, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_error_default(message: *const c_char) {
        let message = c_char_to_str(message);
        error!(target: DEFAULT_LOGGER, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_warn(logger: *const c_char, message: *const c_char) {
        let logger = c_char_to_str(logger);
        let message = c_char_to_str(message);
        warn!(target: logger, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_warn_default(message: *const c_char) {
        let message = c_char_to_str(message);
        warn!(target: DEFAULT_LOGGER, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_info(logger: *const c_char, message: *const c_char) {
        let logger = c_char_to_str(logger);
        let message = c_char_to_str(message);
        info!(target: logger, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_info_default(message: *const c_char) {
        let message = c_char_to_str(message);
        info!(target: DEFAULT_LOGGER, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_debug(logger: *const c_char, message: *const c_char) {
        let logger = c_char_to_str(logger);
        let message = c_char_to_str(message);
        debug!(target: logger, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_debug_default(message: *const c_char) {
        let message = c_char_to_str(message);
        debug!(target: DEFAULT_LOGGER, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_trace(logger: *const c_char, message: *const c_char) {
        let logger = c_char_to_str(logger);
        let message = c_char_to_str(message);
        trace!(target: logger, "{}", message);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_trace_default(message: *const c_char) {
        let message = c_char_to_str(message);
        trace!(target: DEFAULT_LOGGER, "{}", message);
    }
}

#[cfg(test)]
mod tests {
    use super::new_console_logger;

    #[test]
    #[cfg_attr(miri, ignore)]
    fn recreate_logger_test() {
        let _ = new_console_logger("test", log::LevelFilter::Trace).unwrap();
        let _ = new_console_logger("test", log::LevelFilter::Trace).unwrap();
    }
}
