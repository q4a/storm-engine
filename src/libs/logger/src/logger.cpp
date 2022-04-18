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

Logger Logger::file_logger(std::string name, LogLevel level)
{
    auto logs_path = fs::GetLogsPath();
    add_file_logger(logs_path.string().data(), name.data(), level);
    return Logger(name);
}

Logger::Logger(std::string name) : m_name(name)
{
}

Logger Logger::console_logger(std::string name, LogLevel level)
{
    create_console();
    add_console_logger(name.data(), level);
    return Logger(name);
}

void Logger::log_error(std::string message) const
{
    error(m_name.data(), message.data());
}

void Logger::log_warn(std::string message) const
{
    warn(m_name.data(), message.data());
}

void Logger::log_info(std::string message) const
{
    info(m_name.data(), message.data());
}

void Logger::log_debug(std::string message) const
{
    debug(m_name.data(), message.data());
}

void Logger::log_trace(std::string message) const
{
    trace(m_name.data(), message.data());
}

// https://stackoverflow.com/a/49812356
const std::string format(const char *const format, ...)
{
    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, format);

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
    va_end(vaArgs);
    return std::string(zc.data(), zc.size());
}

} // namespace storm