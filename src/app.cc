#include "app.hh"
#include "data.h"
#include "imgui.hh"
#include "vector.hh"
#include "file_io.hh"

namespace tom
{

internal void on_resize(app_state *state)
{
    f32 aspect = scast(f32, state->win32.win_dims.width) / scast(f32, state->win32.win_dims.height);
    state->wvp.proj = mat::proj_persp(aspect, state->fov, 1.0f, 1000.0f);
}

internal void app_init(app_state *state)
{
    state->fov            = 1.0f;
    state->clear_color    = { 0.1f, 0.3f, 0.3f, 1.0f };
    state->vars.unit      = 1.0f;
    state->imgui_demo     = false;
    state->model_pos      = { 0.0f, 0.0f, -4.0f };
    state->cam            = camera_init();
    state->cam.target_pos = state->model_pos;
    // state->cam_pos.z   = -4.0f;
    // state->cam.pos.z   = -4.0f;
    state->wvp.view = mat::identity();
    state->rot_spd  = {};

    // state->cam_main = camera_init();
    // state->cam_main.set_pos(state->cam_pos);
    // state->cam_main.speed = 5.0f;

#if 0
    const char *dir_name = "./test";
    if (!dir_exists(dir_name)) {
        CreateDirectoryA(dir_name, NULL);
    }

    constexpr szt buf_size = 4096 * 4096;
    char *buf              = new char[buf_size];

    for (szt i = 0; i < buf_size;) {
        buf[i++] = 'p';
        buf[i++] = 'o';
        buf[i++] = 'o';
        buf[i++] = 'p';
        buf[i++] = ' ';
    }
    write_entire_file("out.txt", sizeof(char) * buf_size, buf);
    vector<scoped_file> files;
    for (szt i = 0; i < 10; ++i) {
        files.emplace_back("out.txt");
    }

    szt file_i = 0;
    for (auto &f : files) {
        std::string out_name = "./test/out_" + std::to_string(file_i++) + ".txt";
        write_entire_file(out_name.c_str(), sizeof(char) * buf_size, buf);
    }

    #define NUM_ITER 100000

    BEGIN_TIMED_BLOCK(std_vec);

    std::vector<s32> std_vec;
    for (szt i = 0; i < NUM_ITER; ++i) {
        s32 x = i * i;
        std_vec.push_back(x);
    }
    END_TIMED_BLOCK(std_vec);

    // NOTE: my simple as fuck implementation is 2.5 - 3.0 ish times faster...
    BEGIN_TIMED_BLOCK(tom_vec);
    vector<s32> tom_vec;
    for (szt i = 0; i < NUM_ITER; ++i) {
        s32 x = i * i;
        tom_vec.push_back(x);
    }
    END_TIMED_BLOCK(tom_vec);

    {
        BEGIN_TIMED_BLOCK(std_ptr);
        f64 val = 0.0;
        for (s32 iter = 0; iter < NUM_ITER; ++iter) {
            auto foo = std::make_unique<f64>((f64)iter);
            val += *foo;
            foo.release();
        }
        state->val1 = val;

        END_TIMED_BLOCK(std_ptr);
    }

    {
        BEGIN_TIMED_BLOCK(raw_ptr);
        f64 val = 0.0;
        for (s32 iter = 0; iter < NUM_ITER; ++iter) {
            auto *foo = new f64 { (f64)iter };
            val += *foo;
            delete foo;
        }
        state->val2 = val;
        END_TIMED_BLOCK(raw_ptr);
    }

    f32 xt = (f32)state->counters[cycle_counter_raw_ptr].cycle_cnt /
             (f32)state->counters[cycle_counter_std_ptr].cycle_cnt;
    printf("raw / std: %f\n", xt);
    f32 xu = (f32)state->counters[cycle_counter_tom_vec].cycle_cnt /
             (f32)state->counters[cycle_counter_std_vec].cycle_cnt;
    printf("tom / std: %f\n", xu);

#endif

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
    BEGIN_TIMED_BLOCK(update);

    ImGui::Begin("Scene");
    ImGui::ColorEdit4("Clear", (f32 *)&state->clear_color.e[0]);
    ImGui::SliderFloat("fov", &state->fov, 0.1f, 3.0f);
    ImGui::SliderFloat3("Cube Pos", (f32 *)&state->model_pos, -5.0f, 10.0f);
    ImGui::SliderFloat("rotation speed x", &state->rot_spd.x, 0.0f, 10.0f);
    ImGui::SliderFloat("rotation speed y", &state->rot_spd.y, 0.0f, 10.0f);
    ImGui::SliderFloat("rotation speed z", &state->rot_spd.z, 0.0f, 10.0f);
    ImGui::End();

    ImGui::Begin("Camera");
    imgui_text_v3("pos", state->cam.pos);
    imgui_text_v3("up", state->cam.up);
    imgui_text_v3("forward", state->cam.forward);
    imgui_text_v3("target", state->cam.target_pos);
    ImGui::End();

    if (state->imgui_demo) ImGui::ShowDemoWindow(&state->imgui_demo);

    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;

    state->gfx.device_context->Map(state->gfx.const_buf, 0, D3D11_MAP_WRITE_DISCARD, 0,
                                   &mapped_sub_resource);

    auto *cons = rcast(constants *, mapped_sub_resource.pData);

    // float w = (f32)memory->win_dims.width / (f32)memory->win_dims.height;  // width (aspect
    // ratio) float h = 1.0f;                                                        // height float
    // n = 1.0f;                                                        // near float f = 9.0f; //
    // far

    // cons->projection = { 2 * n / w, 0,           0, 0, 0, 2 * n / h,       0, 0, 0,
    //                      0,         f / (f - n), 1, 0, 0, n * f / (n - f), 0 };

    // m4 model = mat::identity();
    // model    = mat::translate(model, { 0.0f, 0.0f, 4.0f });

    // state->cam.pos = state->cam_pos;
    // TODO: this doesn't work
    orbit_cam(&state->cam, state->input.keyboard, state->input.mouse, state->win32.win_dims);

    m4 model = mat::translate(state->model_pos);
    state->rot += state->rot_spd * state->dt;
    m4 rot = mat::rot_z(state->rot.z) * mat::rot_y(state->rot.y) * mat::rot_x(state->rot.x);
    model  = rot * model;
    cons->transform = model;
    state->wvp.view = state->cam.view();

    m4 wvp = state->wvp.view * state->wvp.proj;

    // wvp.e[11] = -1.0f;
    // wvp.e[15] = 4.0f;
    ImGui::Begin("WVP");
    imgui_text_m4("view", state->wvp.view);
    imgui_text_m4("wvp", wvp);
    ImGui::End();
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

    if (key_down(state->input.keyboard.d1)) {
        printf("1\n");
        printf("%d\n", state->input.keyboard.d1.half_transition_cnt);
    }
    if (key_up(state->input.keyboard.t)) {
        printf("t\n");
    }

    if (key_pressed(state->input.keyboard.r)) {
        printf("r\n");
        printf("%d\n", state->input.keyboard.r.half_transition_cnt);
        
    }
    
    END_TIMED_BLOCK(update);
}

s32 start(HINSTANCE hinst)
{
    const TCHAR *icon_path = _T(".\\data\\tomato.ico");
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
    state.target_fps                    = 144;
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

    state.win32.win_dims.width  = 1600;
    state.win32.win_dims.height = 900;

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

    s64 last_counter     = get_time();
    u64 last_cycle_count = __rdtsc();

    f32 delta_time = 0.0f;
    // NOTE: dummy thread context, for now
    thread_context thread {};

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
        ++state.frame_cnt;
        // printf("%llu\n",state.frame_cnt);
        if (!state.win32.running) break;
        if (state.win32.resize) {
            on_resize(&state);
            state.win32.resize = false;
        }

        state.target_frames_per_second = 1.0f / scast(f32, state.target_fps);

        state.win32.ms_scroll = 0;
        process_pending_messages(&state.win32);
        // do_controller_input(*old_input, *new_input, hwnd);
        // NOTE: this isn't calculated and needs to be for a variable framerate
        // state.dt            = state.target_frames_per_second;
        state.dt            = state.ms_frame / 1000.0f;
        local u64 frame_cnt = 0;
        local f32 one_sec   = 0.0f;
        ++frame_cnt;
        one_sec += state.ms_frame;
        if (one_sec > 1000.0f) {
            one_sec -= 1000.0f;
            state.fps = (s32)frame_cnt;
            frame_cnt = 0;
        }

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

        do_input(&state.input, state.win32.hwnd, state.win32.ms_scroll);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        app_update(&state);

        f32 work_secs_avg = 0.0f;
        for (u32 i = 0; i < ARRAY_COUNT(state.work_secs); ++i) {
            work_secs_avg += (f32)state.work_secs[i];
        }
        work_secs_avg /= (f32)ARRAY_COUNT(state.work_secs);

        ImGui::Begin("Cycles");
        ImGui::Text("tom_vec: %llu", state.counters[cycle_counter_tom_vec].cycle_cnt);
        ImGui::Text("std_vec: %llu", state.counters[cycle_counter_std_vec].cycle_cnt);
        ImGui::Text("raw_ptr: %llu", state.counters[cycle_counter_raw_ptr].cycle_cnt);
        ImGui::Text("std_ptr: %llu", state.counters[cycle_counter_std_ptr].cycle_cnt);
        ImGui::Text("Update: %llu", state.counters[cycle_counter_update].cycle_cnt);
        ImGui::Text("total: %llu", state.counters[cycle_counter_total].cycle_cnt);
        ImGui::End();

        ImGui::Begin("FPS");
        ImGui::Text("fps: %d", state.fps);
        ImGui::Text("frametime: %fms", state.ms_frame);
        ImGui::Text("work: %fms", work_secs_avg);
        // NOTE: can't go higher than 144... because of vsync?
        ImGui::SliderInt("target_fps:", &state.target_fps, 1, 144);
        ImGui::RadioButton("30", &state.target_fps, 30);
        ImGui::SameLine();
        ImGui::RadioButton("60", &state.target_fps, 60);
        ImGui::SameLine();
        ImGui::RadioButton("144", &state.target_fps, 144);
        ImGui::SameLine();
        ImGui::End();

        state.counters[cycle_counter_update].cycle_cnt = 0;

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        state.gfx.swap_chain->Present(1, 0);

        // fill_sound_buffer(sound_output, samples_to_write, sound_buffer);

        // clock stuffs
        auto work_counter = get_time();
        f32 work_seconds_elapsed =
            get_seconds_elapsed(last_counter, work_counter, state.performance_counter_frequency);
        state.work_secs[state.work_ind++] = work_seconds_elapsed;
        if (state.work_ind == ARRAY_COUNT(state.work_secs)) state.work_ind = 0;

        bool is_sleep_granular        = false;
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
        state.ms_frame   = 1000.f * get_seconds_elapsed(last_counter, end_counter,
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

        u64 end_cycle_count = __rdtsc();
        u64 cycles_elapsed  = end_cycle_count - last_cycle_count;
        last_cycle_count    = end_cycle_count;

        state.counters[cycle_counter_total].cycle_cnt = cycles_elapsed;
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
