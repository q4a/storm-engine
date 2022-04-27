#pragma once

#include <filesystem>

#ifdef false // _WIN32 // FIX_LINUX SHGetKnownFolderPath
#include <ShlObj.h>
#else
#include <SDL2/SDL.h>
#endif

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

inline path GetStashPath()
{
    static path path;
    if (path.empty())
    {
#ifdef false // _WIN32 // FIX_LINUX SHGetKnownFolderPath
        wchar_t *str = nullptr;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, nullptr, &str);
        path = str;
        path = path / "My Games" / "Sea Dogs";
        CoTaskMemFree(str);
#else
        char *pref_path = nullptr;
        pref_path = SDL_GetPrefPath("Akella", "Sea Dogs");
        path = pref_path;
#endif
    }
    return path;
}

inline path GetLogsPath()
{
    return GetStashPath() / "Logs";
}

inline path GetSaveDataPath()
{
    return GetStashPath() / "SaveData";
}

inline path GetScreenshotsPath()
{
    return GetStashPath() / "Screenshots";
}

inline bool starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

inline bool ends_with(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
