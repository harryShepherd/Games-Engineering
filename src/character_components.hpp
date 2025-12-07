#pragma once

#include "../engine/ecm.hpp"
#include "../engine/game_parameters.hpp"

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
    void regain_health(const float health);
    void increase_max_health(const float amount);

    explicit HealthComponent(Entity *p, float health);

    void update(const float& dt) override;
    void render() override; 

    HealthComponent() = delete;
};