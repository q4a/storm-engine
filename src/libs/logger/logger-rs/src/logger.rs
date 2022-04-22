use std::{collections::HashMap, path::PathBuf, sync::Mutex};

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
    config::{Appender, Logger, Root},
    encode::pattern::PatternEncoder,
    Config, Handle,
};
use once_cell::sync::Lazy;

static ROOT_LOGGER_NAME: &str = "stdout";
static LOG_LINE_PATTERN: &str = "{d(%Y-%m-%d %H:%M:%S)} | {t} | {({l}):5.5} | {m}{n}";
static TRIGGER_SIZE: u64 = size_in_mb(30);
static ROLLER_COUNT: u32 = 5;
static ROLLER_BASE: u32 = 1;

static LOGGER_HANDLE: Lazy<Mutex<Option<(Handle, GlobalConfig)>>> = Lazy::new(|| Mutex::new(None));

const fn size_in_mb(mb: u16) -> u64 {
    (mb as u64) * 1024 * 1024
}

#[derive(Clone, Debug)]
enum AppenderType {
    Console,
    File {
        roller_pattern: String,
        log_path: String,
    },
}

impl AppenderType {
    fn new_file_type(path: &str, name: &str) -> AppenderType {
        let mut path = PathBuf::from(path);

        let roller_pattern = format!("{}_{{}}.gz", name);
        path.push(roller_pattern);
        let roller_pattern = path.to_str().unwrap().to_string();

        let log_path = format!("{}.log", name);
        path.pop();
        path.push(log_path);
        let log_path = path.to_str().unwrap().to_string();

        AppenderType::File {
            roller_pattern,
            log_path,
        }
    }

    fn get_appender(&self, name: &str) -> Appender {
        match self {
            AppenderType::Console => {
                let appender = ConsoleAppender::builder()
                    .encoder(Box::new(PatternEncoder::new(LOG_LINE_PATTERN)))
                    .build();
                Appender::builder().build(name, Box::new(appender))
            }
            AppenderType::File {
                roller_pattern,
                log_path,
            } => {
                let trigger = Box::new(SizeTrigger::new(TRIGGER_SIZE));
                let roller = Box::new(
                    FixedWindowRoller::builder()
                        .base(ROLLER_BASE)
                        .build(roller_pattern, ROLLER_COUNT)
                        .unwrap(),
                );

                let compound_policy = Box::new(CompoundPolicy::new(trigger, roller));
                let appender = RollingFileAppender::builder()
                    .encoder(Box::new(PatternEncoder::new(LOG_LINE_PATTERN)))
                    .build(&log_path, compound_policy)
                    .unwrap();

                Appender::builder().build(name, Box::new(appender))
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

#[derive(Clone, Debug)]
struct GlobalConfig {
    loggers: HashMap<String, LoggerConfig>,
}

impl From<GlobalConfig> for Config {
    fn from(config: GlobalConfig) -> Self {
        let root_appender = AppenderType::Console.get_appender(ROOT_LOGGER_NAME);

        let mut builder = Config::builder().appender(root_appender);
        for (name, logger) in &config.loggers {
            let appender = logger.appender_type.get_appender(&logger.appender_name);
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
            .unwrap()
    }
}

pub fn add_console_logger(name: &str, level: LevelFilter) {
    if let Ok(ref mut mutex) = LOGGER_HANDLE.lock() {
        let (handle, config) = mutex.get_or_insert_with(initial_setup);

        if config.loggers.contains_key(name) {
            return;
        }

        let appender_name = format!("{}_ap", name);
        let appender_type = AppenderType::Console;
        let logger_config = LoggerConfig {
            level,
            appender_name,
            appender_type,
        };

        config.loggers.insert(name.to_string(), logger_config);
        let config_clone = config.clone();
        handle.set_config(config_clone.into())
    }
}

pub fn add_file_logger(path: &str, name: &str, level: LevelFilter) {
    if let Ok(ref mut mutex) = LOGGER_HANDLE.lock() {
        let (handle, config) = mutex.get_or_insert_with(initial_setup);

        if config.loggers.contains_key(name) {
            return;
        }

        let appender_name = format!("{}_ap", name);
        let appender_type = AppenderType::new_file_type(path, name);
        let logger_config = LoggerConfig {
            level,
            appender_name,
            appender_type,
        };
        config.loggers.insert(name.to_string(), logger_config);

        let config_clone = config.clone();
        handle.set_config(config_clone.into())
    }
}

fn initial_setup() -> (Handle, GlobalConfig) {
    let initial_config = GlobalConfig {
        loggers: HashMap::new(),
    };

    let config_clone = initial_config.clone();
    let handle = log4rs::init_config(config_clone.into()).unwrap();
    (handle, initial_config)
}

#[cfg(test)]
mod tests {
    use super::add_console_logger;

    #[test]
    fn recreate_logger_test() {
        add_console_logger("test", log::LevelFilter::Trace);
        add_console_logger("test", log::LevelFilter::Trace);
    }
}
