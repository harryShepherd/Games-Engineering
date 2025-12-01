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

    std::cout << "I took " << dmg << " damage and have " << current_health << " health remaining." << std::endl;
    
    // we are dead.
    if(current_health <= 0.0f) 
    {
        kill();
    }
}

HealthComponent::HealthComponent(Entity *p, float health) :
    Component(p), max_health(health), current_health(health)
{
    std::cout << "I have " << current_health << " health" << std::endl;
}

void HealthComponent::update(const float& dt)
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        take_damage(2.0f);
    }
};

void HealthComponent::render() {}