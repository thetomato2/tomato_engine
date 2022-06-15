#ifndef TOMATO_COMMON_HH_
#define TOMATO_COMMON_HH_

#define STL_LIB 1

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#ifndef NOMINMAX
    #define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN 0
#include <windows.h>
#include <tchar.h>

#include <xinput.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

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

#include "types.hh"

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
    #define INTERNAL_ONLY_EXECUTE(args) args
#else
    #define TOM_ASSERT(x)
    #define TOM_ASSERT_MSG(x, msg)
    #define DEBUG_BREAK(x)
    #define INTERNAL_ONLY_EXECUTE(args)
#endif

// FIXME: there is no EXPP_TEXT
#define EXPP_HRESULT(hr, what) EXPP_EXCEPTION(HRESULT, (HRESULT)hr, EXPP_TEXT(what))
#define EVALUATE_HRESULT(call, what)      \
    {                                     \
        HRESULT hr;                       \
        if (FAILED(hr = call)) {          \
            throw EXPP_HRESULT(hr, what); \
        }                                 \
    }
#define EVALUATE_HRESULT_HR(hr, call, what) \
    if (FAILED(hr = call)) {                \
        throw EXPP_HRESULT(hr, what);       \
    }

#define INVALID_CODE_PATH TOM_ASSERT(!"Invalid code path!")

#include "math.hh"
#include "color.hh"
#include "memory.hh"

namespace tom
{

#if __cplusplus
template<typename T, size_t N>
byt (&ArrayCountHelper(T (&)[N]))[N];
    #define ARRAY_COUNT(arr) (sizeof(ArrayCountHelper(arr)))
#else
    #define ARRAY_COUNT(array) (sizeof((array)) / sizeof((array)[0]))
#endif

struct thread_context
{
    s32 place_holder;
};

struct window_dims
{
    s32 width;
    s32 height;
};

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

}  // namespace tom
#endif  // TOMATO_COMMON_HPP_
