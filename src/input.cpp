#include "input.hpp"

namespace tom
{

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

internal void process_keyboard_message(key &new_state, const b32 is_down)
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

void do_input(input *old_input, input *new_input, HWND hwnd, s32 ms_scroll)
{
    // mouse cursor
    POINT mouse_point;
    GetCursorPos(&mouse_point);
    ScreenToClient(hwnd, &mouse_point);
    new_input->mouse.pos_last.x = new_input->mouse.pos.x;
    new_input->mouse.pos_last.y = new_input->mouse.pos.y;
    new_input->mouse.pos.x      = mouse_point.x;
    new_input->mouse.pos.y      = mouse_point.y;

    new_input->mouse.scroll = ms_scroll;

    // mouse buttons
    process_keyboard_message(new_input->mouse.buttons[0], ::GetKeyState(VK_LBUTTON) & (1 << 15));
    process_keyboard_message(new_input->mouse.buttons[1], ::GetKeyState(VK_RBUTTON) & (1 << 15));
    process_keyboard_message(new_input->mouse.buttons[2], ::GetKeyState(VK_MBUTTON) & (1 << 15));

    for (szt key = 0; key < ARRAY_COUNT(old_input->keyboard.keys); ++key) {
        if (old_input->keyboard.keys[key].half_transition_cnt > 0 &&
            old_input->keyboard.keys[key].ended_down == 0)
            old_input->keyboard.keys[key].half_transition_cnt = 0;
    }

    // keyboard
    process_keyboard_message(new_input->keyboard.enter, ::GetKeyState(keys::enter) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d1, ::GetKeyState(keys::d1) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d2, ::GetKeyState(keys::d2) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d3, ::GetKeyState(keys::d3) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d4, ::GetKeyState(keys::d4) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d5, ::GetKeyState(keys::d5) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d6, ::GetKeyState(keys::d6) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d7, ::GetKeyState(keys::d7) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d8, ::GetKeyState(keys::d8) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d9, ::GetKeyState(keys::d9) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d0, ::GetKeyState(keys::d0) & (1 << 15));
    process_keyboard_message(new_input->keyboard.space, ::GetKeyState(keys::space) & (1 << 15));
    process_keyboard_message(new_input->keyboard.left_shift,
                             ::GetKeyState(keys::left_shift) & (1 << 15));
    process_keyboard_message(new_input->keyboard.tab, ::GetKeyState(keys::tab) & (1 << 15));
    process_keyboard_message(new_input->keyboard.a, ::GetKeyState(keys::a) & (1 << 15));
    process_keyboard_message(new_input->keyboard.b, ::GetKeyState(keys::b) & (1 << 15));
    process_keyboard_message(new_input->keyboard.c, ::GetKeyState(keys::c) & (1 << 15));
    process_keyboard_message(new_input->keyboard.d, ::GetKeyState(keys::d) & (1 << 15));
    process_keyboard_message(new_input->keyboard.e, ::GetKeyState(keys::e) & (1 << 15));
    process_keyboard_message(new_input->keyboard.f, ::GetKeyState(keys::f) & (1 << 15));
    process_keyboard_message(new_input->keyboard.g, ::GetKeyState(keys::g) & (1 << 15));
    process_keyboard_message(new_input->keyboard.i, ::GetKeyState(keys::i) & (1 << 15));
    process_keyboard_message(new_input->keyboard.j, ::GetKeyState(keys::j) & (1 << 15));
    process_keyboard_message(new_input->keyboard.k, ::GetKeyState(keys::k) & (1 << 15));
    process_keyboard_message(new_input->keyboard.l, ::GetKeyState(keys::l) & (1 << 15));
    process_keyboard_message(new_input->keyboard.m, ::GetKeyState(keys::m) & (1 << 15));
    process_keyboard_message(new_input->keyboard.n, ::GetKeyState(keys::n) & (1 << 15));
    process_keyboard_message(new_input->keyboard.o, ::GetKeyState(keys::o) & (1 << 15));
    process_keyboard_message(new_input->keyboard.p, ::GetKeyState(keys::p) & (1 << 15));
    process_keyboard_message(new_input->keyboard.q, ::GetKeyState(keys::q) & (1 << 15));
    process_keyboard_message(new_input->keyboard.r, ::GetKeyState(keys::r) & (1 << 15));
    process_keyboard_message(new_input->keyboard.s, ::GetKeyState(keys::s) & (1 << 15));
    process_keyboard_message(new_input->keyboard.t, ::GetKeyState(keys::t) & (1 << 15));
    process_keyboard_message(new_input->keyboard.u, ::GetKeyState(keys::u) & (1 << 15));
    process_keyboard_message(new_input->keyboard.v, ::GetKeyState(keys::v) & (1 << 15));
    process_keyboard_message(new_input->keyboard.w, ::GetKeyState(keys::w) & (1 << 15));
    process_keyboard_message(new_input->keyboard.x, ::GetKeyState(keys::x) & (1 << 15));
    process_keyboard_message(new_input->keyboard.y, ::GetKeyState(keys::y) & (1 << 15));
    process_keyboard_message(new_input->keyboard.z, ::GetKeyState(keys::z) & (1 << 15));

#if CONTROLLER_ENABLED
    // controller
    // poll the input device
    s32 max_controller_count = XUSER_MAX_COUNT;
    if (max_controller_count > 4) {
        max_controller_count = 4;
    }

    for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; controller_index++) {
        controller &old_controller = old_input->controllers[controller_index];
        controller &new_controller = new_input->controllers[controller_index];

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
#endif
}

}  // namespace tom