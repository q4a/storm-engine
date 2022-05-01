#pragma once

#include "rust_rs.h"
#include <string>

namespace rust::ini
{
class IniFile
{
  public:
    IniFile();
    ~IniFile();
    bool Load(std::string path);

  private:
    void Unload();
    IniData *iniData;
    std::string path;
};
}