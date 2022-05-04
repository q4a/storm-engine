#ifndef rust_h
#define rust_h

#pragma once

/* Generated with cbindgen:0.21.0 */

#include <stdint.h>

enum class EngineVersion {
  Unknown,
  SeaDogs,
  PiratesOfTheCaribbean,
  CaribbeanTales,
  CityOfAbandonedShips,
  ToEachHisOwn,
  Latest,
};

enum class LogLevel {
  Error,
  Warn,
  Info,
  Debug,
  Trace,
};

/// Parser for ini-configurations to be used with the storm-engine.
/// Supports comments, sections and values without sections (`default` section will be used in such cases).
/// This parser also supports multiple values for the same `key` (values will be merged in a single `Vec`).
/// Comments at the end of value are NOT supported
struct IniData;

struct WCharArray {
  wchar_t *ptr;
  size_t len;
  size_t capacity;
};

struct CCharArray {
  char *ptr;
  size_t len;
  size_t capacity;
};

struct ArrayOfCCharArrays {
  CCharArray *ptr;
  size_t len;
  size_t capacity;
};

extern "C" {

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void ffi_free_wchar_array(WCharArray *ptr);

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void ffi_free_cchar_array(CCharArray *ptr);

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void ffi_free_array_of_cchar_arrays(ArrayOfCCharArrays *ptr);

EngineVersion ffi_get_target_engine_version(const char *version);

bool ffi_engine_versions_eq(EngineVersion first, EngineVersion second);

WCharArray *ffi_home_directory();

WCharArray *ffi_logs_directory();

WCharArray *ffi_save_directory();

WCharArray *ffi_screenshot_directory();

CCharArray *ffi_screenshot_filename();

WCharArray *ffi_executable_directory();

uint64_t ffi_file_size(const char *path);

void ffi_delete_directory(const char *path);

bool ffi_delete_file(const char *path);

bool ffi_create_directory(const char *path);

IniData *ffi_load_ini_data(const char *path);

void ffi_free_ini_data(IniData *ptr);

bool ffi_get_string(IniData *ptr,
                    const char *section,
                    const char *key,
                    char *buffer,
                    size_t buffer_size);

ArrayOfCCharArrays *ffi_get_multiple_strings(IniData *ptr, const char *section, const char *key);

size_t ffi_get_amount_of_values(IniData *ptr, const char *section, const char *key);

int ffi_get_int(IniData *ptr, const char *section, const char *key, int default_value);

double ffi_get_double(IniData *ptr, const char *section, const char *key, double default_value);

float ffi_get_float(IniData *ptr, const char *section, const char *key, float default_value);

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
