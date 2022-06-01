#ifndef TOMATO_TIME_HPP_
#define TOMATO_TIME_HPP_

#include "core.hpp"

namespace tom
{

inline LARGE_INTEGER get_time()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time;
}

inline f32 get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end,
                               s64 performance_counter_frequency)
{
    return scast(f32, end.QuadPart - start.QuadPart) / scast(f32, performance_counter_frequency);
}

}  // namespace tom

#endif  // !TOMATO_TIME_HPP