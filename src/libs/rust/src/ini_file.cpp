#include "ini_file.hpp"

namespace rust::ini
{
IniFile::IniFile()
{
    iniData = nullptr;
    path.clear();
}

IniFile::~IniFile()
{
    Unload();
}

bool IniFile::Load(std::string path)
{
    Unload();
    iniData = ffi_load_ini_data(path.data());
    if (iniData != nullptr)
    {
        this->path = path;
        return true;
    }
    return false;
}

bool IniFile::ReadString(const char *section, const char *key, char *buffer, size_t buffer_size, const char *default_value)
{
    strcpy_s(buffer, buffer_size, default_value);
    if (iniData == nullptr)
    {
        return false;
    }

    const auto value = ffi_get_string(iniData, section, key);
    if (value == nullptr)
    {
        return false;
    }

    strcpy_s(buffer, buffer_size, value->ptr);
    ffi_free_cchar_array(value);
    return true;
}

std::vector<std::string> IniFile::ReadMultipleStrings(const char *section, const char *key)
{
    std::vector<std::string> values;
    if (iniData == nullptr)
    {
        return values;
    }

    const auto ffi_values = ffi_get_multiple_strings(iniData, section, key);
    for (int i = 0; i < ffi_values->len; i++)
    {
        values.push_back(ffi_values->ptr[i].ptr);
    }

    ffi_free_array_of_cchar_arrays(ffi_values);
    return values;
}

void IniFile::Unload()
{
    if (iniData != nullptr)
    {
        ffi_free_ini_data(iniData);
        iniData = nullptr;
    }
    if (!path.empty())
    {
        path.clear();
    }
}
}