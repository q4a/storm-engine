#ifndef rust_util_h
#define rust_util_h

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

extern "C" {

wchar_t *get_stash_path();

wchar_t *get_logs_path();

wchar_t *get_save_data_path();

wchar_t *get_screenshots_path();

int ignore_case_find(const char *s, const char *pattern, size_t start);

bool ignore_case_starts_with(const char *s, const char *pattern);

bool ignore_case_equal(const char *first, const char *second);

bool ignore_case_equal_win1251(const char *first, const char *second);

bool equal(const char *first, const char *second);

bool ignore_case_less(const char *first, const char *second);

bool ignore_case_less_or_equal(const char *first, const char *second);

bool ignore_case_glob(const char *s, const char *pattern);

} // extern "C"

#endif // rust_util_h
