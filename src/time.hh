#ifndef TOMATO_TIME_HH_
#define TOMATO_TIME_HH_

#include "core.hh"
#include "string.hh"
#include "vector.hh"

namespace tom
{

struct cycle_counter
{
    enum type
    {
        temp1,
        update,
        total,
        count
    };

    type t_;
    string name;
    u64 cycle_cnt;
};

inline s64 get_time()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time.QuadPart;
}

inline constexpr const char *get_cycle_counter_name(cycle_counter::type type)
{
    switch (type) {
        case cycle_counter::type::temp1: return "temp1";
        case cycle_counter::type::update: return "update";
        case cycle_counter::type::total: return "total";
        case cycle_counter::type::count: return "count";

        default: {
            INVALID_CODE_PATH;
        } break;
    }

    return nullptr;
}

inline vector<cycle_counter> init_cycle_counters()
{
    vector<cycle_counter> result;

    for (u32 i = 0; i < cycle_counter::type::count; ++i) {
        cycle_counter counter {};
        counter.t_   = (cycle_counter::type)i;
        counter.name = get_cycle_counter_name(counter.t_);
        result.push_back(counter);
    }

    return result;
}

inline void reset_cycle_counters(vector<cycle_counter> *counters)
{
    for (auto &counter : *counters) {
        counter.cycle_cnt = 0;
    }
}

inline cycle_counter *get_cycle_counter(vector<cycle_counter> *counters, cycle_counter::type type)
{
    for (auto &counter : *counters) {
        if (counter.t_ == type) {
            return &counter;
        }
    }

    return nullptr;
}

inline f32 get_seconds_elapsed(s64 start, s64 end, s64 performance_counter_frequency)
{
    return scast(f32, end - start) / scast(f32, performance_counter_frequency);
}

#define BEGIN_TIMED_BLOCK(ID) u64 start_cycle_cnt_##ID = __rdtsc()
#define END_TIMED_BLOCK(ID)                                                                \
    {                                                                                      \
        auto *counter      = get_cycle_counter(&state->counters, cycle_counter::type::ID); \
        counter->cycle_cnt = __rdtsc() - start_cycle_cnt_##ID;                             \
    }

}  // namespace tom

#endif  // !TOMATO_TIME_HPP