#pragma once

#include <filesystem>
#include "rust_rs.h"

namespace rust::fs
{
using namespace std::filesystem;

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";

static path GetStashPath()
{
    const auto r_str = ffi_home_directory();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

static path GetLogsPath()
{
    const auto r_str = ffi_logs_directory();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

static path GetSaveDataPath()
{
    const auto r_str = ffi_save_directory();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

static path GetScreenshotsPath()
{
    const auto r_str = ffi_screenshot_directory();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

static std::string GetScreenshotFilename()
{
    const auto r_str = ffi_screenshot_filename();
    const std::string c_str = r_str->ptr;
    free_array_cchar(r_str);
    return c_str;
}
}