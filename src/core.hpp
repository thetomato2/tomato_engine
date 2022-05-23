#ifndef TOMATO_COMMON_HPP_
#define TOMATO_COMMON_HPP_

#define USE_CRT 1  // use the c runtime lib

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#ifndef NOMINMAX
    #define NOMINMAX
#endif
// TODO: use this, right now there is some sleep fuction in the platform layer this turns off
// #define WIN32_LEAN_AND_MEAN 0
#include <windows.h>
#include <tchar.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "directx.hpp"

// NOTE: for grep purposes
#define scast(t, v) static_cast<t>(v)
#define rcast(t, v) reinterpret_cast<t>(v)
#define ccast(t, v) const_cast<t>(v)
#define cast(t, v)  ((t)(v))

#ifdef _MSVC
    #define MSVC 1
#endif

#ifdef _LLVM
    #define LLVM 1
#endif

#if MSVC
    /* #include <intrin.h> */
    #pragma intrinsic(_BitScanForward)
#endif

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

#ifdef _WIN32
using whcar = wchar_t;
using ul    = unsigned long;
using ull   = unsigned long long;
#endif

static_assert(sizeof(s8) == 1, "s8 isn't 1 byte!");
static_assert(sizeof(s16) == 2, "s16 isn't 2 byte!s");
static_assert(sizeof(s32) == 4, "s32 isn't 4 byte!s");
static_assert(sizeof(s64) == 8, "s64 isn't 8 byte!s");
static_assert(sizeof(u8) == 1, "u8 isn't 1 byte!");
static_assert(sizeof(u16) == 2, "u16 isn't 2 byte!s");
static_assert(sizeof(u32) == 4, "u32 isn't 4 byte!s");
static_assert(sizeof(u64) == 8, "u64 isn't 8 byte!s");
static_assert(sizeof(f32) == 4, "f32 isn't 4 byte!s");
static_assert(sizeof(f64) == 8, "f64 isn't 8 byte!s");
static_assert(sizeof(b32) == 4, "b32 isn't 4 byte!s");

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

#if 1
template<typename T, size_t N>
byt (&ArrayCountHelper(T (&)[N]))[N];
    #define ARRAY_COUNT(arr) (sizeof(ArrayCountHelper(arr)))
#else
    #define ARRAY_COUNT(array) (sizeof((array)) / sizeof((array)[0]))
#endif

#define internal static
#define global   static
#define local    static
#define function static

#define Z_UP 1

#ifdef TOM_WIN32
    #define TOM_DLL_EXPORT __declspec(dllexport)
#else
    #define TOM_DLL_EXPORT
#endif

#ifndef TOM_INTERNAL
    #define TOM_INTERNAL
#endif

#ifdef TOM_INTERNAL
    #define TOM_ASSERT(x)                                               \
        if (!(x)) {                                                     \
            printf("FAILED ASSERT -> %s at :%d\n", __FILE__, __LINE__); \
            __debugbreak();                                             \
        }                                                               \
        assert(x)

    #define TOM_ASSERT_MSG(x, msg)                                                \
        if (!(x)) {                                                               \
            printf("FAILED ASSERT -> %s at :%d - %s\n", __FILE__, __LINE__, msg); \
            __debugbreak();                                                       \
        }                                                                         \
        assert(x)

    #define DEBUG_BREAK(x)  \
        if (x) {            \
            __debugbreak(); \
        }
#else
    #define TOM_ASSERT(x)
    #define TOM_ASSERT_MSG(x, msg)
    #define DEBUG_BREAK(x)
#endif

#define INVALID_CODE_PATH TOM_ASSERT(!"Invalid code path!")

#include "math.hpp"
#include "color.hpp"

namespace tom
{

struct thread_context
{
    s32 place_holder;
};

struct window_dims
{
    s32 width;
    s32 height;
};

struct memory_arena
{
    szt size;
    u8 *base;
    szt used;
};

inline void init_arena(memory_arena *arena, const szt size, void *base)
{
    arena->size = size;
    arena->base = scast(byt *, base);
    arena->used = 0;
}

inline void *push_size(memory_arena *arena, szt size)
{
    TOM_ASSERT((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return result;
}

inline void zero_size(void *ptr, szt size)
{
#if USE_CRT
    memset(ptr, 0, size);
#else
    // TODO: profile this for performance
    byt *byte = scast(byt *, ptr);
    while (size--) {
        *byte++ = 0;
    }
#endif
}

#define PUSH_STRUCT(arena, type)       (type *)push_size(arena, sizeof(type))
#define PUSH_ARRAY(arena, count, type) (type *)push_size(arena, (count * sizeof(type)))
#define ZERO_STRUCT(inst)              zero_size(&(inst), sizeof(inst))

// Generic flag stuff

inline bool is_flag_set(s32 flags, s32 flag)
{
    return flags & flag;
}

inline void set_flags(s32 &flags, s32 flag)
{
    flags |= flag;
}

inline void clear_flags(s32 &flags, s32 flag)
{
    flags &= ~flag;
}

inline u32 safe_truncate_u32_to_u64(u64 value)
{
    TOM_ASSERT(value <= U32_MAX);
    u32 result = scast(u32, value);
    return result;
}

struct read_file_result
{
    szt content_size;
    void *contents;
};

using platform_free_file_memory  = void (*)(thread_context *, void *);
using platform_read_entire_file  = read_file_result (*)(thread_context *, const char *);
using platform_write_entire_file = b32 (*)(thread_context *, const char *, u64, void *);

struct platform_io
{
    platform_free_file_memory platform_free_file_memory;
    platform_read_entire_file platform_read_entire_file;
    platform_write_entire_file platform_write_entire_file;
};

}  // namespace tom
#endif  // TOMATO_COMMON_HPP_
