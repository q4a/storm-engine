#pragma once

#include <filesystem>
#include <SDL2/SDL.h>
#ifdef _WIN32
#include <ShlObj.h>
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
#ifdef _WIN32
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

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
