#pragma once

#define DEBUG

struct params
{
    static constexpr int window_width = 700;
    static constexpr int window_height = 700;

    static constexpr float g = -9.8f;   // gravity

    static constexpr float phys_scale = 30.0f;
    static constexpr float inv_phys_scale = 1.0f / phys_scale;

    static constexpr int sub_step_count = 4;
    static constexpr float time_step = 1.0f / 60.0f;    // 60FPS
};