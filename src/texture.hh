#ifndef TOMATO_TEXTURE_HH_
#define TOMATO_TEXTURE_HH_

#include "core.hh"

namespace tom
{

struct platform_io;

enum class texture_type
{
    albedo,
    normal
};

struct texture_buffer
{
    s32 width, height;
    byte *data;
};

struct texture
{
    texture_type type;
    u32 tex_target;
    u32 tex_obj;

#if TOM_INTERNAL
    const char *path;
#endif
};

}  // namespace tom

#endif
