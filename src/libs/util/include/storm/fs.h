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
    auto const r_path = get_stash_path();
    const path c_path = r_path->ptr;
    free_array_wchar(r_path);
    return c_path;
}

inline path GetLogsPath()
{
    auto const r_path = get_logs_path();
    const path c_path = r_path->ptr;
    free_array_wchar(r_path);
    return c_path;
}

inline path GetSaveDataPath()
{
    auto const r_path = get_save_data_path();
    const path c_path = r_path->ptr;
    free_array_wchar(r_path);
    return c_path;
}

inline path GetScreenshotsPath()
{
    auto const r_path = get_screenshots_path();
    const path c_path = r_path->ptr;
    free_array_wchar(r_path);
    return c_path;
}

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
