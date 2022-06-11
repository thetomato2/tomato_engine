#include "mesh.hh"
#include "obj_loader.hh"

namespace tom
{

#ifdef SAVED_FOR_REFERENCE

mesh load_mesh(ogl::wgl_func_ptrs *gfx, platform_io *plat_io, const char *path)
{
    mesh result;

    #if TOM_INTERNAL
    result.path = path;
    #endif

    vector<f32> vert_pos;
    vector<f32> vert_uv;
    vector<f32> vert_nrm;
    vector<s32> inds;

    obj_loader *obj = new obj_loader(plat_io, path);

    szt j = 0;
    for (szt i = 0; i < obj->faces.size(); ++i) {
        vert_pos.push_back(obj->pos[obj->faces[i].v1.pos - 1].x);
        vert_pos.push_back(obj->pos[obj->faces[i].v1.pos - 1].y);
        vert_pos.push_back(obj->pos[obj->faces[i].v1.pos - 1].z);
        vert_uv.push_back(obj->uv[obj->faces[i].v1.uv - 1].u);
        vert_uv.push_back(obj->uv[obj->faces[i].v1.uv - 1].v);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v1.nrm - 1].x);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v1.nrm - 1].y);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v1.nrm - 1].z);
        inds.push_back(j++);

        vert_pos.push_back(obj->pos[obj->faces[i].v2.pos - 1].x);
        vert_pos.push_back(obj->pos[obj->faces[i].v2.pos - 1].y);
        vert_pos.push_back(obj->pos[obj->faces[i].v2.pos - 1].z);
        vert_uv.push_back(obj->uv[obj->faces[i].v2.uv - 1].u);
        vert_uv.push_back(obj->uv[obj->faces[i].v2.uv - 1].v);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v2.nrm - 1].x);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v2.nrm - 1].y);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v2.nrm - 1].z);
        inds.push_back(j++);

        vert_pos.push_back(obj->pos[obj->faces[i].v3.pos - 1].x);
        vert_pos.push_back(obj->pos[obj->faces[i].v3.pos - 1].y);
        vert_pos.push_back(obj->pos[obj->faces[i].v3.pos - 1].z);
        vert_uv.push_back(obj->uv[obj->faces[i].v3.uv - 1].u);
        vert_uv.push_back(obj->uv[obj->faces[i].v3.uv - 1].v);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v3.nrm - 1].x);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v3.nrm - 1].y);
        vert_nrm.push_back(obj->nrm[obj->faces[i].v3.nrm - 1].z);
        inds.push_back(j++);
    }

    result.ind_cnt = inds.size();

    gfx->gen_vert_arr(1, &result.vao);
    gfx->bind_vert_arr(result.vao);

    gfx->gen_buffers(1, &result.pos_vb);
    gfx->gen_buffers(1, &result.uv_vb);
    gfx->gen_buffers(1, &result.nrm_vb);
    gfx->gen_buffers(1, &result.ind_buf);

    gfx->bind_buffer(GL_ARRAY_BUFFER, result.pos_vb);
    gfx->buffer_data(GL_ARRAY_BUFFER, sizeof(f32) * vert_pos.size(), vert_pos.data(),
                     GL_STATIC_DRAW);
    gfx->enable_vert_attrib_array(POS_LOC);
    gfx->vertex_attrib_ptr(POS_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gfx->bind_buffer(GL_ARRAY_BUFFER, result.uv_vb);
    gfx->buffer_data(GL_ARRAY_BUFFER, sizeof(f32) * vert_uv.size(), vert_uv.data(), GL_STATIC_DRAW);
    gfx->enable_vert_attrib_array(TEXCOORD_LOC);
    gfx->vertex_attrib_ptr(TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0);

    gfx->bind_buffer(GL_ARRAY_BUFFER, result.nrm_vb);
    gfx->buffer_data(GL_ARRAY_BUFFER, sizeof(f32) * vert_nrm.size(), vert_nrm.data(),
                     GL_STATIC_DRAW);
    gfx->enable_vert_attrib_array(NRM_LOC);
    gfx->vertex_attrib_ptr(NRM_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gfx->bind_buffer(GL_ELEMENT_ARRAY_BUFFER, result.ind_buf);
    gfx->buffer_data(GL_ELEMENT_ARRAY_BUFFER, sizeof(s32) * inds.size(), inds.data(),
                     GL_STATIC_DRAW);

    // NOTE: so the vao is not tampered with
    gfx->bind_vert_arr(0);

    return result;
}
#endif

}  // namespace tom