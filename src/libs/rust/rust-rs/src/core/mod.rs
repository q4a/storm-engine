#[repr(C)]
#[derive(PartialEq)]
pub enum EngineVersion {
    Unknown,
    SeaDogs,
    // 1.0 - Will likely never be supported
    PiratesOfTheCaribbean,
    // 2.0
    CaribbeanTales,
    // 2.5
    CityOfAbandonedShips,
    // 2.8
    ToEachHisOwn,
    // Current, modified version
    Latest,
}

pub fn get_target_engine_version(version: &str) -> EngineVersion {
    match version {
        "sd" => EngineVersion::SeaDogs,
        "potc" => EngineVersion::PiratesOfTheCaribbean,
        "ct" => EngineVersion::CaribbeanTales,
        "coas" => EngineVersion::CityOfAbandonedShips,
        "teho" => EngineVersion::ToEachHisOwn,
        "latest" => EngineVersion::Latest,
        _ => EngineVersion::Unknown,
    }
}

mod export {
    use std::os::raw::c_char;

    use crate::common::ffi::c_char_to_str;

    use super::{get_target_engine_version, EngineVersion};

    #[no_mangle]
    pub unsafe extern "C" fn ffi_get_target_engine_version(
        version: *const c_char,
    ) -> EngineVersion {
        let version = c_char_to_str(version);
        get_target_engine_version(version)
    }
}
