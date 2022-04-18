use std::sync::Mutex;

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
static LOG_LINE_PATTERN: &str = "{d(%Y-%m-%d %H:%M:%S)} | {({l}):5.5} | {f}:{L} — {m}{n}";
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
        let roller_pattern = format!("{}/{}_{{}}.gz", path, name);
        let log_path = format!("{}/{}.log", path, name);
        AppenderType::File {
            roller_pattern,
            log_path,
        }
    }

    fn get_appender(&self, name: &str) -> Appender {
        match self {
            AppenderType::Console => {
                let appender = ConsoleAppender::builder().build();
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
    name: String,
    level: LevelFilter,
    appender_name: String,
    appender_type: AppenderType,
}

#[derive(Clone, Debug)]
struct GlobalConfig {
    loggers: Vec<LoggerConfig>,
}

impl From<GlobalConfig> for Config {
    fn from(config: GlobalConfig) -> Self {
        let root_appender = AppenderType::Console.get_appender(ROOT_LOGGER_NAME);

        let mut builder = Config::builder().appender(root_appender);
        for logger in &config.loggers {
            let appender = logger.appender_type.get_appender(&logger.appender_name);
            builder = builder.appender(appender).logger(
                Logger::builder()
                    .appender(&logger.appender_name)
                    .additive(false)
                    .build(&logger.name, logger.level)
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

        let appender_name = format!("{}_ap", name);
        let appender_type = AppenderType::Console;
        config.loggers.push(LoggerConfig {
            name: name.to_string(),
            level,
            appender_name,
            appender_type,
        });

        let config_clone = config.clone();
        handle.set_config(config_clone.into())
    }
}

pub fn add_file_logger(path: &str, name: &str, level: LevelFilter) {
    if let Ok(ref mut mutex) = LOGGER_HANDLE.lock() {
        let (handle, config) = mutex.get_or_insert_with(initial_setup);

        let appender_name = format!("{}_ap", name);
        let appender_type = AppenderType::new_file_type(path, name);
        config.loggers.push(LoggerConfig {
            name: name.to_string(),
            level,
            appender_name,
            appender_type,
        });

        let config_clone = config.clone();
        handle.set_config(config_clone.into())
    }
}

fn initial_setup() -> (Handle, GlobalConfig) {
    let initial_config = GlobalConfig {
        loggers: Vec::new(),
    };

    let config_clone = initial_config.clone();
    let handle = log4rs::init_config(config_clone.into()).unwrap();
    (handle, initial_config)
}
