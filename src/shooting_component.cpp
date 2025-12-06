#include "shooting_component.hpp"
#include "graphic_components.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"
#include "level_system.hpp"
#include "character_components.hpp"
#include "scenes.hpp"
#include <cmath>
#include <random>
#include <iostream>

BulletComponent::BulletComponent(Entity* p, const sf::Vector2f& direction, float speed, float damage, float lifetime, Entity* owner)
    : Component(p), m_direction(direction), m_speed(speed), m_damage(damage),
      m_lifetime_remaining(lifetime), m_max_lifetime(lifetime), m_owner(owner)
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
    //Clamp dt values (emergency fix)
    float safe_dt = (dt > 0.1f) ? 0.016f : dt;

    m_lifetime_remaining -= safe_dt;
    if (m_lifetime_remaining <= 0.0f)
    {
        m_parent->set_alive(false);  // Mark as dead (will be returned to pool)
        return;
    }

    // Calculate new position
    sf::Vector2f old_pos = m_parent->get_position();
    sf::Vector2f new_pos = old_pos + m_velocity * safe_dt;

    // Check for walls before moving
    sf::Vector2f direction = new_pos - old_pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0.1f)
    {
        // Normalize direction
        direction = direction / distance;

        // Check multiple points along the path
        int steps = (int)(distance / 5.0f) + 1;  // Check every 5 pixels
        for (int i = 0; i <= steps; ++i)
        {
            float t = (float)i / (float)steps;
            sf::Vector2f check_pos = old_pos + direction * (distance * t);

            if (LevelSystem::get_tile_at(check_pos) == LevelSystem::WALL)
            {
                // Hit a wall - destroy bullet
                m_parent->set_alive(false);
                return;
            }
        }
    }

    // No wall hit - move the bullet
    m_parent->set_position(new_pos);
}

void BulletComponent::render()
{
    // Rendering is handled by ShapeComponent attached to the bullet entity
}

void BulletComponent::check_collision(const std::vector<std::shared_ptr<Entity>>& entities)
{
    sf::Vector2f bullet_pos = m_parent->get_position();

    for (const auto& entity : entities)
    {
        // Don't collide with owner, dead entities, or entities marked for deletion
        if (entity.get() == m_owner || !entity->is_alive() || entity->to_be_deleted())
        {
            continue;
        }

        // Friendly fire check - Don't let enemies damage other enemies
        if (m_owner)
        {
            auto owner_enemy_shooting = m_owner->get_compatible_components<EnemyShootingComponent>();
            bool owner_is_enemy = !owner_enemy_shooting.empty();

            auto target_enemy_shooting = entity->get_compatible_components<EnemyShootingComponent>();
            bool target_is_enemy = !target_enemy_shooting.empty();

            if (owner_is_enemy && target_is_enemy)
            {
                continue;
            }
        }

        sf::Vector2f entity_pos = entity->get_position();
        sf::Vector2f diff = bullet_pos - entity_pos;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance < 20.0f) // Entity hit radius
        {
            auto health_components = entity->get_compatible_components<HealthComponent>();
            if (!health_components.empty() && health_components[0])
            {
                float health_before = health_components[0]->get_current_health();
                health_components[0]->take_damage(m_damage);
                float health_after = health_components[0]->get_current_health();

                if (health_before > 0.0f && health_after <= 0.0f && m_on_kill_callback)
                {
                    m_on_kill_callback(bullet_pos);
                }
            }

            // Destroy the bullet
            m_parent->set_alive(false);  // Mark as dead (will be returned to pool)
            return;
        }
    }
}

// Shooting component

ShootingComponent::ShootingComponent(Entity* p, Scene* scene, int clip_size, float reload_time,
                                     float fire_rate, float bullet_speed, float bullet_damage)
    : Component(p), m_scene(scene), m_clip_size(clip_size), m_current_ammo(clip_size),
      m_reload_time(reload_time), m_reload_timer(0.0f),
      m_fire_rate(fire_rate), m_fire_cooldown(0.0f),
      m_bullet_speed(bullet_speed), m_bullet_damage(bullet_damage),
      m_bullet_lifetime(3.0f), m_reloading(false), m_allowed_to_shoot(true),
      m_bullet_color(sf::Color::White), m_bullet_size(5.0f)
{
}

void ShootingComponent::update(const float& dt)
{
    // Clamp dt to prevent burst fire from huge dt values
    float safe_dt = (dt > 0.1f) ? 0.016f : dt;

    // Update fire cooldown
    if (m_fire_cooldown > 0.0f)
    {
        m_fire_cooldown -= safe_dt;
        if (m_fire_cooldown < 0.0f)
        {
            m_fire_cooldown = 0.0f;
        }
    }

    // Update reload timer
    if (m_reloading)
    {
        m_reload_timer += safe_dt;

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
        return;
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
        return;
    }

    // Try to get bullet from pool (if scene is BasicLevelScene)
    BasicLevelScene* levelScene = dynamic_cast<BasicLevelScene*>(m_scene);
    std::shared_ptr<Entity> bullet;

    if (levelScene)
    {
        // Get bullet from pool
        bullet = levelScene->get_bullet_from_pool();
    }
    else
    {
        bullet = m_scene->make_entity();
    }

    // Position bullet at shooter's location
    bullet->set_position(m_parent->get_position());
    bullet->set_alive(true);

    auto shape_components = bullet->get_compatible_components<ShapeComponent>();
    if (!shape_components.empty() && shape_components[0])
    {
        auto& shape = shape_components[0]->get_shape();
        if (auto* circle = dynamic_cast<sf::CircleShape*>(&shape))
        {
            circle->setRadius(m_bullet_size);
            circle->setOrigin(m_bullet_size, m_bullet_size);
        }
        shape.setFillColor(m_bullet_color);
    }
    else
    {
        auto shape = bullet->add_component<ShapeComponent>();
        shape->set_shape<sf::CircleShape>(m_bullet_size);
        shape->get_shape().setFillColor(m_bullet_color);
        shape->get_shape().setOrigin(m_bullet_size, m_bullet_size);
    }

    // Remove old BulletComponents before adding new one
    bullet->remove_components_by_type<BulletComponent>();

    // Add fresh bullet component
    bullet->add_component<BulletComponent>(
        direction,
        m_bullet_speed,
        m_bullet_damage,
        m_bullet_lifetime,
        m_parent
    );
}


PlayerShootingComponent::PlayerShootingComponent(Entity* p, Scene* scene, int clip_size, float reload_time,
                                                 float fire_rate, float bullet_speed, float bullet_damage)
    : ShootingComponent(p, scene, clip_size, reload_time, fire_rate, bullet_speed, bullet_damage)
{
    set_bullet_color(sf::Color::Cyan);
    set_bullet_size(3.0f);
}

void PlayerShootingComponent::update(const float& dt)
{
    ShootingComponent::update(dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
        reload();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        sf::Vector2f direction = get_shooting_direction();
        shoot(direction);
    }
}

sf::Vector2f PlayerShootingComponent::get_shooting_direction() const
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(Renderer::getWindow());

    sf::Vector2f mouse_world_pos = Renderer::getWindow().mapPixelToCoords(mouse_pos, Renderer::getView());

    sf::Vector2f direction = mouse_world_pos - m_parent->get_position();

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
      m_target(target), m_shooting_range(500.0f), m_predictive(false),
      m_shoot_chance(0.3f), m_random_delay_min(0.5f), m_random_delay_max(2.0f),
      m_random_delay_timer(0.0f)
{
    set_bullet_color(sf::Color(255, 100, 0));
    set_bullet_size(2.5f);

    // Override bullet damage
    m_bullet_damage = bullet_damage;

    // Generate unique random delay per enemy
    // Use entity pointer as seed to make each enemy different
    std::random_device rd;
    std::mt19937 gen(rd() + reinterpret_cast<uintptr_t>(p));
    std::uniform_real_distribution<float> dis(m_random_delay_min, m_random_delay_max);
    m_random_delay_timer = dis(gen);
}

void EnemyShootingComponent::update(const float& dt)
{
    if (this->m_parent->is_alive())
    {
        // Clamp dt to prevent burst fire
        float safe_dt = (dt > 0.1f) ? 0.016f : dt;

        // Update base shooting logic
        ShootingComponent::update(safe_dt);

        // Update random delay timer with clamped dt
        if (m_random_delay_timer > 0.0f)
        {
            m_random_delay_timer -= safe_dt;
        }

        // Check if we should shoot at target
        if (can_shoot_target())
        {
            sf::Vector2f direction = m_predictive ? get_predictive_direction() : get_shooting_direction();

            if (shoot(direction))
            {
                // Shot was successful, generate new random delay
                generate_random_delay();
            }
        }
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

    sf::Vector2f direction = target_pos - m_parent->get_position();

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction = direction / length;
    }

    return direction;
}

bool EnemyShootingComponent::can_shoot_target()
{
    // Don't shoot if target is dead or doesn't exist
    if (!m_target || !m_target->is_alive() || !can_shoot())
    {
        return false;
    }

    // Check random delay timer
    if (m_random_delay_timer > 0.0f)
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

    // Check line of sight
    if (!has_line_of_sight())
    {
        return false;
    }

    // Add random chance - use unique generator per enemy
    // Each enemy gets their own generator seeded with their pointer address
    std::mt19937 gen(reinterpret_cast<uintptr_t>(m_parent) + static_cast<unsigned>(m_random_delay_timer * 1000.0f));
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    if (dis(gen) > m_shoot_chance)
    {
        return false;
    }

    return true;
}

bool EnemyShootingComponent::has_line_of_sight() const
{
    if (!m_target)
    {
        return false;
    }

    sf::Vector2f start = m_parent->get_position();
    sf::Vector2f end = m_target->get_position();

    // Calculate direction and distance
    sf::Vector2f direction = end - start;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 1.0f)
    {
        return true; // Too close to matter
    }

    // Normalise direction
    direction = direction / distance;

    // Ray-cast from enemy to target, checking tiles along the way
    // Step size should be smaller than tile size to not miss walls
    float step_size = params::tile_size * 0.5f;
    int num_steps = static_cast<int>(distance / step_size);

    for (int i = 1; i < num_steps; ++i)
    {
        sf::Vector2f check_pos = start + direction * (step_size * i);

        // Check if this position contains a wall
        LevelSystem::Tile tile = LevelSystem::get_tile_at(check_pos);

        if (tile == LevelSystem::WALL)
        {
            return false; // Wall blocking line of sight
        }
    }

    return true; // Clear line of sight
}

void EnemyShootingComponent::generate_random_delay()
{
    // Use unique generator per enemy - seeded with entity pointer
    std::random_device rd;
    std::mt19937 gen(rd() + reinterpret_cast<uintptr_t>(m_parent));
    std::uniform_real_distribution<float> dis(m_random_delay_min, m_random_delay_max);

    m_random_delay_timer = dis(gen);
}