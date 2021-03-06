#ifndef TOMATO_APP_HH_
#define TOMATO_APP_HH_

#include "core.hh"
#include "win32.hh"
#include "input.hh"
#include "graphics.hh"
#include "sound.hh"
#include "camera.hh"
#include "time.hh"
#include "string.hh"

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
    char exe_path[MAX_PATH];
    win32_state win32;
    gfx_state gfx;
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

    m4 world;
    m4 view;
    m4 proj;
    m4 wvp;
    
    vector<cycle_counter> counters;
};

s32 start(HINSTANCE hinst);

}  // namespace tom

#endif  // !TOMATO_APP_HPP_