#include "IniFile.hpp"

namespace storm::ini
{
IniFile::IniFile()
{
}

IniFile::~IniFile()
{
    Unload();
}

bool IniFile::Load(const char *logger, std::string path)
{
    Unload();
    iniData = load_ini_data(logger, path.data());
    if (iniData != nullptr)
    {
        this->path = path;
        return true;
    }
    return false;
}

void IniFile::Unload()
{
    if (iniData != nullptr)
    {
        free_ini_data(iniData);
        iniData = nullptr;
    }
    if (!path.empty())
    {
        path.clear();
    }
}
}