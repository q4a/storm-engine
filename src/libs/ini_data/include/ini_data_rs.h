#ifndef rust_ini_data_h
#define rust_ini_data_h

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct IniData;

extern "C" {

IniData *load_ini_data(const char *logger_name, const char *path);

void free_ini_data(IniData *ptr);

} // extern "C"

#endif // rust_ini_data_h
