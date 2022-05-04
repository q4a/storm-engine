#pragma once

#include <algorithm>
#include <ctype.h>
#include <string>
#include "rust_rs.h"

namespace rust::string
{
template <typename Range1T, typename Range2T = Range1T>
int32_t iFind(const Range1T &first, const Range2T &second, const size_t start)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_find_ignore_case(first_normalized.data(), second_normalized.data(), start);
}

template <typename Range1T, typename Range2T = Range1T> 
bool iStartsWith(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_starts_with_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iEndsWith(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_ends_with_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iEquals(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_equal_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool Equals(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_equal(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iEqualsWin1251(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return ffi_win1251_equal_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iLess(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_less_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iLessOrEqual(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range2T>::value ? std::string_view(second) : second;

    return ffi_less_or_equal_ignore_case(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> 
bool iGreater(const Range1T &first, const Range2T &second)
{
    return !iLessOrEqual(first, second);
}

template <typename Range1T, typename Range2T = Range1T> 
bool iGreaterOrEqual(const Range1T &first, const Range2T &second)
{
    return !iLess(first, second);
}

inline bool wildicmp(const char *wild, const char8_t *string)
{
    return ffi_glob_ignore_case(reinterpret_cast<const char *>(string), wild);
}

class iStrHasher
{
  public:
    size_t operator()(const std::string &key) const
    {
        std::string lower_copy = key;
        std::transform(lower_copy.begin(), lower_copy.end(), lower_copy.begin(), ::tolower);
        return inner_hasher_(lower_copy);
    }

  private:
    std::hash<std::string> inner_hasher_;
};

struct iStrComparator
{
    bool operator()(const std::string &left, const std::string &right) const
    {
        return iEquals(left, right);
    }
};
} // namespace storm
