#pragma once

#include <cstdarg>
#include <string>
#include <vector>

namespace fmt
{
// https://stackoverflow.com/a/49812356
static const std::string format(const char *const format, va_list vaArgs)
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

static const std::string format(const std::string format, ...)
{
    const char *const zcFormat = format.c_str();

    // initialize use of the variable argument array
    va_list args;
    va_start(args, format);
    auto const formatted = fmt::format(zcFormat, args);
    va_end(args);
    return formatted;
}
}