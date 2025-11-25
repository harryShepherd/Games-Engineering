#include "physics_comps.hpp"
#include "level_system.hpp"
#include "game_parameters.hpp"
#include <array>
#include <iostream>

PlatformComponent::PlatformComponent(Entity *p, const std::vector<sf::Vector2i> &tile_group,
    float friction, float restitution) :
    Component(p), m_friction(friction), m_restitution(restitution)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_staticBody;

    // Create the body
    m_body_id = b2CreateBody(Physics::get_world_id(), &body_def);
    m_create_chain_shape(tile_group);
}

void PlatformComponent::update(const float &dt) 
{
    std::cout << "PlatformComponent::update()" << std::endl;
}

void PlatformComponent::render() {}

PlatformComponent::~PlatformComponent()
{
    b2DestroyChain(m_chain_id);
    m_chain_id = b2_nullChainId;
    b2DestroyBody(m_body_id);
    m_body_id = b2_nullBodyId;
}

void PlatformComponent::m_create_chain_shape(const std::vector<sf::Vector2i> &tile_group)
{
    std::vector<b2Vec2> points;

    for(int i = 0; i < tile_group.size(); ++i)
    {
        const sf::Vector2i &tile = tile_group[i];

        std::vector<LevelSystem::Tile> neighbours =
        {
            LevelSystem::in_group({tile.x-1,tile.y-1},tile_group) ? LevelSystem::get_tile({tile.x-1,tile.y-1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x,tile.y-1},tile_group) ? LevelSystem::get_tile({tile.x,tile.y-1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x+1,tile.y-1},tile_group) ? LevelSystem::get_tile({tile.x+1,tile.y-1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x+1,tile.y},tile_group) ? LevelSystem::get_tile({tile.x+1,tile.y}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x+1,tile.y+1},tile_group) ? LevelSystem::get_tile({tile.x+1,tile.y+1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x,tile.y+1},tile_group) ? LevelSystem::get_tile({tile.x,tile.y+1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x-1,tile.y+1},tile_group) ? LevelSystem::get_tile({tile.x-1,tile.y+1}) : LevelSystem::EMPTY,
            LevelSystem::in_group({tile.x-1,tile.y},tile_group) ? LevelSystem::get_tile({tile.x-1,tile.y}) : LevelSystem::EMPTY
        };

        sf::Vector2f pos = LevelSystem::get_tile_pos(tile);

        std::vector<sf::Vector2f> pts;

        if(neighbours[0] == LevelSystem::EMPTY || neighbours[1] == LevelSystem::EMPTY || neighbours[7] == LevelSystem::EMPTY)
            pts.push_back(pos);
        if(neighbours[1] == LevelSystem::EMPTY || neighbours[2] == LevelSystem::EMPTY || neighbours[3] == LevelSystem::EMPTY)
            pts.push_back({pos.x+params::tile_size,pos.y});
        if(neighbours[3] == LevelSystem::EMPTY || neighbours[4] == LevelSystem::EMPTY || neighbours[5] == LevelSystem::EMPTY)
            pts.push_back({pos.x+params::tile_size,pos.y+params::tile_size});
        if(neighbours[5] == LevelSystem::EMPTY || neighbours[6] == LevelSystem::EMPTY || neighbours[7] == LevelSystem::EMPTY)
            pts.push_back({pos.x,pos.y+params::tile_size});
        
        for(const sf::Vector2f &pt : pts)
        {
            b2Vec2 point = Physics::sv2_to_bv2(Physics::invert_height(pt, params::window_height));
            bool already_in = false;
            for(const b2Vec2 &p : points)
            {
                if(p.x == point.x && p.y == point.y)
                {
                    already_in = true;
                    break;
                }
            }
            if (!already_in)
            {
                points.push_back(point);
            }
        }
    }

    b2Vec2 centroid = {0, 0};
    for(const b2Vec2 pt : points)
    {
        centroid.x += pt.x;
        centroid.y += pt.y;
    }

    centroid.x /= static_cast<float>(points.size());
    centroid.y /= static_cast<float>(points.size());

    // order the list of points in counter clockwise
    std::sort(points.begin(), points.end(), [&](b2Vec2 a, b2Vec2 b)
    {
        a = {a.x - centroid.x, a.y - centroid.y};
        b = {b.x - centroid.x, b.y - centroid.y};

        float angle1 = std::atan2(a.x, a.y);
        float angle2 = std::atan2(b.x, b.y);

        if(angle1 == angle2)
            return std::sqrt(a.x * a.x + a.y * a.y) > std::sqrt(b.x * b.x + b.y * b.y);
        else
            return angle1 > angle2;
    });
    points.push_back(points.front());

    b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
    material.friction = m_friction;
    material.restitution = m_restitution;
    b2ChainDef chain_def = b2DefaultChainDef();
    chain_def.count = points.size();
    chain_def.points = points.data();
    chain_def.isLoop = true;
    chain_def.materials = &material;
    chain_def.materialCount = 1;
    m_chain_id = b2CreateChain(m_body_id, &chain_def);
    std::vector<b2ShapeId> shape_ids(points.size());
    int nbr_seg = b2Chain_GetSegments(m_chain_id, shape_ids.data(), points.size());
    shape_ids.size();
}

void PhysicsComponent::update(const float &dt)
{
    m_parent->set_position(Physics::invert_height(Physics::bv2_to_sv2(b2Body_GetPosition(m_body_id)), params::window_height));
    m_parent->set_rotation((180 / 3.1415f) * b2Rot_GetAngle(b2Body_GetRotation(m_body_id)));
}

PhysicsComponent::PhysicsComponent(Entity *p, bool dyn) : Component(p), m_dynamic(dyn)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = m_dynamic ? b2_dynamicBody : b2_staticBody;
    body_def.position = Physics::sv2_to_bv2(Physics::invert_height(m_parent->get_position(), params::window_height));

    m_body_id = b2CreateBody(Physics::get_world_id(), &body_def);
}

void PhysicsComponent::set_restitution(float r)
{
    m_restitution = r;
    b2Shape_SetRestitution(m_shape_id, r);
}

void PhysicsComponent::set_friction(float f)
{
    m_friction = f;
    b2Shape_SetFriction(m_shape_id, f);
}

void PhysicsComponent::set_mass(float m)
{
    m_mass = m;
    b2Shape_SetDensity(m_shape_id, m_mass, true);
}

void PhysicsComponent::teleport(const sf::Vector2f &v)
{
    b2Rot rot;
    rot.c = 1;
    rot.s = 0;
    b2Body_SetTransform(m_body_id, Physics::sv2_to_bv2(Physics::invert_height(v, params::window_height)), rot);
}

const sf::Vector2f PhysicsComponent::get_velocity() const
{
    return Physics::bv2_to_sv2(b2Body_GetLinearVelocity(m_body_id));
}

void PhysicsComponent::set_velocity(const sf::Vector2f &v)
{
    b2Body_SetLinearVelocity(m_body_id, Physics::sv2_to_bv2(v));
}

const b2ShapeId& PhysicsComponent::get_shape_id() const
{
    return m_shape_id;
}

PhysicsComponent::~PhysicsComponent()
{
    b2DestroyShape(m_shape_id, true);
    m_shape_id = b2_nullShapeId;
    b2DestroyBody(m_body_id);
    m_body_id = b2_nullBodyId;
}

void PhysicsComponent::render() {}

void PhysicsComponent::impulse(const sf::Vector2f &i)
{
    b2Vec2 a;
    a.x = i.x;
    a.y = i.y * -1.0f;
    b2Body_ApplyLinearImpulseToCenter(m_body_id, a, true);
    auto vel = b2Body_GetLinearVelocity(m_body_id);
}

void PhysicsComponent::dampen(const sf::Vector2f &i)
{
    auto vel = b2Body_GetLinearVelocity(m_body_id);
    vel.x *= i.x;
    vel.y *= i.y;
    b2Body_SetLinearVelocity(m_body_id, vel);
}

int PhysicsComponent::get_contacts(std::array<b2ContactData, 10> &contacts) const
{
    int contact_count = b2Body_GetContactData(m_body_id, contacts.data(), 10);
    return contact_count;
}

void PhysicsComponent::create_box_shape(const sf::Vector2f &size, float mass, float friction, float restitution)
{
    m_mass = mass;
    m_friction = friction;
    m_restitution = restitution;
    //Create the fixture shape
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = m_dynamic ? m_mass : 0.f;
    shape_def.material.friction = m_friction;
    shape_def.material.restitution = m_restitution;
    b2Polygon polygon = b2MakeBox(Physics::sv2_to_bv2(size).x * 0.5f, Physics::sv2_to_bv2(size).y * 0.5f);
    m_shape_id = b2CreatePolygonShape(m_body_id,&shape_def,&polygon);
}

void PhysicsComponent::create_capsule_shape(const sf::Vector2f& size,float mass,float friction, float restitution){
    m_mass = mass;
    m_friction = friction;
    m_restitution = restitution;  
    //Create the fixture shape
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = m_dynamic ? m_mass : 0.f;
    shape_def.material.friction =   m_friction;
    shape_def.material.restitution = m_restitution;
    b2Vec2 b2_size = Physics::sv2_to_bv2(size);
    b2Capsule capsule;
    capsule.center1 = {0,b2_size.y*0.5f-b2_size.x*0.5f};
    capsule.center2 = {0,-b2_size.y*0.5f+b2_size.x*0.5f};
    capsule.radius = b2_size.x*0.5f;
    m_shape_id = b2CreateCapsuleShape(m_body_id,&shape_def,&capsule);
}

PlayerPhysicsComponent::PlayerPhysicsComponent(Entity *p, const sf::Vector2f &size) : PhysicsComponent(p, true)
{
    m_size = Physics::sv2_to_bv2(size);
    m_max_velocity = sf::Vector2f(params::player_max_vel[0], params::player_max_vel[1]);
    m_ground_speed = params::player_impulse;
    m_grounded = false;
    b2Body_EnableSleep(m_body_id, false);
}

bool PlayerPhysicsComponent::is_grounded() const
{
    std::array<b2ContactData, 10> contacts;
    int count = get_contacts(contacts);

    if(count <= 0)
        return false;
    
    const b2Vec2 &pos = b2Body_GetPosition(m_body_id);
    const float half_y = m_size.y * 0.5f;

    for(int i = 0; i < count; ++i)
    {
        if(contacts[i].manifold.normal.y == 1)
            return true;
    }

    return false;
}

void PlayerPhysicsComponent::update(const float &dt)
{
    const sf::Vector2f pos = m_parent->get_position();

    b2Vec2 b2_pos = Physics::sv2_to_bv2(Physics::invert_height(pos, params::window_height));

    // Movement
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                impulse({dt * m_ground_speed, 0});
            }
            else
            {
                impulse({-(dt * m_ground_speed), 0});
            }
        }
        else
        {
            dampen({0.9f, 1.0f});
        }

        // Jump
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            m_grounded = is_grounded();

            if(m_grounded)
            {
                set_velocity(sf::Vector2f(get_velocity().x, 0.0f));
                teleport(sf::Vector2f(pos.x, pos.y - 2.0f));
                impulse(sf::Vector2f(0, -params::player_jump));
            }
        }

        // Disable friction while we are in the air
        if(!m_grounded)
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