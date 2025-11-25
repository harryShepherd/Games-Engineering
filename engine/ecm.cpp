#include "ecm.hpp"
#include "renderer.hpp"
#include <iostream>

void EntityManager::update(const float &dt)
{
    std::cout << "Updating " << list.size() << " entities" << std::endl;
    for(size_t i = 0; i < list.size(); ++i)
    {
        if(list[i]->to_be_deleted())
        {
            std::cout << "list[" << i << "] is to be deleted." << std::endl;
            list.erase(list.begin() + i);
            --i;
            continue;
        }
        if(list[i]->is_alive())
        {
            std::cout << "list[" << i << "] is to be updated." << std::endl;
            list[i]->update(dt);
        }
    }
}

void EntityManager::render()
{
    for(std::shared_ptr<Entity> &ent : list)
    {
        if(ent->is_visible())
        {
            ent->render();
        }
    }
}

const sf::Vector2f& Entity::get_position() const
{
    return m_position;
}

void Entity::set_position(const sf::Vector2f &pos)
{
    m_position = pos;
}

void Entity::update(const float &dt)
{
    for(std::shared_ptr<Component> &comp : m_components)
    {
        comp->update(dt);
    }
}

void Entity::render()
{
    for(std::shared_ptr<Component> &comp : m_components)
    {
        comp->render();
    }
}

bool Entity::to_be_deleted() const
{
    return m_for_deletion;
}

float Entity::get_rotation() const
{
    return m_rotation;
}

void Entity::set_rotation(float rotation)
{
    m_rotation = rotation;
}

bool Entity::is_alive() const
{
    return m_alive;
}

void Entity::set_alive(bool alive)
{
    m_alive = alive;
}

void Entity::set_to_delete()
{
    m_for_deletion = true;
    m_alive = false;
    m_visible = false;
}

bool Entity::is_visible() const
{
    return m_visible;
}

void Entity::set_visible(bool visible)
{
    m_visible = visible;
}

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

    if(m_components.size() > 0)
    {
        throw std::runtime_error("Can't delete entity - someone is grabbing a component.");
    }

    m_components.clear();
}

Component::Component(Entity* const p) : m_parent(p), m_to_delete(false) {}

bool Component::to_be_deleted() const
{
    return m_to_delete;
}

Component::~Component() {}