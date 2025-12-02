#include "control_components.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"
#include "physics.hpp"
#include "ai_components.hpp"
#include <array>


PlayerControlComponent::PlayerControlComponent(Entity* p, const sf::Vector2f& size) : PhysicsComponent(p, true)
{
    m_size = Physics::sv2_to_bv2(size);
    m_max_velocity = sf::Vector2f(params::player_max_vel[0], params::player_max_vel[1]);
    m_ground_speed = params::player_impulse;
    m_grounded = false;
    b2Body_EnableSleep(m_body_id, false);
}

bool PlayerControlComponent::is_grounded() const
{
    std::array<b2ContactData, 10> contacts;
    int count = get_contacts(contacts);

    if (count <= 0)
        return false;

    const b2Vec2& pos = b2Body_GetPosition(m_body_id);
    const float half_y = m_size.y * 0.5f;

    for (int i = 0; i < count; ++i)
    {
        if (contacts[i].manifold.normal.y == 1)
            return true;
    }

    return false;
}

void PlayerControlComponent::update(const float& dt)
{
    const sf::Vector2f pos = m_parent->get_position();
    b2Vec2 b2_pos = Physics::sv2_to_bv2(Physics::invert_height(pos, params::window_height));

    if (sf::Keyboard::isKeyPressed(params::getControls().at("Left")))
    {
        m_direction.x = -1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(params::getControls().at("Right")))
    {
        m_direction.x = 1.0f;
    }
    else
    {
        m_direction.x = 0.0f;
    }

    set_velocity({ m_ground_speed * m_direction.x, get_velocity().y });

    if (sf::Keyboard::isKeyPressed(params::getControls().at("Up")))
    {
        m_grounded = is_grounded();

        if (m_grounded)
        {
            m_grounded = false;
            impulse({ 0.0f, -params::player_jump });
        }
    }

    dampen({ 1.0f, 1.0f });

    // Disable friction while we are in the air
    if (!m_grounded)
    {
        m_grounded = is_grounded();
        set_friction(0.0f);
    }
    else
    {
        set_friction(m_friction);
    }

    // Clamp velocity
    sf::Vector2f v = get_velocity();
    v.x = copysign(std::min(abs(v.x), m_max_velocity.x), v.x);
    v.y = copysign(std::min(abs(v.y), m_max_velocity.y), v.y);
    set_velocity(v);

    PhysicsComponent::update(dt);
}

EnemyControlComponent::EnemyControlComponent(Entity* e, const sf::Vector2f& size) : PhysicsComponent(e, true)
{
    m_size = Physics::sv2_to_bv2(size);
    m_max_velocity = sf::Vector2f(params::player_max_vel[0], params::player_max_vel[1]);
    m_ground_speed = 300.0f;
    m_grounded = false;
    b2Body_EnableSleep(m_body_id, false);
}

bool EnemyControlComponent::is_grounded() const
{
    std::array<b2ContactData, 10> contacts;
    int count = get_contacts(contacts);

    if (count <= 0)
        return false;

    const b2Vec2& pos = b2Body_GetPosition(m_body_id);
    const float half_y = m_size.y * 0.5f;

    for (int i = 0; i < count; ++i)
    {
        if (contacts[i].manifold.normal.y == 1)
            return true;
    }

    return false;
}

SteeringOutput output;

void EnemyControlComponent::update(const float& dt)
{
    // Need to make the enemy consider whether or not its movement is valid
    // This should be based on whether or not the next tile is empty.
    const sf::Vector2f pos = m_parent->get_position();
    b2Vec2 b2_pos = Physics::sv2_to_bv2(Physics::invert_height(pos, params::window_height));

    auto distance = [](const sf::Vector2f& a, const sf::Vector2f& b) -> float {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
        };
    //If target is further than 100 pixels away then seek.
    if (distance(m_parent->get_position(), target->get_position()) > 150.0f) {
        output = SteeringBehaviours::seek(target->get_position(), m_parent->get_position());
        seeking = true;
    }
    //If target is closer than 50 pixels away then flee.
    else if (distance(m_parent->get_position(), target->get_position()) < 100.0f) {
        output = SteeringBehaviours::flee(target->get_position(), m_parent->get_position());
        seeking = false;
    }

    m_direction.x = output.direction.x;

    set_velocity({ m_ground_speed * m_direction.x, get_velocity().y });

    if (seeking && (output.direction.y < -0.1) || !seeking) {
        m_grounded = is_grounded();
        if (m_grounded)
        {
            m_grounded = false;
            impulse({ 0.0f, -params::player_jump });
        }
    }

    dampen({ 1.0f, 1.0f });

    // Disable friction while in the air
    if (!m_grounded)
    {
        m_grounded = is_grounded();
        //set_friction(0.0f);
    }
    else
    {
        set_friction(m_friction);
    }

    // Clamp velocity
    sf::Vector2f v = get_velocity();
    v.x = copysign(std::min(abs(v.x), m_max_velocity.x), v.x);
    v.y = copysign(std::min(abs(v.y), m_max_velocity.y), v.y);
    set_velocity(v);

    PhysicsComponent::update(dt);
}

void EnemyControlComponent::set_target(std::shared_ptr<Entity> targetEntity) {
    target = targetEntity;
}