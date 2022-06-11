#ifndef WIN32_PLATFORM_HH_
#define WIN32_PLATFORM_HH_

#include "core.hh"

namespace tom
{

struct win32_state
{
    bool running;
    bool pause;
    bool resize;
    window_dims win_dims;
    s32 ms_scroll;
    WINDOWPLACEMENT win_pos = { sizeof(win_pos) };
    HWND hwnd;
    HINSTANCE hinst;
    HDC hdc;
    const TCHAR *cls_name;
    HICON icon;
};

bool dir_exists(const char *dir);
void create_console();
void create_window(win32_state *state);
void process_pending_messages(win32_state *state);

}  // namespace tom
#endif