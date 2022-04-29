#include "logger.hpp"
#include "fmt.hpp"
#include "fs.hpp"
#include <Windows.h>

namespace storm
{
static std::once_flag console_created;

std::shared_ptr<Logger> Logger::default_logger = nullptr;

static void create_console()
{
    std::call_once(console_created, []() { AllocConsole(); });
}

Logger::Logger(std::string name) : m_name(name)
{
}

std::shared_ptr<Logger> Logger::file_logger(const std::string &name, LogLevel level)
{
    auto const logs_path = rust::fs::GetLogsPath();
    add_file_logger(logs_path.string().data(), name.data(), level);
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

void Logger::error(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = fmt::format(format, args);
    rs_error(m_name.data(), message.data());
    va_end(args);
}

void Logger::warn(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = fmt::format(format, args);
    rs_warn(m_name.data(), message.data());
    va_end(args);
}

void Logger::info(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = fmt::format(format, args);
    rs_info(m_name.data(), message.data());
    va_end(args);
}

void Logger::debug(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = fmt::format(format, args);
    rs_debug(m_name.data(), message.data());
    va_end(args);
}

void Logger::trace(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = fmt::format(format, args);
    rs_trace(m_name.data(), message.data());
    va_end(args);
}
} // namespace storm