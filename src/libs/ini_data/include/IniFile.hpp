#pragma once

#include "ini_data_rs.h"
#include <string>

namespace storm::ini
{
class IniFile
{
  public:
    IniFile();
    ~IniFile();
    bool Load(const char *logger, std::string path);

  private:
    void Unload();
    IniData *iniData;
    std::string path;
};
}