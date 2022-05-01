#ifndef rust_h
#define rust_h

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

enum class LogLevel {
  Error,
  Warn,
  Info,
  Debug,
  Trace,
};

struct IniData;

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

IniData *load_ini_data(const char *path);

void free_ini_data(IniData *ptr);

void ffi_new_file_logger(const char *name, LogLevel level);

void ffi_new_console_logger(const char *name, LogLevel level);

void ffi_error(const char *logger, const char *message);

void ffi_error_default(const char *message);

void ffi_warn(const char *logger, const char *message);

void ffi_warn_default(const char *message);

void ffi_info(const char *logger, const char *message);

void ffi_info_default(const char *message);

void ffi_debug(const char *logger, const char *message);

void ffi_debug_default(const char *message);

void ffi_trace(const char *logger, const char *message);

void ffi_trace_default(const char *message);

int ffi_find_ignore_case(const char *s, const char *pattern, size_t start);

bool ffi_starts_with_ignore_case(const char *s, const char *pattern);

bool ffi_ends_with_ignore_case(const char *s, const char *pattern);

bool ffi_equal_ignore_case(const char *first, const char *second);

bool ffi_win1251_equal_ignore_case(const char *first, const char *second);

bool ffi_equal(const char *first, const char *second);

bool ffi_less_ignore_case(const char *first, const char *second);

bool ffi_less_or_equal_ignore_case(const char *first, const char *second);

bool ffi_glob_ignore_case(const char *s, const char *pattern);

} // extern "C"

#endif // rust_h
