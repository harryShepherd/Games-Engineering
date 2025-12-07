#include "character_components.hpp"
#include <iostream>

void HealthComponent::kill()
{
    std::cout << "I am dead." << std::endl;
    m_parent->set_to_delete();
}

float HealthComponent::get_current_health() { return current_health; }

float HealthComponent::get_max_health() { return max_health; }

void HealthComponent::take_damage(const float dmg)
{
    current_health -= dmg;
    
    // we are dead.
    if(current_health <= 0.0f) 
    {
        kill();
    }
}

void HealthComponent::regain_health(const float health)
{
    if (this->current_health + health >= this->max_health)
    {
        this->current_health = this->max_health;
    }
    else {
        this->current_health = current_health + health;
    }
}

HealthComponent::HealthComponent(Entity *p, float health) :
    Component(p), max_health(health), current_health(health)
{}

void HealthComponent::update(const float& dt)
{}

void HealthComponent::render() {}