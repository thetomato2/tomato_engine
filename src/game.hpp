#ifndef TOMATO_GAME_HPP_
#define TOMATO_GAME_HPP_

#include "core.hpp"
#include "array.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "noise.hpp"
#include "stack.hpp"
#include "vector.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "obj_loader.hpp"

#define MAX_ENT_CNT 256

namespace tom
{

struct game_memory
{
    b32 is_initialized;
    b32 line_mode;
    u64 permanent_storage_size;
    void *permanent_storage;  // NOTE: required to be cleared to 0!
    u64 transient_storage_size;
    void *transient_storage;  // NOTE: required to be cleared to 0!
    platform_io plat_io;
    window_dims win_dims;
    b32 win_resize;
    d3d::d3d_context d3d_context;
};

struct game_input
{
    global constexpr szt controller_cnt = 4;
    mouse mouse;
    keyboard keyboard;
    controller controllers[controller_cnt];
};

struct model
{
    const char *name;
    v3 color;
    // TODO: mutliple meshes?
    mesh mesh;
    texture tex_alb;
    texture tex_nrm;
};

struct light
{
    bool active;
    f32 spec_strength;
    v3 pos;
    v3 color;
    mesh mesh;
};

enum class entity_type
{
    none,
    object,
    light
};

struct entity
{
    const char *name;
    v3 pos;
    model model;
};

struct game_vars
{
    f32 unit;
};

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

struct game_state
{
    memory_arena arena;
    bool running;
    bool line_mode;
    bool imgui;
    platform_io *plat_io;
    d3d::d3d_context *gfx;
    struct
    {
        ID3D11PixelShader *pixel_shader;
        ID3D11VertexShader *vertex_shader;
        ID3D11InputLayout *input_layout;
        ID3D11Buffer *const_buf;
        ID3D11Buffer *vert_buf;
        ID3D11Buffer *ind_buf;
        ID3D11Texture2D *texture;
        ID3D11ShaderResourceView *texture_view;
        D3D11_VIEWPORT viewport;
        u32 stride;
        u32 offset;
    } d3d;

    game_vars vars;

    keyboard keyboard;
    mouse mouse;
    struct
    {
        m4 world;
        m4 view;
        m4 proj;
    } wvp;

    camera cam_main;
    v3 cam_pos;
    f32 fov;
    f32 fov_old;
    f32 time_last;
    v4 clear_color;

    texture_buffer tex_buf;
    u32 cur_buf_line;

    shader obj_shader;
    shader light_shader;

    u32 ent_cnt;
    stack<entity, MAX_ENT_CNT> entities;
    light light;
    f32 spec;
    f32 z_rot;

    v3 obj_col;
};

struct offscreen_buffer
{
    void *memory;
    s32 width;
    s32 height;
    s32 pitch;
    s32 bytes_per_pixel;
};

struct sound_output_buffer
{
    s32 samples_per_second;
    s32 sample_count;
    s16 *samples;
    s32 tone_hertz;
};

bool init(thread_context *thread, game_memory *memory);
void update(thread_context *thread, game_memory *memory, game_input input, f32 dt);
bool exit(thread_context *thread, game_memory *memory);

// clang-format off
inline matrix operator*(const matrix& m1, const matrix& m2)
{
    return
    {
        m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],
        m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],
        m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],
        m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],
        m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],
        m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],
        m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],
        m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],
        m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],
        m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],
        m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],
        m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],
        m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],
        m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],
        m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],
        m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3],
    };
}
// clang-format on

}  // namespace tom

#endif
