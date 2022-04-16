#pragma once

#include <algorithm>
#include <cctype>
#include <string>

#include "../util_rs.h"

namespace storm
{
namespace detail
{
struct is_iless
{
    template <typename T1, typename T2 = T1> bool operator()(const T1 &first, const T2 &second) const
    {
        return std::toupper(first) < std::toupper(second);
    }
};

struct is_iless_eq
{
    template <typename T1, typename T2 = T1> bool operator()(const T1 &first, const T2 &second) const
    {
        return std::toupper(first) <= std::toupper(second);
    }
};

} // namespace detail

inline int32_t iFind(const char *first, const char *second, const size_t start)
{
    return ignore_case_find(first, second, start);
}

inline bool iStartsWith(const char *first, const char *second)
{
    return ignore_case_starts_with(first, second);
}

template <typename Range1T, typename Range2T = Range1T> bool iEquals(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return ignore_case_equal(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> bool Equals(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return equal(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> bool iEqualsWin1251(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return ignore_case_equal_win1251(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> bool iLess(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return ignore_case_less(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> bool iLessOrEqual(const Range1T &first, const Range2T &second)
{
    const auto &first_normalized = std::is_pointer<Range1T>::value ? std::string_view(first) : first;
    const auto &second_normalized = std::is_pointer<Range1T>::value ? std::string_view(second) : second;

    return ignore_case_less_or_equal(first_normalized.data(), second_normalized.data());
}

template <typename Range1T, typename Range2T = Range1T> bool iGreater(const Range1T &first, const Range2T &second)
{
    return !iLessOrEqual(first, second);
}

template <typename Range1T, typename Range2T = Range1T> bool iGreaterOrEqual(const Range1T &first, const Range2T &second)
{
    return !iLess(first, second);
}

inline int wildicmp(const char *wild, const char *string)
{
    const char *cp = nullptr, *mp = nullptr;

    while ((*string) && (*wild != '*'))
    {
        if ((tolower(*wild) != tolower(*string)) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ((tolower(*wild) == tolower(*string)) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            if (cp != nullptr)
            {
                string = cp++;
            }
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

inline int wildicmp(const char *wild, const char8_t *string)
{
    // TODO: implement for UTF8!
    return wildicmp(wild, reinterpret_cast<const char *>(string));
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
