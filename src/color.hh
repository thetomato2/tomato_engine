#ifndef COLOR_HH_
#define COLOR_HH_

#include "core.hh"

namespace tom
{
struct color
{
    union
    {
        struct
        {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        };
        u32 rgba;
    };
};

inline color v3_to_color(v3 col, f32 a = 1.0f)
{
    color res;

    res.r = (u8)(col.r * 255.0f);
    res.g = (u8)(col.g * 255.0f);
    res.b = (u8)(col.b * 255.0f);
    res.a = (u8)(a * 255.0f);

    return res;
}

inline color f32_to_color(f32 col, f32 a = 1.0f)
{
    color res;

    res.r = (u8)(col * 255.0f);
    res.g = (u8)(col * 255.0f);
    res.b = (u8)(col * 255.0f);
    res.a = (u8)(a * 255.0f);

    return res;
}

namespace colors
{
// note: argb
global constexpr color red   = { 0xff'00'00'ff };
global constexpr color green = { 0x00'ff'00'ff };
global constexpr color blue  = { 0x00'00'ff'ff };
global constexpr color pink  = { 0xff'00'ff'ff };
global constexpr color white = { 0xff'ff'ff'ff };
global constexpr color black = { 0x00'00'00'ff };
}  // namespace colors
}  // namespace tom

#endif  // COLOR_HPP_
