#include "physics.hpp"

b2WorldId Physics::m_world_id;

/// <summary>
/// Intialises the physics.
/// </summary>
void Physics::initialise()
{
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2({0.0f, gravity});
    m_world_id = b2CreateWorld(&world_def);
}

/// <summary>
/// Shuts down the physics.
/// </summary>
void Physics::shutdown()
{
    b2DestroyWorld(m_world_id);
}

/// <summary>
/// Updates the physics.
/// </summary>
/// <param name="dt">Delta Time - Linked to frame rate.</param>
void Physics::update(const float &dt)
{
    b2World_Step(m_world_id, time_step, sub_step_count);
}

/// <summary>
/// Gets the Physics world id
/// </summary>
/// <returns>Returns the world id.</returns>
b2WorldId Physics::get_world_id()
{
    return m_world_id;
}

/// <summary>
/// Gets the contact events.
/// </summary>
/// <returns>Returns the contact events associated with the world id.</returns>
b2ContactEvents Physics::get_contact_events()
{
    return b2World_GetContactEvents(m_world_id);
}

/// <summary>
/// Convert from b2Vec2 to Vector2f
/// </summary>
/// <param name="in">The b2Vec2 to convert.</param>
/// <returns>The converted Vector2f</returns>
const sf::Vector2f Physics::bv2_to_sv2(const b2Vec2 &in)
{
    return sf::Vector2f(in.x * phys_scale, in.y * phys_scale);
}

/// <summary>
/// Convert from Vector2f to b2vec2
/// </summary>
/// <param name="in">The Vector2f to convert.</param>
/// <returns>The converted b2vec2</returns>
const b2Vec2 Physics::sv2_to_bv2(const sf::Vector2f &in)
{
    return {in.x * phys_scale_inv, in.y * phys_scale_inv};
}

/// <summary>
/// Change from screenspace Y to physics Y.
/// </summary>
/// <param name="in">Screenspace Y</param>
/// <param name="game_height">Physics Y</param>
/// <returns></returns>
const sf::Vector2f Physics::invert_height(const sf::Vector2f &in, const int &game_height)
{
    return sf::Vector2f(in.x, game_height - in.y);
}

/// <summary>
/// Create a Box2D body with a box fixture
/// </summary>
/// <param name="world_id">The Physics world id.</param>
/// <param name="dynamic">Bool for dynamicism.</param>
/// <param name="position">The position of the body.</param>
/// <param name="size">The size of the body.</param>
/// <returns>The body id.</returns>
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

/// <summary>
/// Creates the physics box.
/// </summary>
/// <param name="world_id">The Physics world id.</param>
/// <param name="dynamic">Bool for dynamicism.</param>
/// <param name="rs">The rectangle shape for the physics box.</param>
/// <returns></returns>
b2BodyId Physics::create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs){
  return create_physics_box(world_id,dynamic,rs->getPosition(),rs->getSize());
}