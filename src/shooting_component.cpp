//
// Created by Megan McLeod on 26/11/2025.
//

#include "shooting_component.hpp"
#include "graphic_components.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"
#include <cmath>


BulletComponent::BulletComponent(Entity* p, const sf::Vector2f& direction, float speed, float damage, float lifetime)
    : Component(p), m_direction(direction), m_speed(speed), m_damage(damage),
      m_lifetime_remaining(lifetime), m_max_lifetime(lifetime)
{
    // Normalize direction
    float length = std::sqrt(m_direction.x * m_direction.x + m_direction.y * m_direction.y);
    if (length > 0.0f)
    {
        m_direction = m_direction / length;
    }

    m_velocity = m_direction * m_speed;
}

void BulletComponent::update(const float& dt)
{
    // Update lifetime
    m_lifetime_remaining -= dt;

    if (m_lifetime_remaining <= 0.0f)
    {
        m_parent->set_to_delete();
        return;
    }

    // Update position
    sf::Vector2f new_pos = m_parent->get_position() + m_velocity * dt;

    // Check bounds - delete if out of screen
    if (new_pos.x < 0.0f || new_pos.x > params::window_width ||
        new_pos.y < 0.0f || new_pos.y > params::window_height)
    {
        m_parent->set_to_delete();
        return;
    }

    m_parent->set_position(new_pos);

    // Calculate rotation based on direction
    float angle = std::atan2(m_direction.y, m_direction.x) * 180.0f / 3.14159f;
    m_parent->set_rotation(angle);
}

void BulletComponent::render()
{
    // Rendering is handled by ShapeComponent attached to the bullet entity
}

// ==================== ShootingComponent Implementation ====================

ShootingComponent::ShootingComponent(Entity* p, Scene* scene, int clip_size, float reload_time,
                                     float fire_rate, float bullet_speed, float bullet_damage)
    : Component(p), m_scene(scene), m_clip_size(clip_size), m_current_ammo(clip_size),
      m_reload_time(reload_time), m_reload_timer(0.0f),
      m_fire_rate(fire_rate), m_fire_cooldown(0.0f),
      m_bullet_speed(bullet_speed), m_bullet_damage(bullet_damage),
      m_bullet_lifetime(3.0f), m_reloading(false), m_allowed_to_shoot(true),
      m_bullet_color(sf::Color::Yellow), m_bullet_size(4.0f)
{
}

void ShootingComponent::update(const float& dt)
{
    // Update fire cooldown
    if (m_fire_cooldown > 0.0f)
    {
        m_fire_cooldown -= dt;
        if (m_fire_cooldown < 0.0f)
        {
            m_fire_cooldown = 0.0f;
        }
    }

    // Update reload timer
    if (m_reloading)
    {
        m_reload_timer += dt;

        if (m_reload_timer >= m_reload_time)
        {
            // Reload complete
            m_current_ammo = m_clip_size;
            m_reloading = false;
            m_reload_timer = 0.0f;
            m_allowed_to_shoot = true;
        }
    }

    // Auto-reload if ammo is empty and not already reloading
    if (m_current_ammo <= 0 && !m_reloading)
    {
        reload();
    }
}

bool ShootingComponent::shoot(const sf::Vector2f& direction)
{
    if (!can_shoot())
    {
        return false;
    }

    // Spawn bullet
    spawn_bullet(direction);

    // Consume ammo
    m_current_ammo--;

    // Start fire cooldown
    m_fire_cooldown = 1.0f / m_fire_rate;

    // If out of ammo, start reload
    if (m_current_ammo <= 0)
    {
        reload();
    }

    return true;
}

bool ShootingComponent::can_shoot() const
{
    return m_allowed_to_shoot &&
           !m_reloading &&
           m_current_ammo > 0 &&
           m_fire_cooldown <= 0.0f;
}

void ShootingComponent::reload()
{
    if (m_reloading)
    {
        return; // Already reloading
    }

    m_reloading = true;
    m_reload_timer = 0.0f;
    m_allowed_to_shoot = false;
}

float ShootingComponent::get_reload_progress() const
{
    if (!m_reloading)
    {
        return 1.0f;
    }

    return m_reload_timer / m_reload_time;
}

void ShootingComponent::spawn_bullet(const sf::Vector2f& direction)
{
    if (!m_scene)
    {
        return; // Can't spawn bullets without scene reference
    }

    // Create bullet entity through the scene
    const std::shared_ptr<Entity>& bullet = m_scene->make_entity();

    // Position bullet at shooter's position
    bullet->set_position(m_parent->get_position());

    // Add visual component (small circle for bullet)
    auto shape = bullet->add_component<ShapeComponent>();
    shape->set_shape<sf::CircleShape>(m_bullet_size);
    shape->get_shape().setFillColor(m_bullet_color);
    shape->get_shape().setOrigin(m_bullet_size, m_bullet_size);

    // Add bullet component for movement and lifetime
    bullet->add_component<BulletComponent>(
        direction,
        m_bullet_speed,
        m_bullet_damage,
        m_bullet_lifetime
    );
}

// void ShootingComponent::clean_bullets()
// {
//  Placeholder function incase required. Bullets will be cleaned by Entity Manager
// }


PlayerShootingComponent::PlayerShootingComponent(Entity* p, Scene* scene, int clip_size, float reload_time,
                                                 float fire_rate, float bullet_speed, float bullet_damage)
    : ShootingComponent(p, scene, clip_size, reload_time, fire_rate, bullet_speed, bullet_damage),
      m_shoot_button(sf::Mouse::Left)
{
    // Player bullets are yellow
    set_bullet_color(sf::Color::Yellow);
    set_bullet_size(4.0f);
}

void PlayerShootingComponent::update(const float& dt)
{
    // Update base shooting logic
    ShootingComponent::update(dt);

    // Check for manual reload input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
        reload();
    }

    // Check for shooting input
    if (sf::Mouse::isButtonPressed(m_shoot_button))
    {
        sf::Vector2f direction = get_shooting_direction();
        shoot(direction);
    }
}

sf::Vector2f PlayerShootingComponent::get_shooting_direction() const
{
    // Get mouse position in window coordinates
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(Renderer::getWindow());
    sf::Vector2f mouse_world_pos(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y));

    // Calculate direction from player to mouse
    sf::Vector2f direction = mouse_world_pos - m_parent->get_position();

    // Normalize
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction = direction / length;
    }

    return direction;
}


EnemyShootingComponent::EnemyShootingComponent(Entity* p, Scene* scene, Entity* target, int clip_size,
                                               float reload_time, float fire_rate,
                                               float bullet_speed, float bullet_damage)
    : ShootingComponent(p, scene, clip_size, reload_time, fire_rate, bullet_speed, bullet_damage),
      m_target(target), m_shooting_range(500.0f), m_predictive(false)
{
    // Enemy bullets are red/orange
    set_bullet_color(sf::Color(255, 100, 50)); // Orange-ish
    set_bullet_size(3.0f);
}

void EnemyShootingComponent::update(const float& dt)
{
    // Update base shooting logic
    ShootingComponent::update(dt);

    // Check if we should shoot at target
    if (can_shoot_target())
    {
        sf::Vector2f direction = m_predictive ? get_predictive_direction() : get_shooting_direction();
        shoot(direction);
    }
}

sf::Vector2f EnemyShootingComponent::get_shooting_direction() const
{
    if (!m_target)
    {
        return sf::Vector2f(0.0f, 0.0f);
    }

    // Calculate direction from enemy to target
    sf::Vector2f direction = m_target->get_position() - m_parent->get_position();

    // Normalize
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction = direction / length;
    }

    return direction;
}

sf::Vector2f EnemyShootingComponent::get_predictive_direction() const
{
    if (!m_target)
    {
        return sf::Vector2f(0.0f, 0.0f);
    }

    // Try to get target's physics component for velocity
    auto target_physics = m_target->get_compatible_components<PhysicsComponent>();

    sf::Vector2f target_pos = m_target->get_position();

    // If target has physics, predict where they'll be
    if (!target_physics.empty() && target_physics[0])
    {
        sf::Vector2f target_velocity = target_physics[0]->get_velocity();

        // Calculate time for bullet to reach target
        sf::Vector2f to_target = target_pos - m_parent->get_position();
        float distance = std::sqrt(to_target.x * to_target.x + to_target.y * to_target.y);
        float time_to_target = distance / m_bullet_speed;

        // Predict target position
        target_pos = target_pos + target_velocity * time_to_target;
    }

    // Calculate direction to predicted position
    sf::Vector2f direction = target_pos - m_parent->get_position();

    // Normalize
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction = direction / length;
    }

    return direction;
}

bool EnemyShootingComponent::can_shoot_target() const
{
    if (!m_target || !can_shoot())
    {
        return false;
    }

    // Check if target is in range
    sf::Vector2f to_target = m_target->get_position() - m_parent->get_position();
    float distance = std::sqrt(to_target.x * to_target.x + to_target.y * to_target.y);

    if (distance > m_shooting_range)
    {
        return false;
    }

    // Additional checks could/should include:
    // - Line of sight check
    // - Target being on screen
    // - Random chance to add variation to enemy behavior

    return true;
}