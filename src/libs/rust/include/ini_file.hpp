#pragma once

#include "rust_rs.h"
#include <string>
#include <vector>

namespace rust::ini
{
class IniFile
{
  public:
    IniFile();
    ~IniFile();
    bool Load(std::string path);
    bool ReadString(const char *section, const char *key, char *buffer, size_t buffer_size);
    bool ReadString(const char *section, const char *key, char *buffer, size_t buffer_size, const char *default_value);
    size_t AmountOfValues(const char *section, const char *key);
    std::vector<std::string> ReadMultipleStrings(const char *section, const char *key);
    int32_t GetInt(const char *section, const char *key, int32_t default_value);
    double GetDouble(const char *section, const char *key, double default_value);
    float GetFloat(const char *section, const char *key, float default_value);

  private:
    void Unload();
    IniData *iniData;
    std::string path;
};
}