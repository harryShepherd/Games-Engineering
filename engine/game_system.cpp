#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"
#include "physics.hpp"

std::shared_ptr<Scene> GameSystem::m_active_scene;
bool GameSystem::m_physics_enabled;

void GameSystem::start(unsigned int w, unsigned int h, const std::string &title, const float &time_step, bool physics_enabled)
{
    sf::RenderWindow window(sf::VideoMode({w, h}), title);

    m_physics_enabled = physics_enabled;

    m_init();
    Renderer::init(window);

    sf::Event event;

    while(window.isOpen())
    {
        static sf::Clock clock;
        float dt = clock.restart().asSeconds();

        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
                clean();
                return;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        window.clear();
        m_update(dt);
        m_render();

        // V-sync
        sf::sleep(sf::seconds(time_step));

        window.display();
    }

    window.close();
    clean();
}

void GameSystem::setActiveScene(const std::shared_ptr<Scene> &active_sc)
{
    m_active_scene = active_sc;
}

void GameSystem::clean()
{
    m_active_scene->unload();
}

void GameSystem::reset() {}

void GameSystem::m_init()
{
    return;
}

void GameSystem::m_update(const float &dt)
{
    m_active_scene->update(dt);

    if(m_physics_enabled)
    {
        Physics::update(Physics::time_step);
    }

    Renderer::update(dt);
}

void GameSystem::m_render()
{
    m_active_scene->render();
    Renderer::render();
}

void Scene::update(const float &dt)
{
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->update(dt);
    }
}

void Scene::render()
{
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->render();
    }
}

void Scene::unload()
{
    m_entities.list.clear();
}