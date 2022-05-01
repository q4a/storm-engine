#include "ini_file.hpp"

namespace storm::ini
{
IniFile::IniFile()
{
}

IniFile::~IniFile()
{
    Unload();
}

bool IniFile::Load(std::string path)
{
    Unload();
    iniData = load_ini_data(path.data());
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