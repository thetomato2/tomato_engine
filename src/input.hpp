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

inline bool is_button_up(const button b)
{
    return b.half_transition_cnt > 0 && b.ended_down == 0;
}

inline bool is_key_up(const key k)
{
    return k.half_transition_cnt > 0 && k.ended_down == 0;
}

inline bool is_button_down(const button b)
{
    return b.ended_down;
}

inline bool is_key_down(const key k)
{
    return k.ended_down;
}

}  // namespace tom

#endif