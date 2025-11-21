#include "physics.hpp"

b2WorldId Physics::m_world_id;

void Physics::initialise()
{
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2({0.0f, gravity});
    m_world_id = b2CreateWorld(&world_def);
}

void Physics::shutdown()
{
    b2DestroyWorld(m_world_id);
}

void Physics::update(const float &dt)
{
    b2World_Step(m_world_id, time_step, sub_step_count);
}

b2WorldId Physics::get_world_id()
{
    return m_world_id;
}

b2ContactEvents Physics::get_contact_events()
{
    return b2World_GetContactEvents(m_world_id);
}

// Convert from b2Vec2 to Vector2f
const sf::Vector2f Physics::bv2_to_sv2(const b2Vec2 &in)
{
    return sf::Vector2f(in.x * phys_scale, in.y * phys_scale);
}

// Convert from Vector2f to b2Vec2
const b2Vec2 Physics::sv2_to_bv2(const sf::Vector2f &in)
{
    return {in.x * phys_scale_inv, in.y * phys_scale_inv};
}

// Convert from screenspace.y to physics.y
const sf::Vector2f Physics::invert_height(const sf::Vector2f &in, const int &game_height)
{
    return sf::Vector2f(in.x, game_height - in.y);
}