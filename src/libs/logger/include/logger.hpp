#pragma once

#include <string>
#include <memory>

#include "logger_rs.h"

namespace storm
{
class Logger
{
  public:
    static std::shared_ptr<Logger> file_logger(const std::string &name, LogLevel level);
    static std::shared_ptr<Logger> console_logger(const std::string &name, LogLevel level);

    void log_error(const char *const format, ...) const;
    void log_warn(const char *const format, ...) const;
    void log_info(const char *const format, ...) const;
    void log_debug(const char *const format, ...) const;
    void log_trace(const char *const format, ...) const;

  private:
    Logger(std::string name);
    std::string m_name;
};
} // namespace storm