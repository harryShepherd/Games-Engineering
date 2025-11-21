#include "ecm.hpp"
#include "physics.hpp"

class PlatformComponent : public Component
{
public:
    PlatformComponent(Entity *p, const std::vector<sf::Vector2i> &tile_group,
        float friction = 40.0f, float restitution = 0.2f);
    void update(const float &dt) override;
    void render() override;
    const b2ShapeId &get_shape_id() const;
    ~PlatformComponent() override;

protected:
    b2BodyId m_body_id;
    b2ChainId m_chain_id;
    float m_friction;
    float m_restitution;
    void m_create_chain_shape(const std::vector<sf::Vector2i> &tile_group);
};

class PhysicsComponent : public Component
{
public:
    PhysicsComponent(Entity *p, bool dyn);
    const b2ShapeId &get_shape_id() const;
    int get_contacts(std::array<b2ContactData, 10>& contacts) const;
    const sf::Vector2f get_velocity() const;
    void set_restitution(float r);
    void set_friction(float r);
    void set_mass(float m);
    void update(const float &dt) override;
    void render() override;
    void impulse(const sf::Vector2f &i);
    void dampen(const sf::Vector2f &s);
    void set_velocity(const sf::Vector2f &v);
    void teleport(const sf::Vector2f &v);
    void create_box_shape(const sf::Vector2f &size, float mass, float friction, float restitution);
    void create_capsule_shape(const sf::Vector2f &size, float mass, float friction, float restitution);

    ~PhysicsComponent() override;

protected:
    b2BodyId m_body_id;
    b2ShapeId m_shape_id;
    const bool m_dynamic;
    float m_friction;
    float m_restitution;
    float m_mass;
};

class PlayerPhysicsComponent : public PhysicsComponent
{
public:
    void update(const float &dt) override;
    explicit PlayerPhysicsComponent(Entity *p, const sf::Vector2f &size);
    PlayerPhysicsComponent() = delete;

protected:
    b2Vec2 m_size;
    sf::Vector2f m_max_velocity;
    bool m_grounded;
    float m_ground_speed;

    bool is_grounded() const;
};