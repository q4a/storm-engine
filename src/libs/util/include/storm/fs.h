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
    return get_stash_path();
}

inline path GetLogsPath()
{
    return get_logs_path();
}

inline path GetSaveDataPath()
{
    return get_save_data_path();
}

inline path GetScreenshotsPath()
{
    return get_screenshots_path();
}

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
