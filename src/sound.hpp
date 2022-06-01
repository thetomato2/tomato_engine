
#ifndef TOMATO_SOUND_HPP_
#define TOMATO_SOUND_HPP_

#include "core.hpp"

namespace tom
{

struct sound_output
{
    s32 samples_per_sec;
    u32 running_sample_index;
    s32 bytes_per_sample;
    DWORD secondary_buf_size;
    s32 latency_sample_count;
};

struct sound_output_buffer
{
    s32 samples_per_second;
    s32 sample_count;
    s16 *samples;
    s32 tone_hertz;
};

struct sound_state
{
    f32 frames_of_audio_latency;
    sound_output output;
    IAudioClient *audio_client;
    IAudioRenderClient *audio_render_client;
    IAudioClock *audio_clock;
};

void init_WASAPI(s32 samples_per_second, s32 buffer_size_in_samples);
void fill_sound_buffer(sound_output &sound_output, s32 samples_to_write,
                       sound_output_buffer &source_buffer);

}  // namespace tom

#endif  // !TOMATO_SOUND_HPP_