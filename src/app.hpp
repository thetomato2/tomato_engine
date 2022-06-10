#ifndef TOMATO_APP_HPP_
#define TOMATO_APP_HPP_

#include "core.hpp"
#include "win32.hpp"
#include "input.hpp"
#include "graphics.hpp"
#include "sound.hpp"
#include "camera.hpp"
#include "time.hpp"

namespace tom
{

struct constants
{
    m4 transform;
    m4 projection;
    v3 light_v3;
};

struct app_input
{
    input *current;
    input *last;
};

struct app_vars
{
    bool line_mode;
    f32 unit;
};

struct app_memory
{
    u64 permanent_storage_size;
    void *permanent_storage;  // NOTE: required to be cleared to 0!
    u64 transient_storage_size;
    void *transient_storage;  // NOTE: required to be cleared to 0!
};

struct app_state
{
    u32 game_update_hertz;
    f32 target_frames_per_second;
    app_vars vars;
    s64 performance_counter_frequency;
    u64 frame_cnt;
    u32 work_ind;
    f32 work_secs[256];
    f32 ms_frame;
    s32 fps;
    s32 target_fps;
    cycle_counter counters[cycle_counter_cnt];
    char exe_path[MAX_PATH];
    win32_state win32;
    graphics_state gfx;
    sound_state sound;
    input input;
    app_memory memory;
    void *memory_block;
    f32 dt;
    f32 fov;
    v3 rot;
    v3 rot_spd;
    v3 cam_pos;
    camera cam;
    v3 model_pos;
    v4 clear_color;
    szt total_size;
    bool imgui_demo;
    f64 val1, val2;

    bool wvp_sw;

    struct
    {
        m4 world;
        m4 view;
        m4 proj;
    } wvp;
};

s32 start(HINSTANCE hinst);

}  // namespace tom

#endif  // !TOMATO_APP_HPP_