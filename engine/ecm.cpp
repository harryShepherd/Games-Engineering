#include "ecm.hpp"
#include "renderer.hpp"
#include <iostream>
#include "../src/character_components.hpp"

/// <summary>
/// Goes through all entities that are to be updated.
/// Checks if the entities should be deleted, and deletes if yes.
/// Otherwise, it calls the Entity's update.
/// </summary>
/// <param name="dt">Delta Time - linked to frame rate.</param>
void EntityManager::update(const float &dt)
{
    for(size_t i = 0; i < list.size(); ++i)
    {
        // Erases entities that have been marked to be deleted.
        if(list[i]->to_be_deleted())
        {
            list.erase(list.begin() + i);
            --i;
            continue;
        }
        // Updates entities if they are alive.
        if(list[i]->is_alive())
        {
            list[i]->update(dt);
        }
    }
}

/// <summary>
/// Iterates through every entity and calls their render if it is visible and alive.
/// </summary>
void EntityManager::render()
{
    for(std::shared_ptr<Entity> &ent : list)
    {
        if(ent->is_visible() && ent->is_alive())
        {
            ent->render();
        }
    }
}

/// <summary>
/// Gets the position of the entity.
/// </summary>
/// <returns>The position of the entity.</returns>
const sf::Vector2f& Entity::get_position() const
{
    return m_position;
}

/// <summary>
/// Sets the position of the entity.
/// </summary>
/// <param name="pos">The position to be set.</param>
void Entity::set_position(const sf::Vector2f &pos)
{
    m_position = pos;
}

/// <summary>
/// Updates all of the components associated with an entity.
/// </summary>
/// <param name="dt">Delta Time - Updates certain # of times per second.</param>
void Entity::update(const float &dt)
{
    for(std::shared_ptr<Component> &comp : m_components)
    {
        comp->update(dt);
    }
    if (this->m_position.y > 2000)
    {
        auto health_components = this->get_compatible_components<HealthComponent>();
        if (!health_components.empty() && health_components[0])
        {
            health_components[0]->take_damage(1000);
        }
    }
}

/// <summary>
/// Renders an individual entity.
/// </summary>
void Entity::render()
{
    for(std::shared_ptr<Component> &comp : m_components)
    {
        comp->render();
    }
}

/// <summary>
/// Checks if an entity is to be deleted.
/// </summary>
/// <returns>Whether or not the entity should be deleted.</returns>
bool Entity::to_be_deleted() const
{
    return m_for_deletion;
}

/// <summary>
/// Checks if the entity is alive.
/// </summary>
/// <returns>Returns the entity's status.</returns>
bool Entity::is_alive() const
{
    return m_alive;
}

/// <summary>
/// Sets the entity's alive status.
/// </summary>
/// <param name="alive">The target life status.</param>
void Entity::set_alive(bool alive)
{
    m_alive = alive;
}

/// <summary>
/// Marks the entity for deletion.
/// </summary>
void Entity::set_to_delete()
{
    m_for_deletion = true;
    m_alive = false;
    m_visible = false;
}

/// <summary>
/// Returns the visibility of the entity.
/// </summary>
/// <returns>Visibility</returns>
bool Entity::is_visible() const
{
    return m_visible;
}

/// <summary>
/// Sets the visibility of the entity.
/// </summary>
/// <param name="visible">The bool value for visibility.</param>
void Entity::set_visible(bool visible)
{
    m_visible = visible;
}

/// <summary>
/// Deletes the entity.
/// </summary>
Entity::~Entity()
{
    // Components can depend on each other, so we may need to make multiple
        // passes. Keep deleting components until we can't delete any more.

    int left_to_delete = 0;

    while(left_to_delete != m_components.size())
    {
        left_to_delete = m_components.size();

        // remove the component if it has 1 or less uses - no dependancies
        m_components.erase(
            remove_if(m_components.begin(), m_components.end(),
                        [](auto& ptr) { return (ptr.use_count() <= 1); }),
            m_components.end()
        );
    }

    m_components.clear();
}

Component::Component(Entity* const p) : m_parent(p), m_to_delete(false) {}

/// <summary>
/// Returns the entity's to be deleted status.
/// </summary>
/// <returns>Intention to delete status.</returns>
bool Component::to_be_deleted() const
{
    return m_to_delete;
}

Component::~Component() {}
/// <summary>
/// Returns whether the entity is facing right.
/// </summary>
/// <returns>True if facing right, false if facing left.</returns>
bool Entity::is_facing_right() const
{
    return m_facing_right;
}

/// <summary>
/// Sets the entity's facing direction.
/// </summary>
/// <param name="facing_right">True for right, false for left.</param>
void Entity::set_facing_right(bool facing_right)
{
    m_facing_right = facing_right;
}