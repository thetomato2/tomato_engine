#ifndef TOMATO_INPUT_HPP_
#define TOMATO_INPUT_HPP_
#include "core.hpp"

namespace tom
{

// TODO: why do I have these globals?
global f64 g_mouse_x;
global f64 g_mouse_y;

global f64 g_scroll_x_off;
global f64 g_scroll_y_off;

enum keys : byt
{
    none           = 0,
    back           = 0x8,
    tab            = 0x9,
    enter          = 0xd,
    pause          = 0x13,
    caps_lock      = 0x14,
    kana           = 0x15,
    kanji          = 0x19,
    escape         = 0x1b,
    ime_convert    = 0x1c,
    ime_no_convert = 0x1d,
    space          = 0x20,
    pageUp         = 0x21,
    pageDown       = 0x22,
    end            = 0x23,
    home           = 0x24,
    left           = 0x25,
    up             = 0x26,
    right          = 0x27,
    down           = 0x28,
    // select         = 0x29,
    print          = 0x2a,
    execute        = 0x2b,
    print_screen   = 0x2c,
    insert         = 0x2d,
    delete_key     = 0x2e,
    help           = 0x2f,
    d0             = 0x30,
    d1             = 0x31,
    d2             = 0x32,
    d3             = 0x33,
    d4             = 0x34,
    d5             = 0x35,
    d6             = 0x36,
    d7             = 0x37,
    d8             = 0x38,
    d9             = 0x39,
    a              = 0x41,
    b              = 0x42,
    c              = 0x43,
    d              = 0x44,
    e              = 0x45,
    f              = 0x46,
    g              = 0x47,
    h              = 0x48,
    i              = 0x49,
    j              = 0x4a,
    k              = 0x4b,
    l              = 0x4c,
    m              = 0x4d,
    n              = 0x4e,
    o              = 0x4f,
    p              = 0x50,
    q              = 0x51,
    r              = 0x52,
    s              = 0x53,
    t              = 0x54,
    u              = 0x55,
    v              = 0x56,
    w              = 0x57,
    x              = 0x58,
    y              = 0x59,
    z              = 0x5a,
    left_windows   = 0x5b,
    right_windows  = 0x5c,
    apps           = 0x5d,
    sleep          = 0x5f,
    num_pad_0      = 0x60,
    num_pad_1      = 0x61,
    num_pad_2      = 0x62,
    num_pad_3      = 0x63,
    num_pad_4      = 0x64,
    num_pad_5      = 0x65,
    num_pad_6      = 0x66,
    num_pad_7      = 0x67,
    num_pad_8      = 0x68,
    num_pad_9      = 0x69,
    multiply       = 0x6a,
    add            = 0x6b,
    separator      = 0x6c,
    subtract       = 0x6d,
    decimal        = 0x6e,
    divide         = 0x6f,
    f1             = 0x70,
    f2             = 0x71,
    f3             = 0x72,
    f4             = 0x73,
    f5             = 0x74,
    f6             = 0x75,
    f7             = 0x76,
    f8             = 0x77,
    f9             = 0x78,
    f10            = 0x79,
    f11            = 0x7a,
    f12            = 0x7b,
    f13            = 0x7c,
    f14            = 0x7d,
    f15            = 0x7e,
    f16            = 0x7f,
    f17            = 0x80,
    f18            = 0x81,
    f19            = 0x82,
    f20            = 0x83,
    f21            = 0x84,
    f22            = 0x85,
    f23            = 0x86,
    f24            = 0x87,
    num_lock       = 0x90,
    scroll         = 0x91,
    left_shift     = 0xa0,
    right_shift    = 0xa1,
    left_control   = 0xa2,
    right_control  = 0xa3,
    left_alt       = 0xa4,
    right_alt      = 0xa5,
    semicolon      = 0xba,
    plus           = 0xbb,
    comma          = 0xbc,
    minus          = 0xbd,
    period         = 0xbe,
    question       = 0xbf,
    tilde          = 0xc0,
    open_brackets  = 0xdb,
    pipe           = 0xdc,
    close_brackets = 0xdd,
    quotes         = 0xde,
    oem8           = 0xdf,
    backslash      = 0xe2,
    process_key    = 0xe5,
};

struct button
{
    s32 half_transition_cnt;
    b32 ended_down;
};

typedef button key;

struct mouse
{
    global constexpr szt but_cnt = 3;
    v2 pos;
    v2 pos_last;
    union
    {
        button buttons[but_cnt];
        struct
        {
            button l, r, m;
        };
    };

    s32 scroll;
    v2 get_delta() const { return pos - pos_last; }
};

struct keyboard
{
    global constexpr szt key_cnt = 39;
    union
    {
        key keys[key_cnt];
        struct
        {
            key d1;
            key d2;
            key d3;
            key d4;
            key d5;
            key d6;
            key d7;
            key d8;
            key d9;
            key d0;
            key enter;
            key space;
            key left_shift;
            key tab;
            key a;
            key b;
            key c;
            key d;
            key e;
            key f;
            key g;
            key i;
            key j;
            key k;
            key l;
            key m;
            key n;
            key o;
            key p;
            key q;
            key r;
            key s;
            key t;
            key u;
            key v;
            key w;
            key x;
            key y;
            key z;
        };
    };
};

struct controller
{
    b32 is_connected;
    b32 is_analog;

    f32 min_x;
    f32 min_y;

    f32 max_x;
    f32 max_y;

    f32 start_left_stick_x;
    f32 start_left_stick_y;

    f32 start_right_stick_y;
    f32 start_right_stick_x;

    f32 end_left_stick_x;
    f32 end_left_stick_y;

    f32 end_right_stick_x;
    f32 end_right_stick_y;

    union
    {
        button buttons[12];
        struct
        {
            button dpad_up;
            button dpad_right;
            button dpad_down;
            button dpad_left;
            button a;
            button b;
            button x;
            button y;
            button rb;
            button lb;
            button back;
            button start;
        };
    };
};

struct input
{
    global constexpr szt controller_cnt = 4;
    mouse mouse;
    keyboard keyboard;
    controller controllers[controller_cnt];
};

inline bool key_up(const key k)
{
    return k.half_transition_cnt > 0 && k.ended_down == 0;
}

inline bool key_pressed(const key k)
{
    return k.half_transition_cnt == 1;
}

inline bool key_down(const key k)
{
    return k.ended_down;
}

void do_input(input *new_input, HWND hwnd, s32 ms_scroll);

}  // namespace tom

#endif