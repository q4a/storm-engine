#include "logger.hpp"
#include "storm/fs.h"
#include <mutex>

namespace storm
{

std::once_flag console_created;

void create_console()
{
    std::call_once(console_created, []() { 
        AllocConsole();
    });
}

// https://stackoverflow.com/a/49812356
static std::string format_message(const char *const format, va_list vaArgs)
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

Logger::Logger(std::string name) : m_name(name)
{
}

std::shared_ptr<Logger> Logger::file_logger(const std::string &name, LogLevel level)
{
    auto const logs_path = fs::GetLogsPath();
    add_file_logger(logs_path.string().c_str(), name.data(), level);
    auto const logger = Logger(name);
    return std::make_shared<Logger>(logger);
}

std::shared_ptr<Logger> Logger::console_logger(const std::string &name, LogLevel level)
{
    create_console();
    add_console_logger(name.data(), level);
    auto const logger = Logger(name);
    return std::make_shared<Logger>(logger);
}

void Logger::log_error(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = format_message(format, args);
    error(m_name.data(), message.data());
    va_end(args);
}

void Logger::log_warn(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = format_message(format, args);
    warn(m_name.data(), message.data());
    va_end(args);
}

void Logger::log_info(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = format_message(format, args);
    info(m_name.data(), message.data());
    va_end(args);
}

void Logger::log_debug(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = format_message(format, args);
    debug(m_name.data(), message.data());
    va_end(args);
}

void Logger::log_trace(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = format_message(format, args);
    trace(m_name.data(), message.data());
    va_end(args);
}

} // namespace storm