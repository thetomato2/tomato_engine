#ifndef TOMATO_TIME_HPP_
#define TOMATO_TIME_HPP_

#include "core.hpp"

namespace tom
{

enum
{
    cycle_counter_total,
    cycle_counter_update,
    cycle_counter_cnt
};

struct cycle_counter
{
    u64 cycle_cnt;
};

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

#define BEGIN_TIMED_BLOCK(ID) u64 start_cycle_cnt_##ID = __rdtsc();
#define END_TIMED_BLOCK(ID)   state->counters[cycle_counter_##ID].cycle_cnt += __rdtsc() - start_cycle_cnt_##ID;

}  // namespace tom

#endif  // !TOMATO_TIME_HPP