#pragma once

#include "ecm.hpp"
#include "game_parameters.hpp"

class HealthComponent : public Component
{
protected:
    float max_health;
    float current_health;

    void kill();

public:
    float get_max_health();
    float get_current_health();

    void take_damage(const float dmg);

    explicit HealthComponent(Entity *p, float health);

    void update(const float& dt) override;
    void render() override; 

    HealthComponent() = delete;
};