#include "app.hpp"
#include "data.h"
#include "imgui_style.h"

namespace tom
{
internal void on_resize(app_state *state)
{
    f32 aspect = scast(f32, state->win32.win_dims.width) / scast(f32, state->win32.win_dims.height);
    state->wvp.proj = mat::proj_persp(aspect, state->fov, 1.0f, 100.0f);
}

internal void app_init(app_state *state)
{
    state->fov         = 1.0f;
    state->clear_color = { 0.1f, 0.3f, 0.3f, 1.0f };
    state->vars.unit   = 1.0f;
    state->imgui_demo  = false;
    state->model_pos   = { 0.0f, 0.0f, 0.0f };
    state->cam         = camera_init();
    state->cam_pos.z   = -4.0f;
    state->cam.pos.z   = -4.0f;
    state->wvp.view    = mat::identity();

    // state->cam_main = camera_init();
    // state->cam_main.set_pos(state->cam_pos);
    // state->cam_main.speed = 5.0f;

    ID3DBlob *vs_blob, *ps_blob;
    D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob,
                       nullptr);
    state->gfx.device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                          nullptr, &state->gfx.vertex_shader);

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

    state->gfx.device->CreateInputLayout(inputElementDesc, ARRAY_COUNT(inputElementDesc),
                                         vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                         &state->gfx.input_layout);

    D3DCompileFromFile(L"./shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &ps_blob,
                       nullptr);
    state->gfx.device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(),
                                         nullptr, &state->gfx.pixel_shader);

    D3D11_BUFFER_DESC constant_buf_desc = {};
    constant_buf_desc.ByteWidth =
        sizeof(constants) + 0xf & 0xfffffff0;  // round constant buffer size to 16 byte boundary
    constant_buf_desc.Usage          = D3D11_USAGE_DYNAMIC;
    constant_buf_desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    constant_buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    state->gfx.device->CreateBuffer(&constant_buf_desc, nullptr, &state->gfx.const_buf);

    D3D11_BUFFER_DESC vert_buf_desc = {};
    vert_buf_desc.ByteWidth         = sizeof(VertexData);
    vert_buf_desc.Usage             = D3D11_USAGE_IMMUTABLE;
    vert_buf_desc.BindFlags         = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = { VertexData };

    state->gfx.device->CreateBuffer(&vert_buf_desc, &vertexData, &state->gfx.vert_buf);

    D3D11_BUFFER_DESC ind_buf_desc = {};
    ind_buf_desc.ByteWidth         = sizeof(IndexData);
    ind_buf_desc.Usage             = D3D11_USAGE_IMMUTABLE;
    ind_buf_desc.BindFlags         = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA index_data = { IndexData };

    state->gfx.device->CreateBuffer(&ind_buf_desc, &index_data, &state->gfx.ind_buf);

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

    state->gfx.device->CreateTexture2D(&tex_desc, &tex_data, &state->gfx.texture);

    state->gfx.device->CreateShaderResourceView(state->gfx.texture, nullptr,
                                                &state->gfx.texture_view);

    state->gfx.stride = 11 * 4;
    state->gfx.offset = 0;

    state->gfx.viewport = {
        0.0f, 0.0f, (f32)state->win32.win_dims.width, (f32)state->win32.win_dims.height, 0.0f, 1.0f
    };

    on_resize(state);
}

internal void app_update(app_state *state)
{
    ImGui::Begin("Scene");
    ImGui::ColorEdit4("Clear", (f32 *)&state->clear_color.e[0]);
    ImGui::SliderFloat("fov", &state->fov, 0.1f, 3.0f);
    ImGui::SliderFloat3("Cube Pos", (f32 *)&state->model_pos, -5.0f, 10.0f);
    ImGui::SliderFloat3("cam Pos", (f32 *)&state->cam_pos, -10.0f, -2.0f);
    ImGui::End();

    if (state->imgui_demo) ImGui::ShowDemoWindow(&state->imgui_demo);

    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;

    state->gfx.device_context->Map(state->gfx.const_buf, 0, D3D11_MAP_WRITE_DISCARD, 0,
                                   &mapped_sub_resource);

    auto *cons = rcast(constants *, mapped_sub_resource.pData);

    float3 modelScale       = { 1.0f, 1.0f, 1.0f };
    float3 modelTranslation = { 0.0f, 0.0f, 4.0f };

    // float w = (f32)memory->win_dims.width / (f32)memory->win_dims.height;  // width (aspect
    // ratio) float h = 1.0f;                                                        // height float
    // n = 1.0f;                                                        // near float f = 9.0f; //
    // far

    // cons->projection = { 2 * n / w, 0,           0, 0, 0, 2 * n / h,       0, 0, 0,
    //                      0,         f / (f - n), 1, 0, 0, n * f / (n - f), 0 };

    // m4 model = mat::identity();
    // model    = mat::translate(model, { 0.0f, 0.0f, 4.0f });

    // state->cam.pos = state->cam_pos;
    orbit_cam(&state->cam, state->input.new_input.keyboard, state->input.new_input.mouse,
              state->win32.win_dims);

    m4 model = mat::translate(state->model_pos);
    state->z_rot += state->dt;
    m4 rot           = mat::rot_z(state->z_rot) * mat::rot_x(state->z_rot);
    model            = rot * model;
    cons->transform  = model;
    state->wvp.view  = state->cam.view();
    m4 wvp           = state->wvp.view * state->wvp.proj;
    cons->projection = wvp;
    cons->light_v3   = { 1.0f, -1.0f, 1.0f };

    state->gfx.device_context->Unmap(state->gfx.const_buf, 0);

    state->gfx.device_context->ClearRenderTargetView(state->gfx.frame_buf_view,
                                                     state->clear_color.e);
    state->gfx.device_context->ClearDepthStencilView(state->gfx.depth_buf_view, D3D11_CLEAR_DEPTH,
                                                     1.0f, 0);

    state->gfx.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    state->gfx.device_context->IASetInputLayout(state->gfx.input_layout);
    state->gfx.device_context->IASetVertexBuffers(0, 1, &state->gfx.vert_buf, &state->gfx.stride,
                                                  &state->gfx.offset);
    state->gfx.device_context->IASetIndexBuffer(state->gfx.ind_buf, DXGI_FORMAT_R32_UINT, 0);

    state->gfx.device_context->VSSetShader(state->gfx.vertex_shader, nullptr, 0);
    state->gfx.device_context->VSSetConstantBuffers(0, 1, &state->gfx.const_buf);

    state->gfx.device_context->RSSetViewports(1, &state->gfx.viewport);
    state->gfx.device_context->RSSetState(state->gfx.rasterizer_state);

    state->gfx.device_context->PSSetShader(state->gfx.pixel_shader, nullptr, 0);
    state->gfx.device_context->PSSetShaderResources(0, 1, &state->gfx.texture_view);
    state->gfx.device_context->PSSetSamplers(0, 1, &state->gfx.sampler_state);

    state->gfx.device_context->OMSetRenderTargets(1, &state->gfx.frame_buf_view,
                                                  state->gfx.depth_buf_view);
    state->gfx.device_context->OMSetDepthStencilState(state->gfx.depth_stencil_state, 0);
    state->gfx.device_context->OMSetBlendState(
        nullptr, nullptr,
        0xffffffff);  // use default blend mode (i.e. disable)

    state->gfx.device_context->DrawIndexed(ARRAY_COUNT(IndexData), 0, 0);
}

s32 start(HINSTANCE hinst)
{
    const TCHAR *icon_path = _T(".\\tomato.ico");
    auto icon              = (HICON)(LoadImage(NULL, icon_path, IMAGE_ICON, 0, 0,
                                               LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED));

    create_console();
    auto cons_hwnd = GetConsoleWindow();
    TOM_ASSERT(cons_hwnd);
    SendMessage(cons_hwnd, WM_SETICON, NULL, (LPARAM)icon);

    printf("Starting...\n");

#if _CPPUWIND
    printf("Exceptions are enabled!\n");
#endif

    app_state state                     = {};
    state.game_update_hertz             = 60;
    state.target_frames_per_second      = 1.0f / scast(f32, state.game_update_hertz);
    state.sound.frames_of_audio_latency = (1.1f / 30.f) * scast(f32, state.game_update_hertz);
    state.win32.icon                    = icon;

    DWORD exe_path_len = GetModuleFileNameA(NULL, state.exe_path, sizeof(state.exe_path));
    printf("exe path %s\n", state.exe_path);

    TCHAR cwd_buf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cwd_buf);
    _tprintf(TEXT("cwd %s\n"), cwd_buf);

    char *p_ = &state.exe_path[exe_path_len];
    s32 i_   = scast(s32, exe_path_len);
    while (i_ > -1 && state.exe_path[i_] != '\\') {
        --i_;
    }

    TCHAR set_cwd_buf[MAX_PATH];
    for (int i = 0; i < i_; ++i) {
        set_cwd_buf[i] = state.exe_path[i];
    }
    set_cwd_buf[i_] = '\0';

    bool cwd_is_exe = true;
    int it_buf      = 0;
    while (cwd_buf[it_buf]) {
        if (cwd_buf[it_buf] != set_cwd_buf[it_buf]) cwd_is_exe = false;
        ++it_buf;
    }

    if (!cwd_is_exe) {
        printf("cwd is not exe dir!\n");
        if (!SetCurrentDirectory(set_cwd_buf)) {
            printf("Failed to set cwd!", GetLastError());
        } else {
            GetCurrentDirectory(MAX_PATH, cwd_buf);
            _tprintf(TEXT("set cwd to %s\n"), cwd_buf);
        }
    }

    LARGE_INTEGER performance_query_result;
    QueryPerformanceFrequency(&performance_query_result);
    state.performance_counter_frequency = performance_query_result.QuadPart;

    // load_Xinput();

    state.win32.win_dims.width  = 1280;
    state.win32.win_dims.height = 720;

    // state->sound.frames_of_audio_latency = (1.1f / 30.f) * scast(f32, win32::game_update_hertz);

    // sound_output sound_output = {
    //     .samples_per_sec      = 48000,
    //     .bytes_per_sample     = sizeof(s16) * 2,
    //     .secondary_buf_size   = scast(DWORD, sound_output.samples_per_sec),
    //     .latency_sample_count = scast(
    //         s32, g_frames_of_audio_latency *
    //                  scast(f32, (sound_output.samples_per_sec / scast(f32,

    // init_WASAPI(sound_output.samples_per_sec, sound_output.secondary_buf_size);
    // g_audio_client->Start();

    // s16 *samples = scast(s16 *, VirtualAlloc(0, sound_output.secondary_buf_size,
    //                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

#ifdef TOM_INTERNAL
    LPVOID base_address = rcast(LPVOID, TERABYTES(scast(u64, 2)));
#else
    LPVOID base_address = 0;
#endif

    state.memory.permanent_storage_size = MEGABYTES(256);
    state.memory.transient_storage_size = GIGABYTES(1);
    state.total_size = state.memory.permanent_storage_size + state.memory.transient_storage_size;
    // TODO: use large pages
    state.memory_block =
        VirtualAlloc(base_address, state.total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    state.memory.permanent_storage = state.memory_block;
    state.memory.transient_storage =
        (scast(u8 *, state.memory.permanent_storage) + state.memory.permanent_storage_size);

    create_window(&state.win32);
    SetCursorPos(state.win32.win_dims.width / 2, state.win32.win_dims.height / 2);
    d3d_init(state.win32.hwnd, &state.gfx);

    LARGE_INTEGER last_counter = get_time();
    u64 last_cycle_count       = __rdtsc();

    f32 delta_time = 0.0f;
    // NOTE: dummy thread context, for now
    thread_context thread {};

    // ImGui
    //  TODO: where to put this?
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplWin32_Init(state.win32.hwnd);
    ImGui_ImplDX11_Init(state.gfx.device, state.gfx.device_context);
    set_ImGui_style();

    state.win32.running = true;

    app_init(&state);

    while (true) {
        if (!state.win32.running) break;
        if (state.win32.resize) {
            on_resize(&state);
            state.win32.resize = false;
        }

        state.win32.ms_scroll = 0;
        process_pending_messages(&state.win32);
        // do_controller_input(*old_input, *new_input, hwnd);
        // NOTE: this isn't calculated and needs to be for a varaible framerate
        state.dt = state.target_frames_per_second;

        // REFERENCE_TIME latency;
        // if (SUCCEEDED(g_audio_client->GetStreamLatency(&latency))) {
        // } else {
        //     printf("ERROR--> Failed to get audio latency\n");
        // }

        // s32 samples_to_write;
        // UINT32 sound_pad_size;
        // if (SUCCEEDED(g_audio_client->GetCurrentPadding(&sound_pad_size))) {
        //     s32 maxSampleCnt = scast(s32, sound_output.secondary_buf_size - sound_pad_size);
        //     samples_to_write = scast(s32, sound_output.latency_sample_count - sound_pad_size);
        //     if (samples_to_write < 0) samples_to_write = 0;
        //     // TOM_ASSERT(samplesToWrite < maxSampleCnt);
        // }

        // sound_output_buffer sound_buffer = { .samples_per_second = sound_output.samples_per_sec,
        //                                      .sample_count       = samples_to_write,
        //                                      .samples            = samples };

        do_input(&state.input.old_input, &state.input.old_input, state.win32.hwnd,
                 state.win32.ms_scroll);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        app_update(&state);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        state.gfx.swap_chain->Present(1, 0);

        // fill_sound_buffer(sound_output, samples_to_write, sound_buffer);

        // clock stuffs
        auto work_counter = get_time();
        f32 work_seconds_elapsed =
            get_seconds_elapsed(last_counter, work_counter, state.performance_counter_frequency);
        bool is_sleep_granular = false;

        f32 seconds_elapsed_for_frame = work_seconds_elapsed;
        if (seconds_elapsed_for_frame < state.target_frames_per_second) {
            if (is_sleep_granular) {
                auto sleepMs = scast(
                    DWORD, 1000.f * (state.target_frames_per_second - seconds_elapsed_for_frame));
                if (sleepMs > 0) {
                    ::Sleep(sleepMs);
                }
            }
            f32 test_seconds_elapsed_for_frame =
                get_seconds_elapsed(last_counter, get_time(), state.performance_counter_frequency);
            while (seconds_elapsed_for_frame < state.target_frames_per_second) {
                seconds_elapsed_for_frame = get_seconds_elapsed(
                    last_counter, get_time(), state.performance_counter_frequency);
            }
        } else {
            printf("WARNING--> missed frame timing!!!\n");
        }

        auto end_counter = get_time();
        f32 ms_per_frame = 1000.f * get_seconds_elapsed(last_counter, end_counter,
                                                        state.performance_counter_frequency);
        // printf("%f\n", ms_per_frame);

        last_counter = end_counter;

        UINT64 frequency_position;
        UINT64 units_posisition;

        // g_audio_clock->GetFrequency(&frequency_position);
        // g_audio_clock->GetPosition(&units_posisition, 0);

        // play_cursor =
        //     scast(DWORD, sound_output.samples_per_sec * units_posisition / frequency_position) %
        //     sound_output.samples_per_sec;
        // write_cursor =
        //     scast(DWORD, sound_output.samples_per_sec * units_posisition / frequency_position) %
        //         sound_output.samples_per_sec +
        //     sound_pad_size;
        // if (write_cursor > sound_output.secondary_buf_size) {
        //     write_cursor -= sound_output.secondary_buf_size;
        // }

        auto temp_input       = state.input.new_input;
        state.input.new_input = state.input.old_input;
        state.input.old_input = temp_input;

        u64 end_cycle_count = __rdtsc();
        u64 cycles_elapsed  = end_cycle_count - last_cycle_count;
        last_cycle_count    = end_cycle_count;
    }

    ImGui_ImplDX11_Shutdown();

    ImGui::DestroyContext();
    // ImGui_ImplWin32_Shutdown();

    ReleaseDC(state.win32.hwnd, state.win32.hdc);
    DestroyWindow(state.win32.hwnd);
    UnregisterClass(state.win32.cls_name, NULL);

    return 0;
}

}  // namespace tom
