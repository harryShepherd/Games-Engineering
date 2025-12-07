#include "character_components.hpp"
#include <iostream>

/// <summary>
/// Marks the entity for deletion.
/// </summary>
void HealthComponent::kill()
{
    std::cout << "I am dead." << std::endl;
    m_parent->set_to_delete();
}

/// <summary>
/// Gets the current health.
/// </summary>
/// <returns>The current health.</returns>
float HealthComponent::get_current_health() { return current_health; }

/// <summary>
/// Gets the maximum health.
/// </summary>
/// <returns>The maximum health.</returns>
float HealthComponent::get_max_health() { return max_health; }

/// <summary>
/// Takes the input damage.
/// </summary>
/// <param name="dmg">The input damage as a float.</param>
void HealthComponent::take_damage(const float dmg)
{
    // Kills the entity if the damage exceeds their health.
    if (current_health - dmg <= 0.0f)
    {
        kill();
    }
    else {
        current_health -= dmg;
    }
}

/// <summary>
/// Regains health corresponding to the input.
/// </summary>
/// <param name="health">The input health as a float.</param>
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

/// <summary>
/// Creates a new HealthComponent.
/// </summary>
/// <param name="p">The entity to be assigned a HealthComponent.</param>
/// <param name="health">The float for health.</param>
HealthComponent::HealthComponent(Entity *p, float health) :
    Component(p), max_health(health), current_health(health)
{}

/// <summary>
/// Updates the health component.
/// </summary>
/// <param name="dt">Delta Time - Linked to frame rate.</param>
void HealthComponent::update(const float& dt)
{}

/// <summary>
/// Any rendering associated with the Health component.
/// </summary>
void HealthComponent::render() {}
