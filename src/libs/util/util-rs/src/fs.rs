use std::path::PathBuf;

pub fn home_directory() -> PathBuf {
    dirs_next::document_dir().unwrap_or_else(|| PathBuf::from("user_data"))
}

pub fn logs_directory() -> PathBuf {
    home_directory().join("Logs")
}

pub fn save_directory() -> PathBuf {
    home_directory().join("SaveData")
}

pub fn screenshot_directory() -> PathBuf {
    home_directory().join("Screenshots")
}
