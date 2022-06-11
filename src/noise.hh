#ifndef TOMATO_NOISE_HH_
#define TOMATO_NOISE_HH_

#include "core.hh"

namespace tom
{

struct fractal_params
{
    fractal_params() : frequency(1.0f), amplitude(1.0f), lacunarity(2.0f), persistance(0.5f) {}

    f32 frequency, amplitude, lacunarity, persistance;
};

f32 simplex_fractal(szt octaves, f32 x, fractal_params fp);
f32 simplex_fractal(szt octaves, f32 x, f32 y, fractal_params fp);
f32 simplex_fractal(szt octaves, f32 x, f32 y, f32 z, fractal_params fp);
f32 simplex_fractal(szt octaves, f32 x);
f32 simplex_fractal(szt octaves, f32 x, f32 y);
f32 simplex_fractal(szt octaves, f32 x, f32 y, f32 z);
}  // namespace tom

#endif  // !TOMATO_NOISE_HPP