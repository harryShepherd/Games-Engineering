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

//Create a Box2D body with a box fixture
b2BodyId Physics::create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size) {
  b2BodyDef body_def = b2DefaultBodyDef();
  //Is Dynamic(moving), or static(Stationary)
  body_def.type = dynamic ? b2_dynamicBody : b2_staticBody;
  body_def.position = sv2_to_bv2(position);
  //Create the body
  b2BodyId body_id = b2CreateBody(world_id,&body_def);

  //Create the fixture shape
  b2ShapeDef shape_def = b2DefaultShapeDef();
  shape_def.density = dynamic ? 10.f : 0.f;
  shape_def.material.friction =  dynamic ? 0.8f : 1.f;
  shape_def.material.restitution = 1.0f;
  b2Polygon polygon = b2MakeBox(sv2_to_bv2(size).x * 0.5f, sv2_to_bv2(size).y * 0.5f);
  b2CreatePolygonShape(body_id,&shape_def,&polygon);

  return body_id;
}

b2BodyId Physics::create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs){
  return create_physics_box(world_id,dynamic,rs->getPosition(),rs->getSize());
}