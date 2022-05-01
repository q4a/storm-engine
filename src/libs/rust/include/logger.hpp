#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "rust_rs.h"
#include "storm/fmt.hpp"

namespace rust::log
{
void error(const char *const format, ...);
void warn(const char *const format, ...);
void info(const char *const format, ...);
void debug(const char *const format, ...);
void trace(const char *const format, ...);

class Logger
{
  public:
    static std::shared_ptr<Logger> file_logger(const std::string &name, LogLevel level);
    static std::shared_ptr<Logger> console_logger(const std::string &name, LogLevel level);

    void error(const char *const format, ...) const;
    void warn(const char *const format, ...) const;
    void info(const char *const format, ...) const;
    void debug(const char *const format, ...) const;
    void trace(const char *const format, ...) const;

  private:
    Logger(std::string name);
    std::string m_name;
};
} // namespace storm