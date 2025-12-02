#pragma once
#include "physics_components.hpp"
#include "ai_components.hpp"

class PlayerControlComponent : public PhysicsComponent
{
    public:
        void update(const float& dt) override;
        explicit PlayerControlComponent(Entity* p, const sf::Vector2f& size);
        PlayerControlComponent() = delete;

    protected:
        b2Vec2 m_size;
        sf::Vector2f m_max_velocity;
        sf::Vector2f m_direction{ 0.0f, 0.0f };
        bool m_grounded;
        float m_ground_speed;

        bool is_grounded() const;
};
class EnemyControlComponent : public PhysicsComponent
{
    public:
        void update(const float& dt) override;
        explicit EnemyControlComponent(Entity* e, const sf::Vector2f& size);
        EnemyControlComponent() = delete;
        void set_target(std::shared_ptr<Entity> targetEntity);
    protected:
        b2Vec2 m_size;
        sf::Vector2f m_max_velocity;
        sf::Vector2f m_direction{ 0.0f, 0.0f };
        bool m_grounded;
        float m_ground_speed;
        SteeringOutput output;
        std::shared_ptr<Entity> target;
        bool m_seeking;

        bool is_grounded() const;
};