#include "win32_platform.hpp"

#include "imgui_style.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

namespace tom
{
namespace win32
{

global constexpr u32 g_game_update_hertz        = 60;
global constexpr f32 g_target_frames_per_second = 1.f / scast(f32, g_game_update_hertz);

global bool running;
global bool g_pause;
global bool g_resize;
global bool g_debug_show_cursor;

global s32 ms_scroll;

WINDOWPLACEMENT g_win_pos = { sizeof(g_win_pos) };

global offscreen_buffer g_back_buffer;
global window_dims g_win_dim;
global s64 g_performance_counter_frequency;

// TODO: the sleep precision issue is keeping this above 1 frame... I think
global constexpr f32 g_frames_of_audio_latency = (1.1f / 30.f) * scast(f32, g_game_update_hertz);

global IAudioClient *g_audio_client;
global IAudioRenderClient *g_audio_render_client;
global IAudioClock *g_audio_clock;

//! this is a roundabout way of extracting a method out of a header...
#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE(xinput_get_state);
XINPUT_GET_STATE(_xinput_get_state)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
xinput_get_state *XInputGetState_ { _xinput_get_state };

#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);
XINPUT_SET_STATE(_xinput_set_state)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
xinput_set_state *XInputSetState_ { _xinput_set_state };

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

void free_file_memory(thread_context *thread, void *memory)
{
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    } else {
        INVALID_CODE_PATH;
    }
}

read_file_result read_entire_file(thread_context *thread, const char *file_name)
{
    read_file_result file = {};

    HANDLE file_handle { CreateFileA(file_name, GENERIC_READ, 0, 0, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, 0) };
    if (file_handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(file_handle, &fileSize)) {
            u32 fileSize32 = safe_truncate_u32_to_u64(fileSize.QuadPart);
            file.contents  = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (file.contents) {
                DWORD bytesRead;
                if (ReadFile(file_handle, file.contents, (DWORD)fileSize.QuadPart, &bytesRead, 0) &&
                    fileSize32 == bytesRead) {
                    file.content_size = fileSize32;
                } else {
                    free_file_memory(thread, file.contents);
                    file.contents = 0;
                }
            } else {
                printf("ERROR-> Failed to read file contents!\n");
            }
        } else {
            printf("ERROR-> Failed to open file handle!\n");
        }
        CloseHandle(file_handle);
    }
    return file;
}

b32 write_entire_file(thread_context *thread, const char *file_name, u64 memory_size, void *memory)
{
    b32 success = false;

    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (file_handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if (WriteFile(file_handle, memory, (DWORD)memory_size, &bytes_written, 0)) {
            success = (bytes_written == memory_size);
        } else {
            printf("ERROR-> Failed to write file contents!\n");
        }
        CloseHandle(file_handle);
    } else {
        printf("ERROR-> Failed to oepn file handle!\n");
    }
    return success;
}

internal void toggle_fullscreen(HWND hwnd)
{
    DWORD dwStyle = scast(DWORD, GetWindowLong(hwnd, GWL_STYLE));
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(hwnd, &g_win_pos) &&
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &g_win_pos);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void load_Xinput()
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!XInputLibrary) {
        HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }
    if (XInputLibrary) {
        XInputGetState = (xinput_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (xinput_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    } else {
        printf("ERROR->failed to load XInput!\n");
    }
}

internal void init_WASAPI(s32 samples_per_second, s32 buffer_size_in_samples)
{
    if (FAILED(CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY))) {
        INVALID_CODE_PATH;
    }

    IMMDeviceEnumerator *enumerator = {};
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&enumerator)))) {
        INVALID_CODE_PATH;
    }

    IMMDevice *device = {};
    if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
        INVALID_CODE_PATH;
    }

    if (FAILED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                                (LPVOID *)&g_audio_client))) {
        INVALID_CODE_PATH;
    }

    WAVEFORMATEXTENSIBLE wave_format  = {};
    wave_format.Format.cbSize         = sizeof(wave_format);
    wave_format.Format.wFormatTag     = WAVE_FORMAT_EXTENSIBLE;
    wave_format.Format.wBitsPerSample = 16;
    wave_format.Format.nChannels      = 2;
    wave_format.Format.nSamplesPerSec = scast(DWORD, samples_per_second);
    wave_format.Format.nBlockAlign =
        (WORD)(wave_format.Format.nChannels * wave_format.Format.wBitsPerSample / 8);
    wave_format.Format.nAvgBytesPerSec =
        wave_format.Format.nSamplesPerSec * wave_format.Format.nBlockAlign;
    wave_format.Samples.wValidBitsPerSample = 16;
    wave_format.dwChannelMask               = KSAUDIO_SPEAKER_STEREO;
    wave_format.SubFormat                   = KSDATAFORMAT_SUBTYPE_PCM;

    // buffer size in 100 nanoseconds
    REFERENCE_TIME buffer_duration =
        scast(REFERENCE_TIME, 10000000ULL * buffer_size_in_samples / samples_per_second);

    if (FAILED(g_audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST,
                                          buffer_duration, 0, &wave_format.Format, nullptr))) {
        INVALID_CODE_PATH;
    }

    if (FAILED(g_audio_client->GetService(IID_PPV_ARGS(&g_audio_render_client)))) {
        INVALID_CODE_PATH;
    }

    UINT32 sound_frame_cnt;
    if (FAILED(g_audio_client->GetBufferSize(&sound_frame_cnt))) {
        INVALID_CODE_PATH;
    }

    if (FAILED(g_audio_client->GetService(IID_PPV_ARGS(&g_audio_clock)))) {
        INVALID_CODE_PATH;
    }

    // Check if we got what we requested (better would to pass this value back
    // as real buffer size)
    TOM_ASSERT(buffer_size_in_samples <= scast(s32, sound_frame_cnt));
}

internal void fill_sound_buffer(sound_output &sound_output, s32 samples_to_write,
                                sound_output_buffer &source_buffer)
{
    {
        BYTE *sound_buf_dat;
        if (SUCCEEDED(g_audio_render_client->GetBuffer(scast(UINT32, samples_to_write),
                                                       &sound_buf_dat))) {
            s16 *sourceSample = source_buffer.samples;
            s16 *destSample   = rcast(s16 *, sound_buf_dat);
            for (szt i = 0; i < samples_to_write; ++i) {
                *destSample++ = *sourceSample++;
                *destSample++ = *sourceSample++;
                ++sound_output.running_sample_index;
            }

            g_audio_render_client->ReleaseBuffer(scast(UINT32, samples_to_write), 0);
        }
    }
}

internal window_dims get_window_dimensions(HWND hwnd)
{
    RECT client_rect;
    window_dims win_dim;
    GetClientRect(hwnd, &client_rect);
    win_dim.width  = client_rect.right - client_rect.left;
    win_dim.height = client_rect.bottom - client_rect.top;
    return win_dim;
}

internal void resize_dib_section(offscreen_buffer &buffer, s32 width, s32 height)
{
    // maybe don't free first, free after, then free first if that fails

    if (buffer.memory) {
        VirtualFree(buffer.memory, 0, MEM_RELEASE);
    }
    buffer.width           = width;
    buffer.height          = height;
    buffer.bytes_per_pixel = 4;

    buffer.info.bmiHeader.biSize        = sizeof(buffer.info.bmiHeader);
    buffer.info.bmiHeader.biWidth       = width;
    buffer.info.bmiHeader.biHeight      = -height;
    buffer.info.bmiHeader.biPlanes      = 1;
    buffer.info.bmiHeader.biBitCount    = 32;
    buffer.info.bmiHeader.biCompression = BI_RGB;

    s32 bytes_per_pixel    = 4;
    s32 bitmap_memory_size = (width * height) * bytes_per_pixel;
    buffer.memory = VirtualAlloc(0, bitmap_memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    buffer.pitch = width * buffer.bytes_per_pixel;
}

internal void display_buffer_in_window(HDC hdc, offscreen_buffer &buffer, s32 x, s32 y, s32 width,
                                       s32 height)
{
    if (width == buffer.width * 2 && height == buffer.height * 2) {
        ::StretchDIBits(hdc, 0, 0, width, height, 0, 0, buffer.width, buffer.height, buffer.memory,
                        &buffer.info, DIB_RGB_COLORS, SRCCOPY);
    } else {
        s32 offset_x = 0, offset_y = 0;

#if 0
        // NOTE: this causes screen flickering - out of sync with screen refresh rate?
        ::PatBlt(hdc, 0, 0, width, offset_y, BLACKNESS);
        ::PatBlt(hdc, 0, offset_y + buffer.height, width, height, BLACKNESS);
        ::PatBlt(hdc, 0, 0, offset_x, height, BLACKNESS);
        ::PatBlt(hdc, offset_x + buffer.width, 0, width, height, BLACKNESS);

#endif

        // NOTE: this is matches the windows dimensions
        ::StretchDIBits(hdc, offset_x, offset_y, buffer.width, buffer.height, 0, 0, buffer.width,
                        buffer.height, buffer.memory, &buffer.info, DIB_RGB_COLORS, SRCCOPY);
    }
}

static void process_keyboard_message(key &new_state, const b32 is_down)
{
    if (new_state.ended_down != (is_down != 0)) {
        new_state.ended_down = is_down;
        ++new_state.half_transition_cnt;
    }
}

internal void process_Xinput_digital_button(DWORD Xinput_button_state_, button &old_state_,
                                            DWORD button_bit_, button &new_state)
{
    new_state.ended_down = ((Xinput_button_state_ & button_bit_) == button_bit_);
    if (new_state.ended_down && old_state_.ended_down)
        new_state.half_transition_cnt = ++old_state_.half_transition_cnt;
}

internal void do_controller_input(game_input &old_input, game_input &new_input, HWND hwnd)
{
    // mouse cursor
    POINT mouse_point;
    GetCursorPos(&mouse_point);
    ScreenToClient(hwnd, &mouse_point);
    new_input.mouse.pos_last.x = new_input.mouse.pos.x;
    new_input.mouse.pos_last.y = new_input.mouse.pos.y;
    new_input.mouse.pos.x      = mouse_point.x;
    new_input.mouse.pos.y      = mouse_point.y;

    new_input.mouse.scroll = ms_scroll;

    // mouse buttons
    process_keyboard_message(new_input.mouse.buttons[0], ::GetKeyState(VK_LBUTTON) & (1 << 15));
    process_keyboard_message(new_input.mouse.buttons[1], ::GetKeyState(VK_RBUTTON) & (1 << 15));
    process_keyboard_message(new_input.mouse.buttons[2], ::GetKeyState(VK_MBUTTON) & (1 << 15));

    for (szt key = 0; key < ARRAY_COUNT(old_input.keyboard.keys); ++key) {
        if (old_input.keyboard.keys[key].half_transition_cnt > 0 &&
            old_input.keyboard.keys[key].ended_down == 0)
            old_input.keyboard.keys[key].half_transition_cnt = 0;
    }

    // keyboard
    process_keyboard_message(new_input.keyboard.enter, ::GetKeyState(keys::enter) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d1, ::GetKeyState(keys::d1) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d2, ::GetKeyState(keys::d2) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d3, ::GetKeyState(keys::d3) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d4, ::GetKeyState(keys::d4) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d5, ::GetKeyState(keys::d5) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d6, ::GetKeyState(keys::d6) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d7, ::GetKeyState(keys::d7) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d8, ::GetKeyState(keys::d8) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d9, ::GetKeyState(keys::d9) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d0, ::GetKeyState(keys::d0) & (1 << 15));
    process_keyboard_message(new_input.keyboard.space, ::GetKeyState(keys::space) & (1 << 15));
    process_keyboard_message(new_input.keyboard.left_shift,
                             ::GetKeyState(keys::left_shift) & (1 << 15));
    process_keyboard_message(new_input.keyboard.tab, ::GetKeyState(keys::tab) & (1 << 15));
    process_keyboard_message(new_input.keyboard.a, ::GetKeyState(keys::a) & (1 << 15));
    process_keyboard_message(new_input.keyboard.b, ::GetKeyState(keys::b) & (1 << 15));
    process_keyboard_message(new_input.keyboard.c, ::GetKeyState(keys::c) & (1 << 15));
    process_keyboard_message(new_input.keyboard.d, ::GetKeyState(keys::d) & (1 << 15));
    process_keyboard_message(new_input.keyboard.e, ::GetKeyState(keys::e) & (1 << 15));
    process_keyboard_message(new_input.keyboard.f, ::GetKeyState(keys::f) & (1 << 15));
    process_keyboard_message(new_input.keyboard.g, ::GetKeyState(keys::g) & (1 << 15));
    process_keyboard_message(new_input.keyboard.i, ::GetKeyState(keys::i) & (1 << 15));
    process_keyboard_message(new_input.keyboard.j, ::GetKeyState(keys::j) & (1 << 15));
    process_keyboard_message(new_input.keyboard.k, ::GetKeyState(keys::k) & (1 << 15));
    process_keyboard_message(new_input.keyboard.l, ::GetKeyState(keys::l) & (1 << 15));
    process_keyboard_message(new_input.keyboard.m, ::GetKeyState(keys::m) & (1 << 15));
    process_keyboard_message(new_input.keyboard.n, ::GetKeyState(keys::n) & (1 << 15));
    process_keyboard_message(new_input.keyboard.o, ::GetKeyState(keys::o) & (1 << 15));
    process_keyboard_message(new_input.keyboard.p, ::GetKeyState(keys::p) & (1 << 15));
    process_keyboard_message(new_input.keyboard.q, ::GetKeyState(keys::q) & (1 << 15));
    process_keyboard_message(new_input.keyboard.r, ::GetKeyState(keys::r) & (1 << 15));
    process_keyboard_message(new_input.keyboard.s, ::GetKeyState(keys::s) & (1 << 15));
    process_keyboard_message(new_input.keyboard.t, ::GetKeyState(keys::t) & (1 << 15));
    process_keyboard_message(new_input.keyboard.u, ::GetKeyState(keys::u) & (1 << 15));
    process_keyboard_message(new_input.keyboard.v, ::GetKeyState(keys::v) & (1 << 15));
    process_keyboard_message(new_input.keyboard.w, ::GetKeyState(keys::w) & (1 << 15));
    process_keyboard_message(new_input.keyboard.x, ::GetKeyState(keys::x) & (1 << 15));
    process_keyboard_message(new_input.keyboard.y, ::GetKeyState(keys::y) & (1 << 15));
    process_keyboard_message(new_input.keyboard.z, ::GetKeyState(keys::z) & (1 << 15));

    // controller
    // poll the input device
    s32 max_controller_count = XUSER_MAX_COUNT;
    if (max_controller_count > 4) {
        max_controller_count = 4;
    }

    for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; controller_index++) {
        controller &old_controller = old_input.controllers[controller_index];
        controller &new_controller = new_input.controllers[controller_index];

        XINPUT_STATE controller_state;
        if (XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS) {
            //! the controller is plugged in
            XINPUT_GAMEPAD &pad = controller_state.Gamepad;

            new_controller.is_connected = true;

            // NOTE: this is hardcoded for convenience
            // newController.isAnalog = oldController->isAnalog;
            new_controller.is_analog = true;

            //  no rmal stick input
            auto normalize = [](SHORT val) -> f32 {
                if (val < 0)
                    return scast(f32, val) / 32768.0f;
                else
                    return scast(f32, val) / 32767.0f;
            };

            f32 stick_left_x  = normalize(pad.sThumbLX);
            f32 stick_left_y  = normalize(pad.sThumbLY);
            f32 stick_right_x = normalize(pad.sThumbRX);
            f32 stick_right_y = normalize(pad.sThumbRY);

            new_controller.min_x = new_controller.max_x = new_controller.end_left_stick_x =
                stick_left_x;
            new_controller.min_y = new_controller.max_y = new_controller.end_left_stick_y =
                stick_left_y;

            for (szt button = 0; button < ARRAY_COUNT(old_controller.buttons); ++button) {
                if (!old_controller.buttons[button].ended_down)
                    old_controller.buttons[button].half_transition_cnt = 0;
            }

            process_Xinput_digital_button(pad.wButtons, old_controller.dpad_up,
                                          XINPUT_GAMEPAD_DPAD_UP, new_controller.dpad_up);
            process_Xinput_digital_button(pad.wButtons, old_controller.dpad_right,
                                          XINPUT_GAMEPAD_DPAD_RIGHT, new_controller.dpad_right);
            process_Xinput_digital_button(pad.wButtons, old_controller.dpad_down,
                                          XINPUT_GAMEPAD_DPAD_DOWN, new_controller.dpad_down);
            process_Xinput_digital_button(pad.wButtons, old_controller.dpad_left,
                                          XINPUT_GAMEPAD_DPAD_LEFT, new_controller.dpad_left);
            process_Xinput_digital_button(pad.wButtons, old_controller.a, XINPUT_GAMEPAD_A,
                                          new_controller.a);
            process_Xinput_digital_button(pad.wButtons, old_controller.b, XINPUT_GAMEPAD_B,
                                          new_controller.b);
            process_Xinput_digital_button(pad.wButtons, old_controller.x, XINPUT_GAMEPAD_X,
                                          new_controller.x);
            process_Xinput_digital_button(pad.wButtons, old_controller.y, XINPUT_GAMEPAD_Y,
                                          new_controller.y);
            process_Xinput_digital_button(pad.wButtons, old_controller.rb,
                                          XINPUT_GAMEPAD_RIGHT_SHOULDER, new_controller.rb);
            process_Xinput_digital_button(pad.wButtons, old_controller.lb,
                                          XINPUT_GAMEPAD_LEFT_SHOULDER, new_controller.lb);
            process_Xinput_digital_button(pad.wButtons, old_controller.back, XINPUT_GAMEPAD_BACK,
                                          new_controller.back);
            process_Xinput_digital_button(pad.wButtons, old_controller.start, XINPUT_GAMEPAD_START,
                                          new_controller.start);

            // NOTE: Not currently used
            // float dpadStickRX = pad.sThumbRX;
            // float dpadStickRY = pad.sThumbRY;
            // bool dPadStart = (pad->wButtons & XINPUT_GAMEPAD_START);
            // bool dPadBack = (pad->wButtons & XINPUT_GAMEPAD_BACK);
            // bool dPadRB = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            // bool dPadLB = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            // bool dPadR3 = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
            // bool dPadL3 = (pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
            // unsigned TCHAR dPadRT = pad->bRightTrigger;
            // unsigned TCHAR dPadLT = pad->bLeftTrigger;
        }
    }
}

internal void d3d_init(HWND hwnd, d3d::d3d_context *context)
{
    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

    ID3D11Device *base_device;
    ID3D11DeviceContext *base_device_context;
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                      feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &base_device,
                      nullptr, &base_device_context);

    ID3D11Device1 *device;
    base_device->QueryInterface(__uuidof(ID3D11Device1), rcast(void **, &device));
    ID3D11DeviceContext1 *device_context;
    base_device_context->QueryInterface(__uuidof(ID3D11DeviceContext1),
                                        rcast(void **, &device_context));
    context->device_context = device_context;
    context->device         = device;

    IDXGIDevice1 *dxgi_device;
    device->QueryInterface(__uuidof(IDXGIDevice1), rcast(void **, &dxgi_device));
    IDXGIAdapter *dxgi_adapter;
    dxgi_device->GetAdapter(&dxgi_adapter);
    IDXGIFactory2 *dxgi_factory;
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), rcast(void **, &dxgi_factory));

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    swap_chain_desc.Width              = 0;  // use window width
    swap_chain_desc.Height             = 0;  // use window height
    swap_chain_desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    swap_chain_desc.Stereo             = FALSE;
    swap_chain_desc.SampleDesc.Count   = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount        = 2;
    swap_chain_desc.Scaling            = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags              = 0;

    IDXGISwapChain1 *swap_chain;
    dxgi_factory->CreateSwapChainForHwnd(device, hwnd, &swap_chain_desc, nullptr, nullptr,
                                         &swap_chain);
    context->swap_chain = swap_chain;

    ID3D11Texture2D *frame_buf;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frame_buf));

    ID3D11RenderTargetView *frame_buf_view;
    device->CreateRenderTargetView(frame_buf, nullptr, &frame_buf_view);
    context->frame_buf_view = frame_buf_view;

    D3D11_TEXTURE2D_DESC depth_buf_desc;
    frame_buf->GetDesc(&depth_buf_desc);  // base on framebuffer properties

    depth_buf_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buf_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D *depth_buf;
    device->CreateTexture2D(&depth_buf_desc, nullptr, &depth_buf);

    ID3D11DepthStencilView *depth_buf_view;
    device->CreateDepthStencilView(depth_buf, nullptr, &depth_buf_view);
    context->depth_buf_view = depth_buf_view;

    D3D11_RASTERIZER_DESC1 rasterizer_desc = {};
    rasterizer_desc.FillMode               = D3D11_FILL_SOLID;
    rasterizer_desc.CullMode               = D3D11_CULL_BACK;

    ID3D11RasterizerState1 *rasterizer_state;
    device->CreateRasterizerState1(&rasterizer_desc, &rasterizer_state);
    context->rasterizer_state = rasterizer_state;

    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter             = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler_desc.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc     = D3D11_COMPARISON_NEVER;

    ID3D11SamplerState *sampler_state;
    device->CreateSamplerState(&sampler_desc, &sampler_state);
    context->sampler_state = sampler_state;

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
    depth_stencil_desc.DepthEnable              = TRUE;
    depth_stencil_desc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc                = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState *depth_stencil_state;
    device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
    context->depth_stencil_state = depth_stencil_state;
}

internal LARGE_INTEGER get_wall_clock()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time;
}

internal f32 get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    return scast(f32, end.QuadPart - start.QuadPart) / scast(f32, g_performance_counter_frequency);
}

internal void init_console()
{
    bool is_initialized = AllocConsole();
    TOM_ASSERT(is_initialized);

    if (is_initialized) {
        FILE *fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);

        HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);
        HANDLE hConIn  = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);
        SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
        SetStdHandle(STD_ERROR_HANDLE, hConOut);
        SetStdHandle(STD_INPUT_HANDLE, hConIn);
    }
}

internal void process_pending_messages(win32_state &state, game_input &input)
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT: running = false; break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 VKCode        = scast(u32, msg.wParam);
                bool was_down     = ((msg.lParam & (1 << 30)) != 0);
                bool is_down      = ((msg.lParam & (1 << 29)) == 0);
                bool alt_key_down = (msg.lParam & (1 << 29));
                if (was_down != is_down) {
                    switch (VKCode) {
                        case VK_ESCAPE: running = false; break;
                        case 'P': {
                            if (is_down) {
                                g_pause = !g_pause;
                            }
                        } break;

                        case (VK_RETURN): {
                            if (alt_key_down) {
                                toggle_fullscreen(msg.hwnd);
                            }
                        } break;
                        case (VK_F4): {
                            if (alt_key_down) {
                                running = false;
                            }
                        } break;
                        default: break;
                    }
                }
            } break;
            default: {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } break;
        }
    }
}

//==================================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

    LRESULT result = 0;
    switch (msg) {
        case WM_SETCURSOR: {
            if (g_debug_show_cursor) {
                result = DefWindowProcA(hwnd, msg, wparam, lparam);
            } else {
                SetCursor(0);
            }
        } break;
        case WM_SIZE: {
            g_win_dim = get_window_dimensions(hwnd);
            g_resize  = true;

            // ResizeDIBSection(g_backBuffer, g_winDims.width, g_winDims.height);
        } break;
        case WM_DESTROY: {
            running = false;
        } break;
        case WM_CLOSE: {
            running = false;
            PostQuitMessage(0);
        } break;
        case WM_ACTIVATEAPP: break;
        case WM_MOUSEWHEEL: {
            ms_scroll = GET_WHEEL_DELTA_WPARAM(wparam);
        } break;
        default:
            //            OutPutDebugStringA("default\n");
            result = DefWindowProc(hwnd, msg, wparam, lparam);
            break;
    }
    return result;
}

// ===============================================================================================
// #START
// ===============================================================================================

s32 win32_main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, s32 nShowCmd)
{
    // Init

    // WARN
    const TCHAR *icon_path = _T("T:\\data\\tomato.ico");
    auto icon              = (HICON)(LoadImage(NULL, icon_path, IMAGE_ICON, 0, 0,
                                               LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED));

    init_console();
    auto cons_hwnd = GetConsoleWindow();
    TOM_ASSERT(cons_hwnd);
    SendMessage(cons_hwnd, WM_SETICON, NULL, (LPARAM)icon);

    printf("Starting...\n");

    win32_state state = {};

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

#if _CPPUWIND
    printf("Exceptions are enabled!\n");
#endif

    LARGE_INTEGER performance_query_result;
    QueryPerformanceFrequency(&performance_query_result);
    g_performance_counter_frequency = performance_query_result.QuadPart;

    load_Xinput();

#ifdef TOM_INTERNAL
    g_debug_show_cursor = true;
#else
    g_debug_show_cursor = false;
#endif

    constexpr s32 win_width  = 1280;
    constexpr s32 win_height = 720;

    resize_dib_section(g_back_buffer, win_width, win_height);

    const TCHAR *cls_name = _T("TomatoWinCls");
    WNDCLASS cls          = { .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                              .lpfnWndProc   = WndProc,
                              .hInstance     = hInstance,
                              .hIcon         = icon,
                              .hCursor       = LoadCursor(NULL, IDC_ARROW),
                              .lpszClassName = cls_name };

    if (!RegisterClass(&cls)) {
        printf("ERROR--> Failed to register window class!\n");
        TOM_ASSERT(false);
        return 0;
    }

    DWORD dw_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    // DWORD dw_style = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;

    RECT wr = { .left = 0, .top = 0, .right = win_width + wr.left, .bottom = win_height + wr.top };

    if (AdjustWindowRect(&wr, dw_style, false) == 0) {
        printf("ERROR--> Failed to adjust window rect");
        TOM_ASSERT(false);
    }

    HWND hwnd = CreateWindowEx(0, cls.lpszClassName, _T("TomatoGame"), dw_style, CW_USEDEFAULT,
                               CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL,
                               hInstance, NULL);

    if (!hwnd) {
        printf("Failed to create window!\n");
        TOM_ASSERT(hwnd);
        return 0;
    }

    ::ShowWindow(hwnd, SW_SHOWNORMAL);

    HRESULT hr;
    hr = GetLastError();

    // BOOL fOK;
    // TCHAR msgBuf[128];
    // fOK = FormatMessage(
    //     FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
    //     FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, 0, (PTSTR)&msgBuf, 0, NULL);
    // // if (!fOK) msgBuf = _T("Failed to format Message!");
    // _tprintf(TEXT("%d\t%s\n"), hr, msgBuf);
    // LocalFree(msgBuf);

    // NOTE: Set the windows schedule granularity to 1ms
    // so sleep will be more granular
    UINT desired_scheduler_MS = 1;
    b32 is_sleep_granular     = (timeBeginPeriod(desired_scheduler_MS) == TIMERR_NOERROR);
    is_sleep_granular         = false;

    HDC hdc                       = GetDC(hwnd);
    running                       = true;
    g_pause                       = false;
    g_back_buffer.bytes_per_pixel = 4;

    s32 monitor_refresh_rate = GetDeviceCaps(hdc, VREFRESH);
    printf("Monitor Refresh Rate: %d\n", monitor_refresh_rate);

    sound_output sound_output = {
        .samples_per_sec      = 48000,
        .bytes_per_sample     = sizeof(s16) * 2,
        .secondary_buf_size   = scast(DWORD, sound_output.samples_per_sec),
        .latency_sample_count = scast(
            s32, g_frames_of_audio_latency *
                     scast(f32, (sound_output.samples_per_sec / scast(f32, g_game_update_hertz))))
    };

    init_WASAPI(sound_output.samples_per_sec, sound_output.secondary_buf_size);
    g_audio_client->Start();

    s16 *samples = scast(s16 *, VirtualAlloc(0, sound_output.secondary_buf_size,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

#ifdef TOM_INTERNAL
    LPVOID base_address = rcast(LPVOID, TERABYTES(scast(u64, 2)));
#else
    LPVOID base_address = 0;
#endif

    game_memory memory                        = {};
    memory.permanent_storage_size             = MEGABYTES(256);
    memory.transient_storage_size             = GIGABYTES(1);
    memory.plat_io.platform_free_file_memory  = free_file_memory;
    memory.plat_io.platform_read_entire_file  = read_entire_file;
    memory.plat_io.platform_write_entire_file = write_entire_file;
    memory.win_dims.width                     = win_width;
    memory.win_dims.height                    = win_height;

    SetCursorPos(win_width / 2, win_height / 2);

    state.total_size = memory.permanent_storage_size + memory.transient_storage_size;
    // TODO: use large pages
    state.game_memory_block =
        VirtualAlloc(base_address, state.total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    memory.permanent_storage = state.game_memory_block;

    memory.transient_storage =
        (scast(u8 *, memory.permanent_storage) + memory.permanent_storage_size);

    d3d_init(hwnd, &memory.d3d_context);

    game_input input[2]   = {};
    game_input *new_input = &input[0];
    game_input *old_input = &input[1];

    LARGE_INTEGER last_counter = get_wall_clock();
    u64 last_cycle_count       = __rdtsc();

    bool is_sound_valid      = true;
    bool is_game_code_loaded = true;
    DWORD last_play_cursor   = {};
    DWORD last_write_cursor  = {};

    f32 delta_time = 0.0f;
    // NOTE: dummy thread context, for now
    thread_context thread {};
    init(&thread, &memory);

    // ImGui
    //  TODO: where to put this?
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(memory.d3d_context.device, memory.d3d_context.device_context);
    set_ImGui_style();

    // ===========================================================================================
    // #MAIN LOOP
    // ===========================================================================================
    while (running) {
        memory.win_dims = g_win_dim;

        if (g_resize) {
            memory.win_dims.width  = g_win_dim.width;
            memory.win_dims.height = g_win_dim.height;
            memory.win_resize      = true;
            g_resize               = false;
        }

        ms_scroll = 0;
        process_pending_messages(state, *new_input);
        do_controller_input(*old_input, *new_input, hwnd);
        // NOTE: this isn't calculated and needs to be for a varaible framerate
        delta_time = g_target_frames_per_second;

        // NOTE: temp program exit from controller
        if (new_input->controllers->back.ended_down) {
            running = false;
        }

        // #Sound

        REFERENCE_TIME latency;
        if (SUCCEEDED(g_audio_client->GetStreamLatency(&latency))) {
        } else {
            printf("ERROR--> Failed to get audio latency\n");
        }

        s32 samples_to_write;
        UINT32 sound_pad_size;
        if (SUCCEEDED(g_audio_client->GetCurrentPadding(&sound_pad_size))) {
            s32 maxSampleCnt = scast(s32, sound_output.secondary_buf_size - sound_pad_size);
            samples_to_write = scast(s32, sound_output.latency_sample_count - sound_pad_size);
            if (samples_to_write < 0) samples_to_write = 0;
            // TOM_ASSERT(samplesToWrite < maxSampleCnt);
        }

        sound_output_buffer sound_buffer = { .samples_per_second = sound_output.samples_per_sec,
                                             .sample_count       = samples_to_write,
                                             .samples            = samples };

        offscreen_buffer buffer = { .memory          = g_back_buffer.memory,
                                    .width           = g_back_buffer.width,
                                    .height          = g_back_buffer.height,
                                    .pitch           = g_back_buffer.pitch,
                                    .bytes_per_pixel = 4 };

        update(&thread, &memory, *input, delta_time);

        fill_sound_buffer(sound_output, samples_to_write, sound_buffer);

        // clock stuffs
        auto work_counter        = get_wall_clock();
        f32 work_seconds_elapsed = get_seconds_elapsed(last_counter, work_counter);

        f32 seconds_elapsed_for_frame = work_seconds_elapsed;
        if (seconds_elapsed_for_frame < g_target_frames_per_second) {
            if (is_sleep_granular) {
                auto sleepMs =
                    scast(DWORD, 1000.f * (g_target_frames_per_second - seconds_elapsed_for_frame));
                if (sleepMs > 0) {
                    ::Sleep(sleepMs);
                }
            }
            f32 test_seconds_elapsed_for_frame =
                get_seconds_elapsed(last_counter, get_wall_clock());
            while (seconds_elapsed_for_frame < g_target_frames_per_second) {
                seconds_elapsed_for_frame = get_seconds_elapsed(last_counter, get_wall_clock());
            }
        } else {
            printf("WARNING--> missed frame timing!!!\n");
        }

        auto end_counter = get_wall_clock();
        f32 ms_per_frame = 1000.f * get_seconds_elapsed(last_counter, end_counter);
        // printf("%f\n", ms_per_frame);

        last_counter = end_counter;

        display_buffer_in_window(hdc, g_back_buffer, 0, 0, g_win_dim.width, g_win_dim.height);

        DWORD play_cursor;
        DWORD write_cursor;

        UINT64 frequency_position;
        UINT64 units_posisition;

        g_audio_clock->GetFrequency(&frequency_position);
        g_audio_clock->GetPosition(&units_posisition, 0);

        play_cursor =
            scast(DWORD, sound_output.samples_per_sec * units_posisition / frequency_position) %
            sound_output.samples_per_sec;
        write_cursor =
            scast(DWORD, sound_output.samples_per_sec * units_posisition / frequency_position) %
                sound_output.samples_per_sec +
            sound_pad_size;
        if (write_cursor > sound_output.secondary_buf_size) {
            write_cursor -= sound_output.secondary_buf_size;
        }

        game_input *temp_input = new_input;
        new_input              = old_input;
        old_input              = temp_input;

        u64 end_cycle_count = __rdtsc();
        u64 cycles_elapsed  = end_cycle_count - last_cycle_count;
        last_cycle_count    = end_cycle_count;
    }

    // clean up
    exit(&thread, &memory);

    ImGui_ImplDX11_Shutdown();

    ImGui::DestroyContext();
    // ImGui_ImplWin32_Shutdown();

    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(cls_name, NULL);

    return 0;
}

}  // namespace win32
}  // namespace tom

//========================================================================================
// ENTRY POINT
//========================================================================================

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement                = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr)) return 1;

    s32 ecode = tom::win32::win32_main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    CoUninitialize();

    return ecode;
}