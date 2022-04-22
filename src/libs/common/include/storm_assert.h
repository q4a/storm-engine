#pragma once
#include <spdlog/spdlog.h>
#include "logger.hpp"

#define EX_OFF

inline void __Storm_Assert__(bool expression, const char *file, int32_t line, const char *str)
{
    if (!expression)
    {
        if (str)
        {
            storm::Logger::default_logger->error("Assert failed in %s line %d, expression string %s", file, line, str);
        }
        else
        {
            storm::Logger::default_logger->error("Assert failed in %s line %d", file, line);
        }
#ifdef EX_OFF
        __debugbreak();
#else
        throw std::runtime_error("assert");
#endif
    }
}

#ifdef _DEBUG
#define Assert(exp) __Storm_Assert__(exp, __FILE__, __LINE__, #exp)
#else
#define Assert(exp)
#endif
