#pragma once
#include "ecm.hpp"
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

class Scene
{
public:
    Scene() = default;

    virtual ~Scene() = default;
    virtual void update(const float &dt);
    virtual void render();
    virtual void load() = 0;
    virtual void unload();

    const std::shared_ptr<Entity>& make_entity();
    std::vector<std::shared_ptr<Entity>> &getEntities() { return m_entities.list; }

protected:
    EntityManager m_entities;
};

class GameSystem
{
public:
    static void start(unsigned int w, unsigned int h, const std::string &title, const float &time_step, bool physics_enabled);
    static void clean();
    static void reset();
    static void setActiveScene(const std::shared_ptr<Scene>& active_sc);
    static void moveCamera(sf::Vector2f pos);
    static float get_fps();

private:
    static void m_init();
    static void m_update(const float &dt);
    static void m_render();
    static std::shared_ptr<Scene> m_active_scene;
    static bool m_physics_enabled;
    static float fps;
};