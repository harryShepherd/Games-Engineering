//
// Created by Megan McLeod on 26/11/2025.
//

#ifndef SHOOTING_COMPONENT_H
#define SHOOTING_COMPONENT_H


#pragma once
#include "ecm.hpp"
#include "physics.hpp"
#include "physics_components.hpp"
#include "game_system.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// Forward declarations
class BulletComponent;
class Scene;

/// Represents a single bullet entity with physics and graphics
class BulletComponent : public Component
{
public:
    BulletComponent(Entity* p, const sf::Vector2f& direction, float speed, float damage, float lifetime);
    void update(const float& dt) override;
    void render() override;

    float get_damage() const { return m_damage; }
    bool is_expired() const { return m_lifetime_remaining <= 0.0f; }

private:
    sf::Vector2f m_direction;
    sf::Vector2f m_velocity;
    float m_speed;
    float m_damage;
    float m_lifetime_remaining;
    float m_max_lifetime;
};

/// Base shooting component - handles shooting logic, ammo, and reloading
class ShootingComponent : public Component
{
public:
    ShootingComponent(Entity* p, Scene* scene, int clip_size, float reload_time, float fire_rate, float bullet_speed, float bullet_damage);

    void update(const float& dt) override;
    void render() override {}

    // Attempt to shoot in a given direction
    bool shoot(const sf::Vector2f& direction);

    // Check if shooting is allowed
    bool can_shoot() const;

    // Force a reload
    void reload();

    // Getters
    int get_current_ammo() const { return m_current_ammo; }
    int get_clip_size() const { return m_clip_size; }
    bool is_reloading() const { return m_reloading; }
    float get_reload_progress() const;

    // Bullet appearance customization
    void set_bullet_color(const sf::Color& color) { m_bullet_color = color; }
    void set_bullet_size(float size) { m_bullet_size = size; }

protected:
    // Scene reference for spawning bullets
    Scene* m_scene;

    // Shooting parameters
    int m_clip_size;
    int m_current_ammo;
    float m_reload_time;
    float m_reload_timer;
    float m_fire_rate;              // Shots per second
    float m_fire_cooldown;
    float m_bullet_speed;
    float m_bullet_damage;
    float m_bullet_lifetime;

    // Bullet appearance
    sf::Color m_bullet_color;
    float m_bullet_size;

    // State flags
    bool m_reloading;
    bool m_allowed_to_shoot;

    // Helper to create and spawn a bullet
    void spawn_bullet(const sf::Vector2f& direction);
};

/// Player shooting component - shoots towards mouse cursor
class PlayerShootingComponent : public ShootingComponent
{
public:
    PlayerShootingComponent(Entity* p, Scene* scene, int clip_size = 10, float reload_time = 1.5f,
                           float fire_rate = 5.0f, float bullet_speed = 400.0f, float bullet_damage = 10.0f);

    void update(const float& dt) override;

    // Set the shoot button (default is left mouse button)
    void set_shoot_key(sf::Mouse::Button button) { m_shoot_button = button; }

private:
    sf::Mouse::Button m_shoot_button;

    // Calculate shooting direction based on mouse position
    sf::Vector2f get_shooting_direction() const;
};

/// Enemy shooting component - shoots towards a target (usually the player)
class EnemyShootingComponent : public ShootingComponent
{
public:
    EnemyShootingComponent(Entity* p, Scene* scene, Entity* target, int clip_size = 8, float reload_time = 2.0f,
                          float fire_rate = 2.0f, float bullet_speed = 300.0f, float bullet_damage = 5.0f);

    void update(const float& dt) override;

    // Set shooting range - enemy won't shoot if target is beyond this distance
    void set_shooting_range(float range) { m_shooting_range = range; }

    // Set whether enemy should lead the target (predictive shooting)
    void set_predictive_shooting(bool enable) { m_predictive = enable; }

private:
    Entity* m_target;
    float m_shooting_range;
    bool m_predictive;

    // Calculate shooting direction towards target
    sf::Vector2f get_shooting_direction() const;

    // Calculate predictive shooting direction
    sf::Vector2f get_predictive_direction() const;

    // Check if target is in range and visible
    bool can_shoot_target() const;
};

#endif //SHOOTING_COMPONENT_H
