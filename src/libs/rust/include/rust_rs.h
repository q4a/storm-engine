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

struct VecWrapper3D;

struct VecWrapper3F;

struct VecWrapper4F;

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

struct U8Array {
  uint8_t *ptr;
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
void ffi_free_u8_array(U8Array *ptr);

/// # Safety
///
/// This function is meant to be called from C/C++ code. As such, it can try to dereference arbitrary pointers
void ffi_free_array_of_cchar_arrays(ArrayOfCCharArrays *ptr);

VecWrapper3F *ffi_new_3f_vector(float x, float y, float z);

void ffi_free_3f_vector(VecWrapper3F *ptr);

VecWrapper3D *ffi_new_3d_vector(double x, double y, double z);

void ffi_free_3d_vector(VecWrapper3D *ptr);

VecWrapper4F *ffi_new_4f_vector(float x, float y, float z, float w);

void ffi_free_4f_vector(VecWrapper4F *ptr);

float ffi_vector3f_x(VecWrapper3F *v);

float ffi_vector3f_y(VecWrapper3F *v);

float ffi_vector3f_z(VecWrapper3F *v);

double ffi_vector3d_x(VecWrapper3D *v);

double ffi_vector3d_y(VecWrapper3D *v);

double ffi_vector3d_z(VecWrapper3D *v);

float ffi_vector4f_x(VecWrapper4F *v);

float ffi_vector4f_y(VecWrapper4F *v);

float ffi_vector4f_z(VecWrapper4F *v);

float ffi_vector4f_w(VecWrapper4F *v);

void ffi_vector3f_add_self(VecWrapper3F *v1, VecWrapper3F *v2);

void ffi_vector3f_sub_self(VecWrapper3F *v1, VecWrapper3F *v2);

void ffi_vector3f_mul_self(VecWrapper3F *v1, VecWrapper3F *v2);

void ffi_vector3f_div_self(VecWrapper3F *v1, VecWrapper3F *v2);

VecWrapper3F *ffi_vector3f_add(VecWrapper3F *v1, VecWrapper3F *v2);

VecWrapper3F *ffi_vector3f_sub(VecWrapper3F *v1, VecWrapper3F *v2);

VecWrapper3F *ffi_vector3f_mul(VecWrapper3F *v1, VecWrapper3F *v2);

VecWrapper3F *ffi_vector3f_div(VecWrapper3F *v1, VecWrapper3F *v2);

VecWrapper3F *ffi_vector3f_mul_f32(VecWrapper3F *v1, float f);

VecWrapper3F *ffi_vector3f_div_f32(VecWrapper3F *v1, float f);

float ffi_vector3f_length_sq(VecWrapper3F *v1);

float ffi_vector3f_length(VecWrapper3F *v1);

VecWrapper3F *ffi_vector3f_normal(VecWrapper3F *v1);

VecWrapper3F *ffi_vector3f_cross(VecWrapper3F *v1, VecWrapper3F *v2);

float ffi_vector3f_dot(VecWrapper3F *v1, VecWrapper3F *v2);

void ffi_vector3d_add_self(VecWrapper3D *v1, VecWrapper3D *v2);

void ffi_vector3d_sub_self(VecWrapper3D *v1, VecWrapper3D *v2);

void ffi_vector3d_mul_self(VecWrapper3D *v1, VecWrapper3D *v2);

void ffi_vector3d_div_self(VecWrapper3D *v1, VecWrapper3D *v2);

VecWrapper3D *ffi_vector3d_add(VecWrapper3D *v1, VecWrapper3D *v2);

VecWrapper3D *ffi_vector3d_sub(VecWrapper3D *v1, VecWrapper3D *v2);

VecWrapper3D *ffi_vector3d_mul(VecWrapper3D *v1, VecWrapper3D *v2);

VecWrapper3D *ffi_vector3d_div(VecWrapper3D *v1, VecWrapper3D *v2);

VecWrapper3D *ffi_vector3d_mul_f64(VecWrapper3D *v1, double f);

VecWrapper3D *ffi_vector3d_div_f64(VecWrapper3D *v1, double f);

double ffi_vector3d_length_sq(VecWrapper3D *v1);

double ffi_vector3d_length(VecWrapper3D *v1);

VecWrapper3D *ffi_vector3d_normal(VecWrapper3D *v1);

VecWrapper3D *ffi_vector3d_cross(VecWrapper3D *v1, VecWrapper3D *v2);

double ffi_vector3d_dot(VecWrapper3D *v1, VecWrapper3D *v2);

EngineVersion ffi_get_target_engine_version(const char *version);

WCharArray *ffi_home_directory();

WCharArray *ffi_logs_directory();

WCharArray *ffi_save_directory();

WCharArray *ffi_screenshot_directory();

WCharArray *ffi_executable_directory();

bool ffi_path_exists(const char *path);

bool ffi_create_directory(const char *path);

void ffi_delete_directory(const char *path);

bool ffi_delete_file(const char *path);

uint64_t ffi_file_size(const char *path);

CCharArray *ffi_screenshot_filename();

CCharArray *ffi_read_file_as_string(const char *path);

U8Array *ffi_read_file_as_bytes(const char *path);

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

uint64_t ffi_hash(const char *s);

uint64_t ffi_hash_ignore_case(const char *s);

} // extern "C"

#endif // rust_h
