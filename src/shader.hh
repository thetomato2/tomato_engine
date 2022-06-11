#ifndef TOMATO_SHADER_HH_
#define TOMATO_SHADER_HH_

#include "core.hh"

namespace tom
{
struct platform_io;

enum class shader_type
{
    vertex,
    pixel,
};

struct shader
{
    u32 id;

#if TOM_INTERNAL
    const char *vert_code;
    const char *pixel_code;
#endif
};

}  // namespace tom

#endif