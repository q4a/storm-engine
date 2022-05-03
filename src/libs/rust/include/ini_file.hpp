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
    bool ReadString(const char *section, const char *key, char *buffer, size_t buffer_size, const char *default_value);
    std::vector<std::string> ReadMultipleStrings(const char *section, const char *key);

  private:
    void Unload();
    IniData *iniData;
    std::string path;
};
}