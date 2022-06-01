
#ifndef TOMATO_CAMERA_HPP_
#define TOMATO_CAMERA_HPP_

#include "core.hpp"
#include "input.hpp"

namespace tom
{

enum class cam_mov_dir
{
    forward,
    backward,
    up,
    down,
    right,
    left,
};

enum class mode
{
    look_at,
    fps
};

struct camera
{
    f32 speed;
    // NOTE: in open_gl coords
    v3 pos;
    f32 angle_h;
    f32 angle_v;
    v2 mouse_pos;
    v3 up;
    v3 forward;
    v3 target_pos;

    void set_pos(v3 pos);
    v3 get_pos();
    m4 view();
};

camera camera_init();
camera camera_init(v3 pos, v3 target, v3 up);
// NOTE: the camera pos might not be affected by any global world space chanages that affect the
// cardinal axis
void camera_set_pos(camera *cam, v3 pos);
v3 camera_get_pos(camera *cam);
m4 camera_view(camera cam);
void move_camera(camera *cam, cam_mov_dir dir, f32 dt);
void move_camera_distance(camera *cam, cam_mov_dir dir, f32 dist);
void pan_camera(camera *cam, cam_mov_dir dir, f32 dt);
void mouse_look_cam(camera *cam, mouse ms, window_dims win_dims);
void orbit_cam(camera *cam, keyboard kb, mouse ms, window_dims win_dims, f32 *dist = nullptr,
               v3 *target_pos = nullptr);
void camera_look_at(camera *cam, v3 target_pos);
camera camera_init_no_angle(v3 pos, v3 forward, v3 up);

}  // namespace tom

#endif  // !TOMATO_CAMERA_HPP_