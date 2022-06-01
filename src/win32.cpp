#include "win32.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

namespace tom
{

// TODO: get rid of these??
global bool g_running = true;
global bool g_pause   = false;
global bool g_resize  = false;
window_dims g_win_dim = {};
s32 g_ms_scroll       = {};

internal void toggle_fullscreen(win32_state *state)
{
    DWORD dwStyle = scast(DWORD, GetWindowLong(state->hwnd, GWL_STYLE));
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(state->hwnd, &state->win_pos) &&
            GetMonitorInfo(MonitorFromWindow(state->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(state->hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(state->hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(state->hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);

        SetWindowPlacement(state->hwnd, &state->win_pos);
        SetWindowPos(state->hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
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

void create_console()
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

void process_pending_messages(win32_state *state)
{
    state->running   = g_running;
    state->pause     = g_pause;
    state->resize    = g_resize;
    state->win_dims  = g_win_dim;
    state->ms_scroll = g_ms_scroll;

    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT: {
                g_running      = false;
                state->running = false;
            } break;
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
                        case VK_ESCAPE: {
                            g_running      = false;
                            state->running = false;
                        } break;
                        case 'P': {
                            if (is_down) {
                                g_pause      = !g_pause;
                                state->pause = g_pause;
                            }
                        } break;

                        case (VK_RETURN): {
                            if (alt_key_down) {
                                toggle_fullscreen(state);
                            }
                        } break;
                        case (VK_F4): {
                            if (alt_key_down) {
                                g_running      = false;
                                state->running = false;
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
            if (true) {
                result = DefWindowProcA(hwnd, msg, wparam, lparam);
            } else {
                SetCursor(0);
            }
        } break;
        case WM_SIZE: {
            g_win_dim = get_window_dimensions(hwnd);
            g_resize  = true;
        } break;
        case WM_DESTROY: {
            g_running = false;
        } break;
        case WM_CLOSE: {
            g_running = false;
            PostQuitMessage(0);
        } break;
        case WM_ACTIVATEAPP: break;
        case WM_MOUSEWHEEL: {
            g_ms_scroll = GET_WHEEL_DELTA_WPARAM(wparam);
        } break;
        default:
            //            OutPutDebugStringA("default\n");
            result = DefWindowProc(hwnd, msg, wparam, lparam);
            break;
    }
    return result;
}

void create_window(win32_state *state)
{
    state->cls_name = _T("TomatoWinCls");
    WNDCLASS cls    = { .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                        .lpfnWndProc   = WndProc,
                        .hInstance     = state->hinst,
                        .hIcon         = state->icon,
                        .hCursor       = LoadCursor(NULL, IDC_ARROW),
                        .lpszClassName = state->cls_name };

    if (!RegisterClass(&cls)) {
        printf("ERROR--> Failed to register window class!\n");
        TOM_ASSERT(false);
        return;
    }

    DWORD dw_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    // DWORD dw_style = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;

    RECT wr = { .left   = 0,
                .top    = 0,
                .right  = state->win_dims.width + wr.left,
                .bottom = state->win_dims.height + wr.top };

    if (AdjustWindowRect(&wr, dw_style, false) == 0) {
        printf("ERROR--> Failed to adjust window rect");
        TOM_ASSERT(false);
    }

    state->hwnd = CreateWindowEx(0, cls.lpszClassName, _T("TomatoGame"), dw_style, CW_USEDEFAULT,
                                 CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL,
                                 state->hinst, NULL);

    if (!state->hwnd) {
        printf("Failed to create window!\n");
        TOM_ASSERT(state->hwnd);
        return;
    }

    ShowWindow(state->hwnd, SW_SHOWNORMAL);

    HRESULT hr;
    hr = GetLastError();

    UINT desired_scheduler_MS = 1;
    b32 is_sleep_granular     = (timeBeginPeriod(desired_scheduler_MS) == TIMERR_NOERROR);
    is_sleep_granular         = false;

    HDC hdc = GetDC(state->hwnd);

    s32 monitor_refresh_rate = GetDeviceCaps(hdc, VREFRESH);
    printf("Monitor Refresh Rate: %d\n", monitor_refresh_rate);
}

}  // namespace tom
