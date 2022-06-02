#include "camera.hpp"

namespace tom
{
internal void camera_init_angle(camera *cam)
{
#if Z_UP
    v3 h_target = vec::normalize({ cam->forward.x, 0.0f, cam->forward.z });
    f32 angle   = math::to_degree(asin(abs(h_target.z)));
#else
    v3 h_target = vec::normalize({ cam->forward.x, 0.0f, -cam->forward.y });
    f32 angle   = math::to_degree(asin(abs(-h_target.y)));
#endif

    if (h_target.z >= 0.0f) {
        if (h_target.x >= 0.0f)
            cam->angle_h = 360.0f - angle;
        else
            cam->angle_h = 180.0f + angle;
    } else {
        if (h_target.x >= 0.0f)
            cam->angle_h = angle;
        else
            cam->angle_h = 180.0f - angle;
    }

    cam->angle_v = -math::to_degree(asin(cam->forward.y));
}

internal camera camera_default_init()
{
    camera result;

    result.speed      = 1.0f;
    result.angle_h    = 0.0f;
    result.angle_v    = 0.0f;
    result.pos        = { 0.0f, 0.0f, 0.0f };
    result.target_pos = {};
    result.up         = { 0.0f, 1.0f, 0.0f };
    result.forward    = { 0.0f, 0.0f, -1.0f };

    return result;
}

camera camera_init()
{
    camera result = camera_default_init();
    camera_init_angle(&result);
    return result;
}

camera camera_init(v3 pos, v3 forward, v3 up)
{
    camera result = camera_default_init();

    result.pos = pos;
    // NOTE: can't assume passed in vectors are normalized
    result.forward = vec::normalize(forward);
    result.up      = vec::normalize(up);
    camera_init_angle(&result);

    return result;
}

camera camera_init_no_angle(v3 pos, v3 forward, v3 up)
{
    camera result = camera_default_init();

    result.pos = pos;
    // NOTE: can't assume passed in vectors are normalized
    result.forward = vec::normalize(forward);
    result.up      = vec::normalize(up);

    return result;
}

void move_camera(camera *cam, cam_mov_dir dir, f32 dt)
{
    switch (dir) {
        case cam_mov_dir::forward: {
            cam->pos += cam->forward * cam->speed * dt;
        } break;
        case cam_mov_dir::backward: {
            cam->pos -= cam->forward * cam->speed * dt;
        } break;
        case cam_mov_dir::up: {
            cam->pos.y += cam->speed * dt;
        } break;
        case cam_mov_dir::down: {
            cam->pos.y -= cam->speed * dt;
        } break;
        case cam_mov_dir::right: {
            v3 res = vec::cross(cam->up, cam->forward);
            res    = vec::normalize(res);
            res *= cam->speed * dt;
            cam->pos += res;

        } break;
        case cam_mov_dir::left: {
            v3 res = vec::cross(cam->forward, cam->up);
            res    = vec::normalize(res);
            res *= cam->speed * dt;
            cam->pos += res;
        } break;
        default: {
        } break;
    }
}

void move_camera_distance(camera *cam, cam_mov_dir dir, f32 dist)
{
    switch (dir) {
        case cam_mov_dir::forward: {
            cam->pos += cam->forward * dist;
        } break;
        case cam_mov_dir::backward: {
            cam->pos -= cam->forward * dist;
        } break;
        case cam_mov_dir::up: {
            cam->pos.y += dist;
        } break;
        case cam_mov_dir::down: {
            cam->pos.y -= dist;
        } break;
        case cam_mov_dir::right: {
            v3 res = vec::cross(cam->up, cam->forward);
            res    = vec::normalize(res);
            res *= dist;
            cam->pos += res;

        } break;
        case cam_mov_dir::left: {
            v3 res = vec::cross(cam->forward, cam->up);
            res    = vec::normalize(res);
            res *= dist;
            cam->pos += res;
        } break;
        default: {
        } break;
    }
}

void pan_camera(camera *cam, cam_mov_dir dir, f32 dt)
{
    f32 new_speed = cam->speed / 2.0f;

    switch (dir) {
        case cam_mov_dir::forward: {
            cam->pos += cam->forward * new_speed * dt;
            cam->target_pos += cam->forward * new_speed * dt;
        } break;
        case cam_mov_dir::backward: {
            cam->pos -= cam->forward * new_speed * dt;
            cam->target_pos -= cam->forward * new_speed * dt;
        } break;
        case cam_mov_dir::up: {
            cam->pos.y += new_speed * dt;
            cam->target_pos.y += new_speed * dt;
        } break;
        case cam_mov_dir::down: {
            cam->pos.y -= new_speed * dt;
            cam->target_pos.y -= new_speed * dt;
        } break;
        case cam_mov_dir::right: {
            v3 res = vec::cross(cam->up, cam->forward);
            res    = vec::normalize(res);
            res *= new_speed * dt;
            cam->pos += res;
            cam->target_pos += res;

        } break;
        case cam_mov_dir::left: {
            v3 res = vec::cross(cam->forward, cam->up);
            res    = vec::normalize(res);
            res *= cam->speed * dt;
            cam->pos += res;
            cam->target_pos += res;
        } break;
        default: {
        } break;
    }
}

void mouse_look_cam(camera *cam, mouse ms, window_dims win_dims)
{
    v2 ms_delta = ms.get_delta();
    if (ms.pos.x < 0.0f || ms.pos.x > scast(f32, win_dims.width) || ms.pos.u < 0.0f ||
        ms.pos.y > scast(f32, win_dims.height))
        ms_delta = {};

    f32 angle_h = ms_delta.x / 20.0f;
    f32 angle_v = ms_delta.y / 20.0f;

    v3 n = vec::normalize(cam->forward);
    v3 u = vec::normalize(vec::cross(cam->up, n));
    v3 v = vec::cross(n, u);

    if (is_key_down(ms.l)) {
        cam->forward = qua::rotate(cam->forward, u, angle_v);
        cam->forward = qua::rotate(cam->forward, cam->up, angle_h);
    }
    // cam.target = qua::rotate(cam.target, cam.up, 1.0f);
}

void orbit_cam(camera *cam, keyboard kb, mouse ms, window_dims win_dims, f32 *dist, v3 *target_pos)
{
    if (target_pos) cam->target_pos = *target_pos;

    v2 ms_delta = ms.get_delta();
    if (ms.pos.x < 0.0f || ms.pos.x > scast(f32, win_dims.width) || ms.pos.u < 0.0f ||
        ms.pos.y > scast(f32, win_dims.height))
        ms_delta = {};

    f32 d1;
    if (dist) {
        d1 = *dist;
    } else {
        d1 = vec::distance(cam->pos, cam->target_pos);
    }

    f32 mouse_sens  = 0.0005f / (1 / d1);
    f32 scroll_sens = 50.0f;

#if 0
    if (is_key_down(ms.m)) {
        if (ms_delta.x > 0.0f) {
            f32 spd         = abs(ms_delta.x) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::left;
            is_key_down(kb.left_shift) ? pan_camera(cam, dir, spd) : move_camera(cam, dir, spd);
        }

        if (ms_delta.x < 0.0f) {
            f32 spd         = abs(ms_delta.x) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::right;
            is_key_down(kb.left_shift) ? pan_camera(cam, dir, spd) : move_camera(cam, dir, spd);
        }

        if (ms_delta.y > 0.0f) {
            f32 spd         = abs(ms_delta.y) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::up;
            is_key_down(kb.left_shift) ? pan_camera(cam, dir, spd) : move_camera(cam, dir, spd);
        }

        if (ms_delta.y < 0.0f) {
            f32 spd         = abs(ms_delta.y) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::down;
            is_key_down(kb.left_shift) ? pan_camera(cam, dir, spd) : move_camera(cam, dir, spd);
        }
    }
#else
    if (is_key_down(ms.m)) {
        if (ms_delta.x > 0.0f) {
            f32 spd         = abs(ms_delta.x) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::left;
            move_camera(cam, dir, spd);
        }

        if (ms_delta.x < 0.0f) {
            f32 spd         = abs(ms_delta.x) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::right;
            move_camera(cam, dir, spd);
        }

        if (ms_delta.y > 0.0f) {
            f32 spd         = abs(ms_delta.y) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::up;
            move_camera(cam, dir, spd);
        }

        if (ms_delta.y < 0.0f) {
            f32 spd         = abs(ms_delta.y) * mouse_sens;
            cam_mov_dir dir = cam_mov_dir::down;
            move_camera(cam, dir, spd);
        }
    }
#endif

    cam->forward = vec::normalize(cam->pos - cam->target_pos);
    f32 d2       = vec::distance(cam->pos, cam->target_pos);

    if (d1 > d2) {
        // moved away
        move_camera_distance(cam, cam_mov_dir::forward, d1 - d2);
    } else if (d1 < d2) {
        // moved toward
        move_camera_distance(cam, cam_mov_dir::backward, d2 - d1);
    }
    if (ms.scroll > 0) {
        f32 spd = scroll_sens * mouse_sens;
        move_camera(cam, cam_mov_dir::backward, spd);
    }

    if (ms.scroll < 0) {
        f32 spd = scroll_sens * mouse_sens;
        move_camera(cam, cam_mov_dir::forward, spd);
    }
}

void camera_look_at(camera *cam, v3 target_pos)
{
    cam->target_pos = target_pos;

    f32 d1 = vec::distance(cam->pos, cam->target_pos);

    // forward
    cam->forward = vec::normalize(cam->pos - cam->target_pos);
    f32 d2       = vec::distance(cam->pos, cam->target_pos);

    v3 n = vec::normalize(cam->forward);
    v3 u = vec::normalize(vec::cross(cam->up, n));
    v3 v = vec::cross(n, u);

    cam->up = v;

    // NOTE: this keeps a constant distance
    // TODO: better way?
    if (d1 > d2) {
        // moved away
        move_camera_distance(cam, cam_mov_dir::forward, d1 - d2);
    } else if (d1 < d2) {
        // moved toward
        move_camera_distance(cam, cam_mov_dir::backward, d2 - d1);
    }
}

m4 camera_view(camera cam)
{
    v3 n = vec::normalize(cam.forward);
    v3 u = vec::normalize(vec::cross(cam.up, n));
    v3 v = vec::cross(n, u);

    m4 result = mat::row_3x3(u, v, n) * mat::translate(-cam.pos);

    return result;
}

void camera_set_pos(camera *cam, v3 pos)
{
#if Z_UP
    cam->pos.x = pos.x;
    cam->pos.y = pos.z;
    cam->pos.z = -pos.y;
#else
    cam->pos = pos;
#endif
}

v3 camera_get_pos(camera *cam)
{
#if Z_UP
    return { cam->pos.x, -cam->pos.z, cam->pos.y };
#else
    return cam->pos;
#endif
}

void camera::set_pos(v3 pos)
{
    camera_set_pos(this, pos);
}

v3 camera::get_pos()
{
    return camera_get_pos(this);
}

m4 camera::view()
{
    return camera_view(*this);
}

}  // namespace tom