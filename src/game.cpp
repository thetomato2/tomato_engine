#include "game.hpp"
#include "utils.hpp"
#include "data.h"

namespace tom
{

struct data_paths
{
    const char *name;
    const char *mesh;
    const char *albedo;
    const char *normal;
};

internal void on_resize(game_state *state, window_dims win_dims)
{
    f32 aspect = scast(f32, win_dims.width) / scast(f32, win_dims.height);
#if 1
    state->wvp.proj = mat::proj_persp(aspect, state->fov, 1.0f, 100.0f);
#else
    state->wvp.proj = mat::proj_ortho(aspect);
#endif
}

bool init(thread_context *thread, game_memory *memory)
{
    TOM_ASSERT(sizeof(game_state) <= memory->permanent_storage_size);
    // NOTE: placement new will rum member constructors (vector)
    game_state *state = new (memory->permanent_storage) game_state;
    // game_state *state = (game_state *)memory->permanent_storage;

    szt game_size  = sizeof(game_state);
    state->plat_io = &memory->plat_io;

    init_arena(&state->arena, memory->permanent_storage_size - game_size,
               (byt *)memory->permanent_storage + game_size);

    data_paths cube_paths = {
        .name = "cube", .mesh = "./cube.obj", .albedo = "./green.png", .normal = nullptr
    };

    data_paths cone_paths = {
        .name = "cone", .mesh = "./cone2.obj", .albedo = "./red.png", .normal = nullptr
    };

    data_paths sphere_paths = {
        .name = "sphere", .mesh = "./sphere.obj", .albedo = "./red.png", .normal = nullptr
    };

    data_paths plane_paths = {
        .name = "plane", .mesh = "./plane.obj", .albedo = "./pika.png", .normal = nullptr
    };

    state->gfx = &memory->d3d_context;

    state->fov         = 1.0f;
    state->clear_color = { 0.1f, 0.3f, 0.3f, 1.0f };
    state->vars.unit   = 1.0f;

    state->cam_main       = camera_init();
    state->cam_main.pos   = camera_set_pos(state->cam_pos);
    state->cam_main.speed = 5.0f;

    state->spec      = 32.0f;
    state->line_mode = false;
    state->imgui     = true;

    ID3DBlob *vs_blob, *ps_blob;
    D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob,
                       nullptr);
    state->gfx->device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                           nullptr, &state->d3d.vertex_shader);

    D3D11_INPUT_ELEMENT_DESC
    inputElementDesc[] =  // float3 position, float3 normal, float2 texcoord, float3 color
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
              D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
              D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
              D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

    state->gfx->device->CreateInputLayout(inputElementDesc, ARRAY_COUNT(inputElementDesc),
                                          vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                          &state->d3d.input_layout);

    D3DCompileFromFile(L"./shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &ps_blob,
                       nullptr);
    state->gfx->device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(),
                                          nullptr, &state->d3d.pixel_shader);

    D3D11_BUFFER_DESC constant_buf_desc = {};
    constant_buf_desc.ByteWidth =
        sizeof(constants) + 0xf & 0xfffffff0;  // round constant buffer size to 16 byte boundary
    constant_buf_desc.Usage          = D3D11_USAGE_DYNAMIC;
    constant_buf_desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    constant_buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    state->gfx->device->CreateBuffer(&constant_buf_desc, nullptr, &state->d3d.const_buf);

    D3D11_BUFFER_DESC vert_buf_desc = {};
    vert_buf_desc.ByteWidth         = sizeof(VertexData);
    vert_buf_desc.Usage             = D3D11_USAGE_IMMUTABLE;
    vert_buf_desc.BindFlags         = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = { VertexData };

    state->gfx->device->CreateBuffer(&vert_buf_desc, &vertexData, &state->d3d.vert_buf);

    D3D11_BUFFER_DESC ind_buf_desc = {};
    ind_buf_desc.ByteWidth         = sizeof(IndexData);
    ind_buf_desc.Usage             = D3D11_USAGE_IMMUTABLE;
    ind_buf_desc.BindFlags         = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA index_data = { IndexData };

    state->gfx->device->CreateBuffer(&ind_buf_desc, &index_data, &state->d3d.ind_buf);

    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width                = TEXTURE_WIDTH;   // in data.h
    tex_desc.Height               = TEXTURE_HEIGHT;  // in data.h
    tex_desc.MipLevels            = 1;
    tex_desc.ArraySize            = 1;
    tex_desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    tex_desc.SampleDesc.Count     = 1;
    tex_desc.Usage                = D3D11_USAGE_IMMUTABLE;
    tex_desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA tex_data = {};
    tex_data.pSysMem                = TextureData;
    tex_data.SysMemPitch            = TEXTURE_WIDTH * 4;  // 4 bytes per pixel

    state->gfx->device->CreateTexture2D(&tex_desc, &tex_data, &state->d3d.texture);

    state->gfx->device->CreateShaderResourceView(state->d3d.texture, nullptr,
                                                 &state->d3d.texture_view);

    state->d3d.stride = 11 * 4;
    state->d3d.offset = 0;

    state->d3d.viewport = { 0.0f, 0.0f, (f32)memory->win_dims.width, (f32)memory->win_dims.height,
                            0.0f, 1.0f };

    on_resize(state, memory->win_dims);

    return true;
}

void update(thread_context *thread, game_memory *memory, game_input input, f32 dt)
{
    game_state *state = (game_state *)memory->permanent_storage;

    if (memory->win_resize || state->fov_old < state->fov - math::eps_f32 ||
        state->fov_old > state->fov + math::eps_f32) {
        on_resize(state, memory->win_dims);
        memory->win_resize = false;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene");
    ImGui::ColorEdit4("Clear", (f32 *)&state->clear_color.e[0]);
    ImGui::SliderFloat("fov", &state->fov, 0.1f, 3.0f);
    ImGui::End();

    state->imgui = false;
    if (state->imgui) ImGui::ShowDemoWindow(&state->imgui);

    d3d::d3d_context *gfx = state->gfx;

    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;

    gfx->device_context->Map(state->d3d.const_buf, 0, D3D11_MAP_WRITE_DISCARD, 0,
                             &mapped_sub_resource);

    auto *cons = rcast(constants *, mapped_sub_resource.pData);

    float3 modelScale       = { 1.0f, 1.0f, 1.0f };
    float3 modelTranslation = { 0.0f, 0.0f, 4.0f };

    float w = (f32)memory->win_dims.width / (f32)memory->win_dims.height;  // width (aspect ratio)
    float h = 1.0f;                                                        // height
    float n = 1.0f;                                                        // near
    float f = 9.0f;                                                        // far

    cons->projection = { 2 * n / w, 0,           0, 0, 0, 2 * n / h,       0, 0, 0,
                         0,         f / (f - n), 1, 0, 0, n * f / (n - f), 0 };

    // m4 model = mat::identity();
    // model    = mat::translate(model, { 0.0f, 0.0f, 4.0f });

    m4 model = mat::translate({ 0.0f, 0.0f, 4.0f });
    state->z_rot += dt;
    m4 rot           = mat::rot_z(state->z_rot) * mat::rot_x(state->z_rot);
    model            = rot * model;
    cons->transform  = model;
    cons->projection = state->wvp.proj;
    cons->light_v3   = { 1.0f, -1.0f, 1.0f };

    gfx->device_context->Unmap(state->d3d.const_buf, 0);

    gfx->device_context->ClearRenderTargetView(gfx->frame_buf_view, state->clear_color.e);
    gfx->device_context->ClearDepthStencilView(gfx->depth_buf_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

    gfx->device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfx->device_context->IASetInputLayout(state->d3d.input_layout);
    gfx->device_context->IASetVertexBuffers(0, 1, &state->d3d.vert_buf, &state->d3d.stride,
                                            &state->d3d.offset);
    gfx->device_context->IASetIndexBuffer(state->d3d.ind_buf, DXGI_FORMAT_R32_UINT, 0);

    gfx->device_context->VSSetShader(state->d3d.vertex_shader, nullptr, 0);
    gfx->device_context->VSSetConstantBuffers(0, 1, &state->d3d.const_buf);

    gfx->device_context->RSSetViewports(1, &state->d3d.viewport);
    gfx->device_context->RSSetState(gfx->rasterizer_state);

    gfx->device_context->PSSetShader(state->d3d.pixel_shader, nullptr, 0);
    gfx->device_context->PSSetShaderResources(0, 1, &state->d3d.texture_view);
    gfx->device_context->PSSetSamplers(0, 1, &gfx->sampler_state);

    gfx->device_context->OMSetRenderTargets(1, &gfx->frame_buf_view, gfx->depth_buf_view);
    gfx->device_context->OMSetDepthStencilState(gfx->depth_stencil_state, 0);
    gfx->device_context->OMSetBlendState(nullptr, nullptr,
                                         0xffffffff);  // use default blend mode (i.e. disable)

    gfx->device_context->DrawIndexed(ARRAY_COUNT(IndexData), 0, 0);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    gfx->swap_chain->Present(1, 0);
}

// NOTE: clean up here
bool exit(thread_context *thread, game_memory *memory)
{
    game_state *state = (game_state *)memory->permanent_storage;

    return true;
}

}  // namespace tom