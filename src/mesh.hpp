#ifndef TOMATO_MESH_HPP_
#define TOMATO_MESH_HPP_

#include "core.hpp"
#include "vector.hpp"

namespace tom
{
struct platform_io;

struct mesh
{
    u32 vao;
    u32 pos_vb;
    u32 uv_vb;
    u32 nrm_vb;
    u32 ind_buf;
    u32 ind_cnt;
    u32 vert_cnt;
    v3 origin;

#if TOM_INTERNAL
    const char *path;
#endif
};

}  // namespace tom

#endif