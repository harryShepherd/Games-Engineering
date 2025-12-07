#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

class Physics
{
public:
    static void initialise();
    static void shutdown();
    static void update(const float& dt);

    static b2WorldId get_world_id();
    static b2ContactEvents get_contact_events();

    static const sf::Vector2f bv2_to_sv2(const b2Vec2& in);
    static const b2Vec2 sv2_to_bv2(const sf::Vector2f& in);
    static const sf::Vector2f invert_height(const sf::Vector2f& in, const int &game_height);

    static b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size);
    static b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs);


    static constexpr float phys_scale = 30.0f; // 30 pixels = 1 meter
    static constexpr float phys_scale_inv = 1.0f / phys_scale;
    static constexpr float time_step = 1.0f / 120.0f; // 120fps
    static constexpr float gravity = -9.8f; // Earth-like gravity intended
    static constexpr int sub_step_count = 4; // box2d parameter

private:
    static b2WorldId m_world_id;
};