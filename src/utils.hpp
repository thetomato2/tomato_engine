#ifndef TOMATO_UTILS_HPP_
#define TOMATO_UTILS_HPP_

#include "core.hpp"

namespace tom
{

namespace util
{
inline bool str_equal(const char *a, const char *b)
{
    while (*a && *a == *b) {
        ++a;
        ++b;
    }
    if (*a != *b) return false;
    return true;
}
}  // namespace util

// Convert a wide Unicode string to an UTF8 string
#if 0
inline szt get_wstr_sz(const wchar *wstr)
{
    auto len  = get_wstr_len(wstr);
    auto size = (szt)WideCharToMultiByte(CP_UTF8, 0, wstr, (s32)len, NULL, 0, NULL, NULL);
    return size;
}

inline szt get_wstr_sz(const char *str)
{
    auto len  = get_str_len(str);
    auto size = (szt)MultiByteToWideChar(CP_UTF8, 0, str, (s32)len, NULL, 0);
    return size;
}

inline void wstr_to_str(const wchar *wstr, char *buf)
{
    auto len  = get_wstr_len(wstr);
    auto size = (szt)WideCharToMultiByte(CP_UTF8, 0, wstr, (s32)len, buf, 0, NULL, NULL);
}

inline void str_to_wstr(const char *str, wchar *buf)
{
    auto len  = get_str_len(str);
    auto size = (szt)MultiByteToWideChar(CP_UTF8, 0, str, (s32)len, NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, (s32)len, buf, (s32)size);
}
#endif

}  // namespace tom
#endif  //  TOMATO_UTILS_HPP_
