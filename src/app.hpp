#ifndef TOMATO_APP_HPP_
#define TOMATO_APP_HPP_

#include "core.hpp"
#include "win32.hpp"
#include "input.hpp"
#include "graphics.hpp"
#include "sound.hpp"
#include "time.hpp"

namespace tom
{

struct float3
{
    float x, y, z;
};

struct matrix
{
    float m[4][4];
};

struct constants
{
    m4 transform;
    m4 projection;
    float3 light_v3;
};

struct app_input
{
    input new_input;
    input old_input;
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
    char exe_path[MAX_PATH];
    win32_state win32;
    graphics_state gfx;
    sound_state sound;
    app_input input;
    app_memory memory;
    void *memory_block;
    f32 dt;
    f32 fov;
    f32 z_rot;
    v3 model_pos;
    v4 clear_color;
    szt total_size;
    bool imgui_demo;

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