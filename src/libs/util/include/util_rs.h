#ifndef rust_util_h
#define rust_util_h

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

extern "C" {

void init_logger();

void error(const char *message);

void warn(const char *message);

void info(const char *message);

void debug(const char *message);

wchar_t *get_stash_path();

wchar_t *get_logs_path();

wchar_t *get_save_data_path();

wchar_t *get_screenshots_path();

int ignore_case_find(const char *first, const char *second, size_t start);

} // extern "C"

#endif // rust_util_h
