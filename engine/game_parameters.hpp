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

    static constexpr float tile_size = 40.0f;

    static constexpr float player_size[2] = {20.f,30.f};
    static constexpr float player_weight = 10.f;
    static constexpr float player_jump = 40.f;
    static constexpr float player_impulse = 100.f;
    static constexpr float player_max_vel[2] = {200.f,400.f};
    static constexpr float player_friction = 100.f;
    static constexpr float player_restitution = 0.0f;
    static constexpr sf::Keyboard::Key controls[4] = {
        sf::Keyboard::W,   // Player Up
        sf::Keyboard::A,   // Player Left
        sf::Keyboard::S,   // Player Down
        sf::Keyboard::D,   // Player Right
    };
};