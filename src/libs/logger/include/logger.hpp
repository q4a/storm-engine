#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "logger_rs.h"

namespace storm
{
class Logger
{
  public:
    static std::shared_ptr<Logger> default_logger;

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