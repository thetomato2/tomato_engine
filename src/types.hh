#ifndef TOMATO_TYPES_HH_
#define TOMATO_TYPES_HH_

#include <cstdint>

namespace tom
{

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using szt = size_t;
using byt = unsigned char;
// using byt = u8; // TODO: is this different?

using b32 = s32;

using wchar = wchar_t;
using ul    = unsigned long;
using ull   = unsigned long long;

#define U8_MIN 0u
#define U8_MAX 0xffu
#define S8_MIN (-0x7f - 1)
#define S8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define S16_MIN (-0x7fff - 1)
#define S16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define S32_MIN (-0x7fffffff - 1)
#define S32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define S64_MIN (-0x7fffffffffffffffll - 1)
#define S64_MAX 0x7fffffffffffffffll

#define BIT(x) (1 << x)

#define KILOBYTES(val) ((val)*1024)
#define MEGABYTES(val) (KILOBYTES(val) * 1024)
#define GIGABYTES(val) (MEGABYTES(val) * 1024)
#define TERABYTES(val) (GIGABYTES(val) * 1024)

union v2
{
    struct
    {
        f32 x, y;
    };
    struct
    {
        f32 u, v;
    };
    f32 e[2];
};

union v3
{
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 r, g, b;
    };
    struct
    {
        v2 xy;
        f32 _ignored0;
    };
    f32 e[3];
};

union v4
{
    struct
    {
        f32 x, y, z, w;
    };
    struct
    {
        f32 r, g, b, a;
    };
    struct
    {
        v3 xyz;
        f32 _ignored0;
    };
    struct
    {
        v3 rgb;
        f32 _ignored1;
    };
    f32 e[4];
};

union m4
{
    f32 e[16];
    f32 m[4][4];
    v4 r[4];
};

struct rect2
{
    v2 min;
    v2 max;
};

struct rect3
{
    v3 min;
    v3 max;
};

// foward declares
class string;
class wstring;

}  // namespace tom

#endif  // !TOMATO_TYPES_HH_