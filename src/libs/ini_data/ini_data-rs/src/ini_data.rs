use configparser::ini::Ini;

pub struct IniData {
    logger_name: String,
    data: Ini
}

impl IniData {
    pub fn new(logger_name: &str, path: &str) -> Option<IniData> {
        let mut data = Ini::new();
        let _ = match data.load(path) {
            Ok(_) => { },
            Err(e) => {
                log::error!(target: &logger_name, "Couldn't load ini file <{}>, reason: {}", path, e);
                return None;
            },
        };

        Some(IniData {
            logger_name: logger_name.to_string(),
            data
        })
    }
}