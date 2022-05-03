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

bool IniFile::ReadString(const char *section, const char *key, char *buffer, size_t buffer_size)
{
    return ReadString(section, key, buffer, buffer_size, nullptr);
}

bool IniFile::ReadString(const char *section, const char *key, char *buffer, size_t buffer_size, const char *default_value)
{
    if (ffi_get_string(iniData, section, key, buffer, buffer_size))
    {
        return true;
    }

    if (default_value == nullptr)
    {
        throw std::runtime_error("no key value");
    }
    strcpy_s(buffer, buffer_size, default_value);
    return false;
}

size_t IniFile::AmountOfValues(const char *section, const char *key)
{
    const auto value = ffi_get_amount_of_values(iniData, section, key);
    return value;
}

std::vector<std::string> IniFile::ReadMultipleStrings(const char *section, const char *key)
{
    std::vector<std::string> values;
    const auto ffi_values = ffi_get_multiple_strings(iniData, section, key);
    if (ffi_values == nullptr)
    {
        return values;
    }

    for (int i = 0; i < ffi_values->len; i++)
    {
        values.push_back(ffi_values->ptr[i].ptr);
    }

    ffi_free_array_of_cchar_arrays(ffi_values);
    return values;
}

int32_t IniFile::GetInt(const char *section, const char *key, int32_t default_value)
{
    return ffi_get_int(iniData, section, key, default_value);
}

double IniFile::GetDouble(const char *section, const char *key, double default_value)
{
    return ffi_get_double(iniData, section, key, default_value);
}

float IniFile::GetFloat(const char *section, const char *key, float default_value)
{
    return ffi_get_float(iniData, section, key, default_value);
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