#include "logger.hpp"
#include "storm/fmt.hpp"
#include <Windows.h>

namespace rust::log
{
static std::once_flag console_created;

static void create_console()
{
    std::call_once(console_created, []() { AllocConsole(); });
}

Logger::Logger(std::string name) : m_name(name)
{
}

std::shared_ptr<Logger> Logger::file_logger(const std::string &name, LogLevel level)
{
    ffi_new_file_logger(name.data(), level);
    auto const logger = Logger(name);
    return std::make_shared<Logger>(logger);
}

std::shared_ptr<Logger> Logger::console_logger(const std::string &name, LogLevel level)
{
    create_console();
    ffi_new_console_logger(name.data(), level);
    auto const logger = Logger(name);
    return std::make_shared<Logger>(logger);
}

void rust::log::error(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_error_default(message.data());
    va_end(args);
}

void rust::log::warn(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_warn_default(message.data());
    va_end(args);
}

void rust::log::info(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_info_default(message.data());
    va_end(args);
}

void rust::log::debug(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_debug_default(message.data());
    va_end(args);
}

void rust::log::trace(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_trace_default(message.data());
    va_end(args);
}

void Logger::error(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_error(m_name.data(), message.data());
    va_end(args);
}

void Logger::warn(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_warn(m_name.data(), message.data());
    va_end(args);
}

void Logger::info(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_info(m_name.data(), message.data());
    va_end(args);
}

void Logger::debug(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_debug(m_name.data(), message.data());
    va_end(args);
}

void Logger::trace(const char *const format, ...) const
{
    va_list args;
    va_start(args, format);
    auto const message = storm::fmt::format(format, args);
    ffi_trace(m_name.data(), message.data());
    va_end(args);
}
} // namespace storm