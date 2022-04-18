#ifndef rust_logger_h
#define rust_logger_h

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

extern "C" {

void add_console_logger(const char *name, LogLevel level);

void add_file_logger(const char *path, const char *name, LogLevel level);

void error(const char *logger, const char *message);

void warn(const char *logger, const char *message);

void info(const char *logger, const char *message);

void debug(const char *logger, const char *message);

void trace(const char *logger, const char *message);

} // extern "C"

#endif // rust_logger_h
