#ifndef rust_h
#define rust_h

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct ArrayWchar {
  wchar_t *ptr;
  size_t len;
  size_t capacity;
};

struct ArrayCchar {
  char *ptr;
  size_t len;
  size_t capacity;
};

extern "C" {

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void free_array_wchar(ArrayWchar *ptr);

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void free_array_cchar(ArrayCchar *ptr);

ArrayWchar *ffi_home_directory();

ArrayWchar *ffi_logs_directory();

ArrayWchar *ffi_save_directory();

ArrayWchar *ffi_screenshot_directory();

ArrayCchar *ffi_screenshot_filename();

int ffi_find_ignore_case(const char *s, const char *pattern, size_t start);

bool ffi_starts_with_ignore_case(const char *s, const char *pattern);

bool ffi_equal_ignore_case(const char *first, const char *second);

bool ffi_win1251_equal_ignore_case(const char *first, const char *second);

bool ffi_equal(const char *first, const char *second);

bool ffi_less_ignore_case(const char *first, const char *second);

bool ffi_less_or_equal_ignore_case(const char *first, const char *second);

bool ffi_glob_ignore_case(const char *s, const char *pattern);

} // extern "C"

#endif // rust_h
