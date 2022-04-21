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

    // https://stackoverflow.com/a/49812356
    static const std::string format_message(const char *const format, va_list vaArgs)
    {

        // reliably acquire the size from a copy of
        // the variable argument array
        // and a functionally reliable call
        // to mock the formatting
        va_list vaCopy;
        va_copy(vaCopy, vaArgs);
        const int iLen = std::vsnprintf(NULL, 0, format, vaCopy);
        va_end(vaCopy);

        // return a formatted string without
        // risking memory mismanagement
        // and without assuming any compiler
        // or platform specific behavior
        std::vector<char> zc(iLen + 1);
        std::vsnprintf(zc.data(), zc.size(), format, vaArgs);
        return std::string(zc.data(), zc.size());
    }
};
} // namespace storm