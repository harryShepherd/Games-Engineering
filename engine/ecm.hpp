#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Component;

class Entity {
public:
    Entity() = default;
    virtual ~Entity();

    virtual void update(const float &dt);
    virtual void render();

    template<typename T, typename... Targs>
    std::shared_ptr<T> add_component(Targs... params)
    {
        static_assert(std::is_base_of<Component, T>::value, "T is not a component");
        std::shared_ptr<T> ptr(std::make_shared<T>(this, params...));
        
        m_components.push_back(ptr);
        
        return ptr;
    }

    template<typename T>
    const std::vector<std::shared_ptr<T>> get_components() const
    {
        static_assert(std::is_base_of<Component, T>::value, "T is not a component");
        std::vector<std::shared_ptr<T>> out;

        for (const auto c : m_components)
        {
            out.push_back(std::dynamic_pointer_cast<T>(c));
        }
    }

    template<typename T>
    const std::vector<std::shared_ptr<T>> get_compatible_components()
    {
        static_assert(std::is_base_of<Component, T>::value, "T is not a component");
        std::vector<std::shared_ptr<T>> out;

        for(auto component : m_components)
        {
            // check if the component is a derivative of T
            auto dd = dynamic_cast<T*>(&(*component));

            // if the component is a derivative of T
            if (dd)
            {
                out.push_back(std::dynamic_pointer_cast<T>(component));
            }
        }

        return out;
    }

    //Remove a specific component from this entity
    template<typename T>
    void remove_component(std::shared_ptr<T> component)
    {
        static_assert(std::is_base_of<Component, T>::value, "T is not a component");

        // Find and remove the component
        m_components.erase(
            std::remove_if(m_components.begin(), m_components.end(),
                [&component](std::shared_ptr<Component>& comp) {
                    return comp == component;
                }),
            m_components.end()
        );
    }

    // Remove all components of a specific type
    template<typename T>
    void remove_components_by_type()
    {
        static_assert(std::is_base_of<Component, T>::value, "T is not a component");

        m_components.erase(
            std::remove_if(m_components.begin(), m_components.end(),
                [](std::shared_ptr<Component>& comp) {
                    return dynamic_cast<T*>(comp.get()) != nullptr;
                }),
            m_components.end()
        );
    }

    const sf::Vector2f &get_position() const;
    void set_position(const sf::Vector2f &position);
    bool to_be_deleted() const;
    bool is_alive() const;
    void set_alive(bool alive);
    void set_to_delete();
    bool is_visible() const;
    void set_visible(bool visible);

    // Facing direction for sprite mirroring (true = right, false = left)
    bool is_facing_right() const;
    void set_facing_right(bool facing_right);

protected:
    std::vector<std::shared_ptr<Component>> m_components;
    sf::Vector2f m_position;
    bool m_alive = true;            // should be updated
    bool m_visible = true;          // should be rendered
    bool m_for_deletion = false;    // should be deleted
    bool m_facing_right = true;     // for sprite mirroring
};

struct EntityManager
{
    std::vector<std::shared_ptr<Entity>> list;
    void update(const float &dt);
    void render();
};

class Component {
public:
    Component() = delete;
    bool to_be_deleted() const;
    virtual void update(const float& dt) = 0;
    virtual void render() = 0;
    virtual ~Component();
protected:
    Entity* const m_parent;
    bool m_to_delete;
    explicit Component(Entity* const p);

};