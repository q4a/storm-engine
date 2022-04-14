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

bool ignore_case_equal(const char *first, const char *second);

bool ignore_case_less(const char *first, const char *second);

bool ignore_case_less_or_equal(const char *first, const char *second);

bool ignore_case_greater(const char *first, const char *second);

bool ignore_case_greater_or_equal(const char *first, const char *second);

bool ignore_case_equal_first_n(const char *first, const char *second, size_t count);

} // extern "C"
