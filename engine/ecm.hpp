#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Component; //forward declare

class Entity {
public:
    Entity() {}
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
                out.push_back(std::dynamic_pointer_cast<T>(c));
            }
        }

        return out;
    }

    const sf::Vector2f &get_position() const;
    void set_position(const sf::Vector2f &position);
    bool to_be_deleted() const;
    float get_rotation() const;
    void set_rotation(float rotation);
    bool is_alive() const;
    void set_alive(bool alive);
    void set_to_delete();
    bool is_visible() const;
    void set_visible(bool visible);
protected:
    std::vector<std::shared_ptr<Component>> m_components;
    sf::Vector2f m_position;
    float m_rotation = 0.0f;
    bool m_alive = true;            // should be updated
    bool m_visible = true;          // should be rendered
    bool m_for_deletion = false;    // should be deleted
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