#include "obj_loader.hh"

namespace tom
{
obj_loader::obj_loader(platform_io *plat_io, const char *path)
{
    auto obj_file = plat_io->platform_read_entire_file(nullptr, path);

    if (obj_file.content_size == 0) {
        printf("ERROR-> Faield to read - %s", path);
        INVALID_CODE_PATH;
    }

    char *file_ptr = (char *)obj_file.contents;
    u32 i          = 0;

    auto to_f32 = [](char *f_ptr, u32 &i) -> f32 {
        while (f_ptr[i] == ' ') ++i;
        char f32_buf[64];
        u32 j = 0;
        while (f_ptr[i] != ' ' && f_ptr[i] != '\n') {
            f32_buf[j++] = f_ptr[i++];
        }
        f32_buf[j] = '\0';
        f32 res    = atof(f32_buf);
        return res;
    };

    auto to_s32 = [](char *f_ptr, u32 &i) -> s32 {
        while (f_ptr[i] == ' ' || f_ptr[i] == '/') ++i;
        char s32_buf[64];
        u32 j = 0;
        while (f_ptr[i] != '/' && f_ptr[i] != '\n' && f_ptr[i] != ' ') {
            s32_buf[j++] = f_ptr[i++];
        }
        s32_buf[j] = '\0';
        s32 res    = atoi(s32_buf);
        return res;
    };

    while (i < obj_file.content_size) {
        char c = file_ptr[i++];
        switch (c) {
            case '#': {
                while (file_ptr[i] != '\n') {
                    ++i;
                }
                ++i;
            } break;
            case 'o': {
                ++i;
                char *name_ptr = &name[0];
                while (file_ptr[i] != '\n') {
                    *name_ptr++ = file_ptr[i];
                    ++i;
                }
                *name_ptr = '\0';
                ++i;
            } break;
            case 'v': {
                char c2 = file_ptr[i++];
                switch (c2) {
                    case ' ': {
                        v3 res;
                        res.x = to_f32(file_ptr, i);
                        res.y = to_f32(file_ptr, i);
                        res.z = to_f32(file_ptr, i);
                        pos.push_back(res);
                        while (file_ptr[i] != '\n') {
                            ++i;
                        }
                        ++i;
                    } break;
                    case 'n': {
                        v3 res;
                        res.x = to_f32(file_ptr, i);
                        res.y = to_f32(file_ptr, i);
                        res.z = to_f32(file_ptr, i);
                        nrm.push_back(res);
                        while (file_ptr[i] != '\n') {
                            ++i;
                        }
                        ++i;
                    } break;
                    case 't': {
                        v2 res;
                        res.x = to_f32(file_ptr, i);
                        res.y = to_f32(file_ptr, i);
                        uv.push_back(res);
                        while (file_ptr[i] != '\n') {
                            ++i;
                        }
                        ++i;
                    } break;
                }
            } break;
            case 'f': {
                face_inds res;
                res.v1.pos = to_s32(file_ptr, i);
                res.v1.uv  = to_s32(file_ptr, i);
                res.v1.nrm = to_s32(file_ptr, i);

                res.v2.pos = to_s32(file_ptr, i);
                res.v2.uv  = to_s32(file_ptr, i);
                res.v2.nrm = to_s32(file_ptr, i);

                res.v3.pos = to_s32(file_ptr, i);
                res.v3.uv  = to_s32(file_ptr, i);
                res.v3.nrm = to_s32(file_ptr, i);
                faces.push_back(res);
                while (file_ptr[i] != '\n') {
                    ++i;
                }
                ++i;
            } break;

            default: break;
        }
    }
}
}  // namespace tom
