#ifndef TOMATO_OBJ_LOADER_HH_
#define TOMATO_OBJ_LOADER_HH_

#include "core.hh"
#include "vector.hh"

namespace tom
{

struct vert_ind
{
    s32 pos;
    s32 nrm;
    s32 uv;
};

struct face_inds
{
    vert_ind v1;
    vert_ind v2;
    vert_ind v3;
};

struct obj_loader
{
    obj_loader(platform_io *plat_io, const char *path);
    char name[1024];
    vector<v3> pos;
    vector<v2> uv;
    vector<v3> nrm;
    vector<face_inds> faces;
};

}  // namespace tom

#endif