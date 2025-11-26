#pragma once

#include "engine_utils.hpp"
#include <map>
#include <string>

#define DEBUG

struct params
{
    static constexpr int window_width = 1000;
    static constexpr int window_height = 1000;

    static constexpr float g = -9.8f;   // gravity

    static constexpr float phys_scale = 30.0f;
    static constexpr float inv_phys_scale = 1.0f / phys_scale;

    static constexpr int sub_step_count = 4;
    static constexpr float time_step = 1.0f / 60.0f;    // 60FPS

    static constexpr float tile_size = 40.0f;

    static constexpr float player_size[2] = {20.f,20.f};
    static constexpr float player_weight = 5.f;
    static constexpr float player_jump = 10.f;
    static constexpr float player_impulse = 350.f;
    static constexpr float player_max_vel[2] = {500.f,400.f};
    static constexpr float player_friction = 0.0f;
    static constexpr float player_restitution = 0.0f;

    static const std::map<std::string, sf::Keyboard::Key> controls;

    static constexpr char const* level_1 = "resources/levels/level1.txt";
    static constexpr char const* level_2 = "resources/levels/level2.txt";

    static const std::map<std::string, sf::Keyboard::Key>& getControls() {
        static const std::map<std::string, sf::Keyboard::Key> controls{
            {"Up", sf::Keyboard::W },
            {"Down", sf::Keyboard::S },
            {"Left", sf::Keyboard::A },
            {"Right", sf::Keyboard::D }
        };
        return controls;
    }
};
