#pragma once

#include <cstdarg>
#include <string>
#include <vector>

#include "../logger_rs.h"

namespace storm
{
class Logger
{
  public:
    static Logger file_logger(std::string name, LogLevel level);
    static Logger console_logger(std::string name, LogLevel level);

    void log_error(std::string message) const;
    void log_warn(std::string message) const;
    void log_info(std::string message) const;
    void log_debug(std::string message) const;
    void log_trace(std::string message) const;

  private:
    Logger(std::string name);
    std::string m_name;
};
} // namespace storm