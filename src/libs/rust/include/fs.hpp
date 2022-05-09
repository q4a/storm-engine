#pragma once

#include "rust_rs.h"
#include <filesystem>
#include <string>

namespace rust::fs
{

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";

static std::filesystem::path GetStashPath()
{
    const auto r_str = ffi_home_directory();
    const std::filesystem::path c_str = r_str->ptr;
    ffi_free_wchar_array(r_str);
    return c_str;
}

static std::filesystem::path GetLogsPath()
{
    const auto r_str = ffi_logs_directory();
    const std::filesystem::path c_str = r_str->ptr;
    ffi_free_wchar_array(r_str);
    return c_str;
}

static std::filesystem::path GetSaveDataPath()
{
    const auto r_str = ffi_save_directory();
    const std::filesystem::path c_str = r_str->ptr;
    ffi_free_wchar_array(r_str);
    return c_str;
}

static std::filesystem::path GetScreenshotsPath()
{
    const auto r_str = ffi_screenshot_directory();
    const std::filesystem::path c_str = r_str->ptr;
    ffi_free_wchar_array(r_str);
    return c_str;
}

static std::string GetScreenshotFilename()
{
    const auto r_str = ffi_screenshot_filename();
    const std::string c_str = r_str->ptr;
    ffi_free_cchar_array(r_str);
    return c_str;
}

static std::filesystem::path GetExecutableDirectory()
{
    const auto r_str = ffi_executable_directory();
    if (r_str == nullptr)
        throw std::runtime_error("Couldn't get exec directory");
    const std::filesystem::path c_str = r_str->ptr;
    ffi_free_wchar_array(r_str);
    return c_str;
}

static uint64_t GetFileSize(const char *path)
{
    return ffi_file_size(path);
}

static void DeleteDirectory(const char *path)
{
    ffi_delete_directory(path);
}

static bool DeleteFile(const char *path)
{
    return ffi_delete_file(path);
}

static bool CreateDirectory(const char *path)
{
    return ffi_create_directory(path);
}

static bool PathExists(const char *path)
{
    return ffi_path_exists(path);
}

static char *ReadCharsFromFile(const char *path, uint64_t &file_size)
{
    file_size = 0;
    char *result = nullptr;

    const auto r_str = ffi_read_file_as_string(path);
    if (r_str != nullptr)
    {
        file_size = r_str->len;
        result = new char[file_size];
        memcpy(result, r_str->ptr, file_size);
        ffi_free_cchar_array(r_str);
    }

    return result;
}

static std::string ReadFileToString(const char *path)
{
    std::string result;

    const auto r_str = ffi_read_file_as_string(path);
    if (r_str != nullptr)
    {
        result = r_str->ptr;
        ffi_free_cchar_array(r_str);
    }

    return result;
}

static uint8_t *ReadBytesFromFile(const char *path, uint64_t &file_size)
{
    file_size = 0;
    uint8_t *result = nullptr;

    const auto r_bytes = ffi_read_file_as_bytes(path);
    if (r_bytes != nullptr)
    {
        file_size = r_bytes->len;
        result = new uint8_t[file_size];
        memcpy(result, r_bytes->ptr, file_size);
        ffi_free_u8_array(r_bytes);
    }

    return result;
}
}