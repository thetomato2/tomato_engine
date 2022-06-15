#ifndef TOMATO_MATH_HH_
#define TOMATO_MATH_HH_

#include "core.hh"
#include "intrinsic.hh"

namespace tom
{

// ===============================================================================================
// #FREE_FUNCS
// ===============================================================================================
namespace math
{

global constexpr f32 eps_f32    = 0.00001f;
global constexpr f32 xm_pi      = 3.141592654f;
global constexpr f32 xm_2pi     = 6.283185307f;
global constexpr f32 xm_1divpi  = 0.318309886f;
global constexpr f32 xm_1div2pi = 0.159154943f;
global constexpr f32 xm_pidiv2  = 1.570796327f;
global constexpr f32 xm_pidiv4  = 0.785398163f;

inline f32 to_radian(f32 val)
{
    return (val * xm_pi) / 180.0f;
}

inline f32 to_degree(f32 val)
{
    return (val * 180.0f) / xm_pi;
}

// Calculate linear interpolation between two floats
inline f32 lerp(f32 v0, f32 v1, f32 a)
{
    return (1 - a) * v0 + a * v1;
}

inline f32 bi_lerp(f32 v0, f32 v1, f32 v2, f32 v3, f32 a, f32 b)
{
    f32 a1 = 1.0f - a;
    return (1.0f - b) * (v0 * a1 + v1 * a) + b * (v2 * a1 + v3 * a);
}

inline s32 fast_floor(f32 fp)
{
    s32 i = (s32)fp;

    return (fp < i) ? (i - 1) : (i);
}

inline f32 safe_ratio(f32 numerator, f32 divisor, f32 def)
{
    f32 res = def;

    if (divisor != 0.0f) {
        res = numerator / divisor;
    }

    return res;
}

inline f32 safe_ratio_0(f32 numerator, f32 divisor)
{
    return safe_ratio(numerator, divisor, 0.0f);
}

inline f32 safe_ratio_1(f32 numerator, f32 divisor)
{
    return safe_ratio(numerator, divisor, 1.0f);
}

inline bool equals_f32(f32 a, f32 b)
{
    bool res = (fabsf(a - b)) <= (eps_f32 * fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b))));

    return res;
}

inline void scalar_sin_cos(f32 *p_sin, f32 *p_cos, f32 val)
{
    TOM_ASSERT(p_sin);
    TOM_ASSERT(p_cos);
    f32 quo = xm_1div2pi * val;

    if (val >= 0.0f)
        quo = (f32)(s32)(quo + 0.5f);
    else
        quo = (f32)(s32)(quo - 0.5f);

    f32 y = val - xm_2pi * quo;
    f32 sign;
    if (y > xm_pidiv2) {
        y    = xm_pi - y;
        sign = -1.0f;
    } else if (y < -xm_pidiv2) {
        y    = -xm_pi - y;
        sign = -1.0f;
    } else {
        sign = 1.0f;
    }
    f32 y2 = y * y;

    // 11-degree minimax approximation
    *p_sin =
        (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) *
              y2 -
          0.16666667f) *
             y2 +
         1.0f) *
        y;

    // 10-degree minimax approximation
    f32 p =
        ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 -
         0.5f) *
            y2 +
        1.0f;
    *p_cos = sign * p;
}

inline bool near_equal_f32(f32 a, f32 b, f32 eps = 0.00001f)
{
    f32 delta = a - b;
    return fabsf(delta) <= eps;
}

template<typename T>
T square(T val)
{
    return val * val;
}

template<typename T>
T cube(T val)
{
    return val * val * val;
}

// Returns min or max if input is not in between
template<typename T>
T clamp(T val, T min, T max)
{
    T res = (val < min) ? min : val;

    if (res > max) res = max;

    return res;
}

template<typename T>
T max(T a, T b)
{
    T res;
    a > b ? res = a : res = b;
    return res;
}

template<typename T>
T min(T a, T b)
{
    T res;
    a < b ? res = a : res = b;
    return res;
}

template<typename T>
inline f32 normalize_coord(T min, T max, T a)
{
    TOM_ASSERT(a >= min && a <= max);
    return ((f32)a - f32(min)) / ((f32)max - (f32)min);
}

}  // namespace math

// ===============================================================================================
// #VECTOR 2
// ===============================================================================================

inline v2 v2_init(f32 a)
{
    v2 res;

    res.x = a;
    res.y = a;

    return res;
}

inline v2 v2_init(f32 x, f32 y)
{
    v2 res;

    res.x = x;
    res.y = y;

    return res;
}

inline v2 v3_init(s32 x, s32 y)
{
    v2 res;

    res.x = scast(f32, x);
    res.y = scast(f32, y);

    return res;
}

inline v2 v2_init(u32 x, u32 y)
{
    v2 res;

    res.x = scast(f32, x);
    res.y = scast(f32, y);

    return res;
}

inline v2 operator+(v2 lhs, v2 rhs)
{
    v2 res;

    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;

    return res;
}

inline v2 operator+(v2 lhs, f32 rhs)
{
    v2 res;

    res.x = lhs.x + rhs;
    res.y = lhs.y + rhs;

    return res;
}

inline v2 &operator+=(v2 &lhs, v2 rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

inline v2 &operator+=(v2 &lhs, f32 rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;

    return lhs;
}

inline v2 operator-(v2 lhs)
{
    v2 res;

    res.x = -lhs.x;
    res.y = -lhs.y;

    return res;
}
inline v2 operator-(v2 lhs, v2 rhs)
{
    v2 res;

    res.x = lhs.x - rhs.x;
    res.y = lhs.y - rhs.y;

    return res;
}

inline v2 operator-(v2 lhs, f32 rhs)
{
    v2 res;

    res.x = lhs.x - rhs;
    res.y = lhs.y - rhs;

    return res;
}

inline v2 &operator-=(v2 &lhs, v2 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v2 &operator-=(v2 &lhs, f32 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v2 operator*(f32 lhs, v2 rhs)
{
    v2 res;

    res.x = lhs * rhs.x;
    res.y = lhs * rhs.y;

    return res;
}

inline v2 operator*(v2 lhs, f32 rhs)
{
    v2 res;

    res.x = lhs.x * rhs;
    res.y = lhs.y * rhs;

    return res;
}

inline v2 &operator*=(v2 &lhs, f32 rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;

    return lhs;
}

inline v2 operator/(v2 lhs, f32 rhs)
{
    v2 res;

    res.x = lhs.x / rhs;
    res.y = lhs.y / rhs;

    return res;
}

inline v2 &operator/=(v2 &lhs, f32 rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;

    return lhs;
}

inline bool operator==(v2 &lhs, v2 &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(v2 &lhs, v2 &rhs)
{
    return !(lhs == rhs);
}

// ===============================================================================================
// #VECTOR 3
// ===============================================================================================

inline v3 v3_init(f32 a)
{
    v3 res;

    res.x = a;
    res.y = a;
    res.z = a;

    return res;
}

inline v3 v3_init(f32 x, f32 y, f32 z)
{
    v3 res;

    res.x = x;
    res.y = y;
    res.z = z;

    return res;
}

inline v3 v3_init(s32 x, s32 y, s32 z)
{
    v3 res;

    res.x = scast(f32, x);
    res.y = scast(f32, y);
    res.z = scast(f32, z);

    return res;
}

inline v3 v3_init(u32 x, u32 y, u32 z)
{
    v3 res;

    res.x = scast(f32, x);
    res.y = scast(f32, y);
    res.z = scast(f32, z);

    return res;
}

inline v3 v3_init(v2 a, f32 z = 0.f)
{
    v3 res;

    res.x = a.x;
    res.y = a.y;
    res.z = z;

    return res;
}

inline v3 v3_zero()
{
    return {};
}

inline v3 operator+(v3 lhs, v3 rhs)
{
    v3 res;

    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;
    res.z = lhs.z + rhs.z;

    return res;
}

inline v3 operator+(v3 lhs, f32 rhs)
{
    v3 res;

    res.x = lhs.x + rhs;
    res.y = lhs.y + rhs;
    res.z = lhs.z + rhs;

    return res;
}

inline v3 &operator+=(v3 &lhs, v3 rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

inline v3 &operator+=(v3 &lhs, f32 rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

inline v3 operator-(v3 lhs)
{
    v3 res;

    res.x = -lhs.x;
    res.y = -lhs.y;
    res.z = -lhs.z;

    return res;
}
inline v3 operator-(v3 lhs, v3 rhs)
{
    v3 res;

    res.x = lhs.x - rhs.x;
    res.y = lhs.y - rhs.y;
    res.z = lhs.z - rhs.z;

    return res;
}

inline v3 operator-(v3 lhs, f32 rhs)
{
    v3 res;

    res.x = lhs.x - rhs;
    res.y = lhs.y - rhs;
    res.z = lhs.z - rhs;

    return res;
}

inline v3 &operator-=(v3 &lhs, v3 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v3 &operator-=(v3 &lhs, f32 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v3 operator*(f32 lhs, v3 rhs)
{
    v3 res;

    res.x = lhs * rhs.x;
    res.y = lhs * rhs.y;
    res.z = lhs * rhs.z;

    return res;
}

inline v3 operator*(v3 lhs, f32 rhs)
{
    v3 res;

    res.x = lhs.x * rhs;
    res.y = lhs.y * rhs;
    res.z = lhs.z * rhs;

    return res;
}

inline v3 &operator*=(v3 &lhs, f32 rhs)
{
    lhs = lhs * rhs;

    return lhs;
}

inline v3 operator/(v3 lhs, f32 rhs)
{
    v3 res;

    res.x = lhs.x / rhs;
    res.y = lhs.y / rhs;
    res.z = lhs.z / rhs;

    return res;
}

inline v3 &operator/=(v3 &lhs, f32 rhs)
{
    lhs = lhs / rhs;

    return lhs;
}

inline bool operator==(v3 &lhs, v3 rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

inline bool operator!=(v3 &lhs, v3 rhs)
{
    return !(lhs == rhs);
}

// ===============================================================================================
// #VECTOR 4
// ===============================================================================================

inline v4 v4_init(f32 x, f32 y, f32 z, f32 w)
{
    v4 res;

    res.x = x;
    res.y = y;
    res.z = z;
    res.w = w;

    return res;
}

inline v4 v4_init(v2 a, f32 z = 0.f, f32 w = 0.0f)
{
    v4 res;
    res.x = a.x;
    res.y = a.y;
    res.z = z;
    res.w = w;

    return res;
}

inline v4 v4_init(v3 a, f32 w = 0.0f)
{
    v4 res;
    res.x = a.x;
    res.y = a.y;
    res.z = a.z;
    res.w = w;

    return res;
}

inline v4 v4_zero()
{
    return {};
}

inline v4 operator+(v4 lhs, v4 rhs)
{
    v4 res;

    res.x = lhs.x + rhs.x;
    res.y = lhs.y + rhs.y;
    res.z = lhs.z + rhs.z;
    res.w = lhs.w + rhs.w;

    return res;
}

inline v4 operator+(v4 lhs, f32 rhs)
{
    v4 res;

    res.x = lhs.x + rhs;
    res.y = lhs.y + rhs;
    res.z = lhs.z + rhs;
    res.w = lhs.w + rhs;

    return res;
}

inline v4 &operator+=(v4 &lhs, v4 rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

inline v4 &operator+=(v4 &lhs, f32 rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

inline v4 operator-(v4 lhs)
{
    v4 res;

    res.x = -lhs.x;
    res.y = -lhs.y;
    res.z = -lhs.z;
    res.w = -lhs.w;

    return res;
}
inline v4 operator-(v4 lhs, v4 rhs)
{
    v4 res;

    res.x = lhs.x - rhs.x;
    res.y = lhs.y - rhs.y;
    res.z = lhs.z - rhs.z;
    res.w = lhs.w - rhs.w;

    return res;
}

inline v4 operator-(v4 lhs, f32 rhs)
{
    v4 res;

    res.x = lhs.x - rhs;
    res.y = lhs.y - rhs;
    res.z = lhs.z - rhs;
    res.w = lhs.w - rhs;

    return res;
}

inline v4 &operator-=(v4 &lhs, v4 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v4 &operator-=(v4 &lhs, f32 rhs)
{
    lhs = lhs - rhs;

    return lhs;
}

inline v4 operator*(f32 lhs, v4 rhs)
{
    v4 res;

    res.x = lhs * rhs.x;
    res.y = lhs * rhs.y;
    res.z = lhs * rhs.z;
    res.w = lhs * rhs.w;

    return res;
}

inline v4 operator*(v4 lhs, f32 rhs)
{
    v4 res;

    res.x = lhs.x * rhs;
    res.y = lhs.y * rhs;
    res.z = lhs.z * rhs;
    res.w = lhs.w * rhs;

    return res;
}

inline v4 &operator*=(v4 &lhs, f32 rhs)
{
    lhs = lhs * rhs;

    return lhs;
}

inline v4 operator*(v4 lhs, v3 rhs)
{
    v4 res;

    res.w = (lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z);
    res.x = (lhs.w * rhs.x) + (lhs.y * rhs.z) - (lhs.z * rhs.y);
    res.y = (lhs.w * rhs.y) + (lhs.z * rhs.x) - (lhs.x * rhs.z);
    res.z = (lhs.w * rhs.z) + (lhs.x * rhs.y) - (lhs.y * rhs.x);

    return res;
}

inline v4 operator*=(v4 &lhs, v3 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

inline v4 operator*(v4 lhs, v4 rhs)
{
    v4 res;

    res.w = (lhs.w * rhs.w) - (lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z);
    res.x = (lhs.x * rhs.w) + (lhs.w * rhs.x) + (lhs.y * rhs.z) - (lhs.z * rhs.y);
    res.y = (lhs.y * rhs.w) + (lhs.w * rhs.y) + (lhs.z * rhs.x) - (lhs.x * rhs.z);
    res.z = (lhs.z * rhs.w) + (lhs.w * rhs.z) + (lhs.x * rhs.y) - (lhs.y * rhs.x);

    return res;
}

inline v4 operator*=(v4 &lhs, v4 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

inline v4 operator/(v4 lhs, f32 rhs)
{
    v4 res;

    res.x = lhs.x / rhs;
    res.y = lhs.y / rhs;
    res.z = lhs.z / rhs;
    res.w = lhs.w / rhs;

    return res;
}

inline v4 &operator/=(v4 &lhs, f32 rhs)
{
    lhs = lhs / rhs;

    return lhs;
}

inline bool operator==(v4 &lhs, v4 &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

inline bool operator!=(v4 &lhs, v4 &rhs)
{
    return !(lhs == rhs);
}

// ===============================================================================================
// #VECTOR FUNCS
// ===============================================================================================
namespace vec
{

// NOTE: inner product or dot product
inline f32 inner(v2 a, v2 b)
{
    f32 res = a.x * b.x + a.y * b.y;
    return res;
};

inline f32 inner(v3 a, v3 b)
{
    f32 res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
};

inline f32 inner(v4 a, v4 b)
{
    f32 res = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return res;
};

inline f32 dot(v2 a, v2 b)
{
    return inner(a, b);
}

inline f32 dot(v3 a, v3 b)
{
    return inner(a, b);
}

inline f32 dot(v4 a, v4 b)
{
    return inner(a, b);
}

inline v2 lerp(v2 a, v2 b, f32 t)
{
    return (1.0f - t) * a + t * b;
}

inline v3 lerp(v3 a, v3 b, f32 t)
{
    return (1.0f - t) * a + t * b;
}

inline v4 lerp(v4 a, v4 b, f32 t)
{
    return (1.0f - t) * a + t * b;
}

inline v3 cross(v3 a, v3 b)
{
    v3 res;

    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;

    return res;
}

inline v2 hadamard(v2 a, v2 b)
{
    v2 res;

    res.x = a.x * b.x;
    res.y = a.y * b.y;

    return res;
}

inline v3 hadamard(v3 a, v3 b)
{
    v3 res;

    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;

    return res;
}

inline v4 hadamard(v4 a, v4 b)
{
    v4 res;

    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    res.w = a.w * b.w;

    return res;
}

inline f32 length_sq(v2 a)
{
    f32 res = inner(a, a);
    return res;
}

inline f32 length_sq(v3 a)
{
    f32 res = inner(a, a);
    return res;
}

inline f32 length_sq(v4 a)
{
    f32 res = inner(a, a);
    return res;
}

inline f32 length(v2 a)
{
    f32 res = math::sqrt_f32(length_sq(a));
    return res;
}

inline f32 length(v3 a)
{
    f32 res = math::sqrt_f32(length_sq(a));
    return res;
}

inline f32 length(v4 a)
{
    f32 res = math::sqrt_f32(length_sq(a));
    return res;
}

inline v3 normalize(v3 a)
{
    f32 len = length(a);
    TOM_ASSERT(len != 0.0f);

    v3 res;

    res.x = a.x / len;
    res.y = a.y / len;
    res.z = a.z / len;

    return res;
}

inline f32 distance(v2 a, v2 b)
{
    v2 dis = a - b;
    dis.x  = abs(dis.x);
    dis.y  = abs(dis.y);

    f32 res = length(dis);

    return res;
}

inline f32 distance(v3 a, v3 b)
{
    v3 dis = a - b;
    dis.x  = abs(dis.x);
    dis.y  = abs(dis.y);
    dis.z  = abs(dis.z);

    f32 res = length(dis);

    return res;
}

inline v2 reflect(v2 a, v2 b)
{
    v2 res;

    f32 dot = vec::inner(a, b);

    res.x = a.x - (2.0f * b.x) * dot;
    res.y = a.y - (2.0f * b.y) * dot;

    return res;
}

inline v3 reflect(v3 a, v3 b)
{
    v3 res;

    f32 dot = vec::inner(a, b);

    res.x = a.x - (2.0f * b.x) * dot;
    res.y = a.y - (2.0f * b.y) * dot;
    res.z = a.z - (2.0f * b.z) * dot;

    return res;
}

// Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c)
// NOTE: Assumes P is on the plane of the triangle
inline v3 barycenter(v3 p, v3 a, v3 b, v3 c)
{
    v3 res;

    v3 v0   = { b.x - a.x, b.y - a.y, b.z - a.z };
    v3 v1   = { c.x - a.x, c.y - a.y, c.z - a.z };
    v3 v2   = { p.x - a.x, p.y - a.y, p.z - a.z };
    f32 d00 = v0.x * v0.x + v0.y * v0.y + v0.z * v0.z;
    f32 d01 = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    f32 d11 = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
    f32 d20 = v2.x * v0.x + v2.y * v0.y + v2.z * v0.z;
    f32 d21 = v2.x * v1.x + v2.y * v1.y + v2.z * v1.z;

    f32 denom = d00 * d11 - d01 * d01;

    res.y = (d11 * d20 - d01 * d21) / denom;
    res.z = (d00 * d21 - d01 * d20) / denom;
    res.x = 1.0f - (res.z + res.y);

    return res;
}

inline void print(v2 a)
{
    printf("(%f, %f)\n", a.x, a.y);
}

inline void print(v3 a)
{
    printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

inline void print(v4 a)
{
    printf("(%f, %f, %f, %f)\n", a.x, a.y, a.z, a.w);
}

}  // namespace vec

// ===============================================================================================
// #Quaternion
// ===============================================================================================

using quat = v4;  // Quaternion
namespace qua
{

inline quat to_quat(v3 v, f32 a)
{
    f32 half_angle_rad = math::to_radian(a / 2.0f);
    f32 sin_half_angle = sinf(half_angle_rad);
    f32 cos_half_angle = cosf(half_angle_rad);

    quat res;

    res.x = v.x * sin_half_angle;
    res.y = v.y * sin_half_angle;
    res.z = v.z * sin_half_angle;
    res.w = cos_half_angle;

    return res;
}

inline f32 norm(quat a)
{
    f32 i   = vec::inner(a.xyz, a.xyz);
    f32 s   = math::square(a.w);
    f32 res = math::sqrt_f32(i + s);

    return res;
}

inline quat unit_norm(quat a)
{
    f32 half_angle_rad = math::to_radian(a.w / 2.0f);
    f32 sin_half_angle = sinf(half_angle_rad);
    f32 cos_half_angle = cosf(half_angle_rad);

    a.w   = cos_half_angle;
    a.xyz = vec::normalize(a.xyz);
    a.xyz = a.xyz * sin_half_angle;

    return a;
}
inline quat conjuate(quat q)
{
    quat res;
    res.x = -q.x;
    res.y = -q.y;
    res.z = -q.z;
    res.w = q.w;

    return res;
}

inline quat inverse(quat a)
{
    f32 abs   = 1 / (math::square(norm(a)));
    quat conj = conjuate(a);
    f32 s     = conj.w * abs;
    v3 i      = conj.xyz * abs;

    quat res;
    res.xyz = i;
    res.w   = s;

    return res;
}

// rotate v3 around arbitrary axis
inline v3 rotate(v3 v, v3 u, f32 a)
{
    quat p   = { v.x, v.y, v.z, 0.0f };
    u        = vec::normalize(u);
    quat q   = unit_norm({ u.x, u.y, u.z, a });
    quat q_i = inverse(q);

    quat res = q * p * q_i;

    return res.xyz;
}

// transform a v3 by quat
inline v3 rotate(v3 v, quat q)
{
    v3 res;

    res.x = v.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) +
            v.y * (2 * q.x * q.y - 2 * q.w * q.z) + v.z * (2 * q.x * q.z + 2 * q.w * q.y);
    res.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) +
            v.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) +
            v.z * (-2 * q.w * q.x + 2 * q.y * q.z);
    res.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) +
            v.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

    return res;
}

}  // namespace qua

// ===============================================================================================
// #Matrix 4x4
// ===============================================================================================

inline m4 operator*(m4 a, m4 b)
{
    m4 res = {};

    res.e[0]  = a.e[0] * b.e[0] + a.e[1] * b.e[4] + a.e[2] * b.e[8] + a.e[3] * b.e[12];
    res.e[1]  = a.e[0] * b.e[1] + a.e[1] * b.e[5] + a.e[2] * b.e[9] + a.e[3] * b.e[13];
    res.e[2]  = a.e[0] * b.e[2] + a.e[1] * b.e[6] + a.e[2] * b.e[10] + a.e[3] * b.e[14];
    res.e[3]  = a.e[0] * b.e[3] + a.e[1] * b.e[7] + a.e[2] * b.e[11] + a.e[3] * b.e[15];
    res.e[4]  = a.e[4] * b.e[0] + a.e[5] * b.e[4] + a.e[6] * b.e[8] + a.e[7] * b.e[12];
    res.e[5]  = a.e[4] * b.e[1] + a.e[5] * b.e[5] + a.e[6] * b.e[9] + a.e[7] * b.e[13];
    res.e[6]  = a.e[4] * b.e[2] + a.e[5] * b.e[6] + a.e[6] * b.e[10] + a.e[7] * b.e[14];
    res.e[7]  = a.e[4] * b.e[3] + a.e[5] * b.e[7] + a.e[6] * b.e[11] + a.e[7] * b.e[15];
    res.e[8]  = a.e[8] * b.e[0] + a.e[9] * b.e[4] + a.e[10] * b.e[8] + a.e[11] * b.e[12];
    res.e[9]  = a.e[8] * b.e[1] + a.e[9] * b.e[5] + a.e[10] * b.e[9] + a.e[11] * b.e[13];
    res.e[10] = a.e[8] * b.e[2] + a.e[9] * b.e[6] + a.e[10] * b.e[10] + a.e[11] * b.e[14];
    res.e[11] = a.e[8] * b.e[3] + a.e[9] * b.e[7] + a.e[10] * b.e[11] + a.e[11] * b.e[15];
    res.e[12] = a.e[12] * b.e[0] + a.e[13] * b.e[4] + a.e[14] * b.e[8] + a.e[15] * b.e[12];
    res.e[13] = a.e[12] * b.e[1] + a.e[13] * b.e[5] + a.e[14] * b.e[9] + a.e[15] * b.e[13];
    res.e[14] = a.e[12] * b.e[2] + a.e[13] * b.e[6] + a.e[14] * b.e[10] + a.e[15] * b.e[14];
    res.e[15] = a.e[12] * b.e[3] + a.e[13] * b.e[7] + a.e[14] * b.e[11] + a.e[15] * b.e[15];

    return res;
}

inline v3 transform(m4 a, v3 p, f32 Pw = 1.0f)
{
    v3 res = {};

    res.x = p.x * a.m[0][0] + p.y * a.m[0][1] + p.z * a.m[0][2] + Pw * a.m[0][3];
    res.y = p.x * a.m[1][0] + p.y * a.m[1][1] + p.z * a.m[1][2] + Pw * a.m[1][3];
    res.z = p.x * a.m[2][0] + p.y * a.m[2][1] + p.z * a.m[2][2] + Pw * a.m[2][3];

    return res;
}

inline v3 operator*(m4 a, v3 p)
{
    v3 res = transform(a, p, 1.0f);

    return res;
}

namespace mat
{

inline m4 y_up_to_z_up()
{
    m4 res = { 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    return res;
}

inline m4 identity(f32 a = 1.0f)
{
    m4 res = {
        a, 0.0f, 0.0f, 0.0f, 0.0f, a, 0.0f, 0.0f, 0.0f, 0.0f, a, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    return res;
}

inline m4 rot_x(f32 a)
{
    f32 c = cos(a);
    f32 s = sin(a);

    // clang-format off
    m4 res = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c, s, 0.0f,
        0.0f, -s, c, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    return res;
}

inline m4 rot_x(m4 a, f32 b)
{
    return a * rot_x(b);
}

inline m4 rot_y(f32 a)
{
    f32 c = cos(a);
    f32 s = sin(a);

    // clang-format off
    m4 res = {
        c, 0.0f, -s, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f,
         s, 0.0f, c, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    return res;
}

inline m4 rot_y(m4 a, f32 b)
{
    return a * rot_y(b);
}

inline m4 rot_z(f32 a)
{
    f32 c = cos(a);
    f32 s = sin(a);

    m4 res = {
        c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    return res;
}

inline m4 rot_z(m4 a, f32 b)
{
    return a * rot_z(b);
}

// u = arbitrary axis, a = angle
inline m4 rotate(v3 u, f32 a)
{
    a       = math::to_radian(a);
    f32 c   = cos(a);
    f32 l_c = 1 - c;
    f32 s   = sin(a);

    m4 res = { u.x * u.x + (1 - u.x * u.x) * c,
               u.x * u.y * l_c + u.z * s,
               u.x * u.z * l_c - u.y * s,
               0,
               u.x * u.y * l_c - u.z * s,
               u.y * u.y + (1 - u.y * u.y) * c,
               u.y * u.z * l_c + u.x * s,
               0,
               u.x * u.z * l_c + u.y * s,
               u.y * u.z * l_c - u.x * s,
               u.z * u.z + (1 - u.z * u.z) * c,
               0,
               0,
               0,
               0,
               1 };
    return res;
}

// u = arbitrary axis, a = angle
inline m4 rotate(m4 m, v3 u, f32 a)
{
    m4 res = m * rotate(u, a);

    return res;
}

inline m4 scale(m4 a, f32 b)
{
    m4 res = mat::identity(b);

    res = a * res;

    return res;
}

inline m4 transpose(m4 a)
{
    m4 res;

#if 1
    res.e[0]  = a.e[0];
    res.e[1]  = a.e[4];
    res.e[2]  = a.e[8];
    res.e[3]  = a.e[12];
    res.e[4]  = a.e[1];
    res.e[5]  = a.e[5];
    res.e[6]  = a.e[9];
    res.e[7]  = a.e[13];
    res.e[8]  = a.e[2];
    res.e[9]  = a.e[6];
    res.e[10] = a.e[10];
    res.e[11] = a.e[14];
    res.e[12] = a.e[3];
    res.e[13] = a.e[7];
    res.e[14] = a.e[11];
    res.e[15] = a.e[15];
#else

    for (s32 i = 0; i < 4; ++i) {
        for (s32 j = 0; j < 4; ++j) {
            res.m[i][j] = a.m[j][i];
        }
    }
#endif

    return res;
}

inline m4 proj_persp(f32 aspect_ratio, f32 fov_y, f32 near_z, f32 far_z)
{
    TOM_ASSERT(near_z > 0.0f && far_z > 0.0f);
    TOM_ASSERT(!math::near_equal_f32(fov_y, 0.0f, 0.00001f * 2.0f));
    TOM_ASSERT(!math::near_equal_f32(aspect_ratio, 0.0f));
    TOM_ASSERT(!math::near_equal_f32(near_z, far_z));

    f32 sin_fov, cos_fov;
    math::scalar_sin_cos(&sin_fov, &cos_fov, 0.5f * fov_y);

    f32 height  = cos_fov / sin_fov;
    f32 width   = height / aspect_ratio;
    f32 f_range = far_z / (far_z - near_z);

    m4 res      = {};
    res.m[0][0] = width;
    res.m[1][1] = height;
    res.m[2][2] = f_range;
    res.m[2][3] = 1.0f;
    res.m[3][2] = -f_range * near_z;

    return res;
}

inline m4 proj_ortho(f32 aspect_ratio)
{
    f32 a = 1.0f;
    f32 b = aspect_ratio;

    m4 res = { a,    0.0f, 0.0f, 0.0f, 0.0f, b,    0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    return res;
}

inline m4 col_3x3(v3 x, v3 y, v3 z)
{
    // clang-format off
    m4 res = {
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f, 
        x.z, y.z, z.z, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    return res;
}

inline m4 row_3x3(v3 x, v3 y, v3 z)
{
    // clang-format off
    m4 res = {
        x.x, x.y, x.z, 0.0f, 
        y.x, y.y, y.z, 0.0f, 
        z.x, z.y, z.z, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    return res;
}
inline m4 translate(v3 t)
{
    m4 res = mat::identity();

    res.m[3][0] = t.x;
    res.m[3][1] = t.y;
    res.m[3][2] = t.z;

    return res;
}

inline m4 translate(m4 a, v3 t)
{
    a.m[3][0] += t.x;
    a.m[3][1] += t.y;
    a.m[3][2] += t.z;

    return a;
}

inline v3 get_col(m4 a, u32 c)
{
    v3 res = { a.m[0][c], a.m[1][c], a.m[2][c] };

    return res;
}

inline v3 get_row(m4 a, u32 r)
{
    v3 res = { a.m[r][0], a.m[r][1], a.m[r][2] };

    return res;
}

inline m4 uvn_to_m4(v3 pos, v3 u, v3 v, v3 n)
{
    m4 res = { u.x, u.y, u.z, -pos.x, v.x,  v.y,  v.z,  -pos.y,
               n.x, n.y, n.z, -pos.z, 0.0f, 0.0f, 0.0f, 1.0f };

    return res;
}

inline m4 look_to(v3 eye_pos, v3 eye_dir, v3 up_dir)
{
    TOM_ASSERT(eye_dir != v3_zero());
    TOM_ASSERT(up_dir != v3_zero());

    v3 r2 = vec::normalize(eye_dir);
    v3 r0 = vec::normalize(vec::cross(up_dir, r2));
    v3 r1 = vec::cross(r2, r0);

    f32 d0 = vec::dot(r0, -eye_pos);
    f32 d1 = vec::dot(r1, -eye_pos);
    f32 d2 = vec::dot(r2, -eye_pos);

    m4 res;
    res.r[0] = v4_init(r0, d0);
    res.r[1] = v4_init(r1, d1);
    res.r[2] = v4_init(r2, d2);
    res.r[3] = mat::identity().r[3];

    return res;
}

inline m4 look_at(v3 eye_pos, v3 target_pos, v3 up_dir)
{
    v3 eye_dir = target_pos - eye_pos;
    return look_to(eye_pos, eye_dir, up_dir);
}

inline m4 get_uvn(v3 forward, v3 up, v3 pos)
{
    v3 n = vec::normalize(forward);
    v3 u = vec::normalize(vec::cross(up, n));
    v3 v = vec::cross(n, u);

    m4 res = mat::row_3x3(u, v, n) * mat::translate(-pos);

    return res;
}

inline m4 inverse(m4 a)
{
}

}  // namespace mat

// ===============================================================================================
// #RECTANGLE FUNCS
// ===============================================================================================

inline rect3 rect3_init(rect2 a)
{
    rect3 res;

    res.min = v3_init(a.min);
    res.max = v3_init(a.max);

    return res;
}

namespace rec
{

inline v2 max_corner(rect2 a)
{
    v2 res = a.max;
    return res;
}

inline v3 max_corner(rect3 a)
{
    v3 res = a.max;
    return res;
}

inline v2 min_corner(rect2 a)
{
    v2 res = a.min;
    return res;
}

inline v3 min_corner(rect3 a)
{
    v3 res = a.min;
    return res;
}

inline rect2 min_max(v2 min, v2 max)
{
    rect2 res;

    res.min = min;
    res.max = max;

    return res;
}

inline rect3 min_max(v3 min, v3 max)
{
    rect3 res;

    res.min = min;
    res.max = max;

    return res;
}

inline rect2 min_dim(v2 min, v2 dim)
{
    rect2 res;

    res.min = min;
    res.max = min + dim;

    return res;
}

inline rect3 min_dim(v3 min, v3 dim)
{
    rect3 res;

    res.min = min;
    res.max = min + dim;

    return res;
}

inline rect2 center_half_dim(v2 center, v2 half_dim)
{
    rect2 res;

    res.min = center - half_dim;
    res.max = center + half_dim;

    return res;
}

inline rect3 center_half_dim(v3 center, v3 half_dim)
{
    rect3 res;

    res.min = center - half_dim;
    res.max = center + half_dim;

    return res;
}

inline rect2 center_dim(v2 center, v2 dim)
{
    rect2 res = center_half_dim(center, dim * 0.5f);

    return res;
}

inline rect3 center_dim(v3 center, v3 dim)
{
    rect3 res = center_half_dim(center, dim * 0.5f);

    return res;
}

inline v2 center(rect2 a)
{
    v2 res = 0.5f * (a.min + a.max);

    return res;
}

inline v3 center(rect3 a)
{
    v3 res = 0.5f * (a.min + a.max);

    return res;
}

inline bool is_inside(rect2 a, v2 test)
{
    bool res = test.x >= a.min.x && test.y >= a.min.y && test.x <= a.max.x && test.y <= a.max.y;
    return res;
}

inline bool is_inside(rect3 a, v3 test)
{
    bool res = test.x >= a.min.x && test.y >= a.min.y && test.x <= a.max.x && test.y <= a.max.y &&
               test.z >= a.min.z && test.z <= a.max.z;
    return res;
}

inline rect2 add_dim(rect2 a, v2 dim)
{
    rect2 res;

    res.min = a.min - dim;
    res.max = a.max + dim;

    return res;
}

inline rect3 add_dim(rect3 a, v3 dim)
{
    rect3 res;

    res.min = a.min - dim;
    res.max = a.max + dim;

    return res;
}

inline rect2 add_radius(rect2 a, f32 r)
{
    v2 r_ = { r, r };
    return add_dim(a, r_);
}

inline rect3 add_radius(rect3 a, f32 r)
{
    v3 r_ = { r, r, r };
    return add_dim(a, r_);
}

inline bool intersect(rect2 a, rect2 b)
{
    bool res = !(b.max.x < a.min.x || b.min.x > a.max.x || b.max.y < a.min.y || b.min.y > a.max.y);

    return res;
}

inline bool intersect(rect3 a, rect3 b)
{
    bool res = !(b.max.x < a.min.x || b.min.x > a.max.x || b.max.y < a.min.y || b.min.y > a.max.y ||
                 b.max.z < a.min.z || b.min.z > a.max.z);

    return res;
}

inline v2 barycenter(rect2 a, v3 p)
{
    v2 res;

    res.x = math::safe_ratio_0(p.x - a.min.x, a.max.x - a.min.x);
    res.y = math::safe_ratio_0(p.y - a.min.y, a.max.y - a.min.y);

    return res;
}

inline v3 barycenter(rect3 a, v3 p)
{
    v3 res;

    res.x = math::safe_ratio_0(p.x - a.min.x, a.max.x - a.min.x);
    res.y = math::safe_ratio_0(p.y - a.min.y, a.max.y - a.min.y);
    res.z = math::safe_ratio_0(p.z - a.min.z, a.max.z - a.min.z);

    return res;
}

}  // namespace rec

inline void print_m4(m4 a)
{
    for (s32 i = 0; i < 4; ++i) {
        printf("%f, %f, %f, %f", a.m[i][0], a.m[i][1], a.m[i][2], a.m[i][3]);
    }
}
inline void print_v2(v2 a)
{
    printf("%f, %f", a.x, a.y);
}

inline void print_v3(v3 a)
{
    printf("%f, %f, %f", a.x, a.y, a.z);
}

inline void print_v4(v4 a)
{
    printf("%f, %f, %f, %f", a.x, a.y, a.z, a.w);
}
}  // namespace tom

#endif  // TOMATO_MATH_HPP_
