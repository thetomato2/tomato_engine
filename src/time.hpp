#ifndef TOMATO_TIME_HPP_
#define TOMATO_TIME_HPP_

#include "core.hpp"

namespace tom
{

enum
{
    cycle_counter_tom_vec,
    cycle_counter_std_vec,
    cycle_counter_std_ptr,
    cycle_counter_raw_ptr,
    cycle_counter_update,
    cycle_counter_total,
    cycle_counter_cnt
};

struct cycle_counter
{
    u64 cycle_cnt;
};

inline s64 get_time()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time.QuadPart;
}

inline f32 get_seconds_elapsed(s64 start, s64 end, s64 performance_counter_frequency)
{
    return scast(f32, end - start) / scast(f32, performance_counter_frequency);
}

#define BEGIN_TIMED_BLOCK(ID) u64 start_cycle_cnt_##ID = __rdtsc()
#define END_TIMED_BLOCK(ID) \
    state->counters[cycle_counter_##ID].cycle_cnt += __rdtsc() - start_cycle_cnt_##ID

}  // namespace tom

#endif  // !TOMATO_TIME_HPP