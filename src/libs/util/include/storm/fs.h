#pragma once

#include <filesystem>

#include <ShlObj.h>

#include "../util_rs.h"

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

inline path GetStashPath()
{
    const auto r_str = get_stash_path();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

inline path GetLogsPath()
{
    const auto r_str = get_logs_path();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

inline path GetSaveDataPath()
{
    const auto r_str = get_save_data_path();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

inline path GetScreenshotsPath()
{
    const auto r_str = get_screenshots_path();
    const path c_str = r_str->ptr;
    free_array_wchar(r_str);
    return c_str;
}

inline std::string GetScreenshotFilename()
{
    const auto r_str = get_screenshot_filename();
    const std::string c_str = r_str->ptr;
    free_array_cchar(r_str);
    return c_str;
}

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
