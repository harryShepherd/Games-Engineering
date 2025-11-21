#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"
#include "physics.hpp"

std::shared_ptr<Scene> GameSystem::m_active_scene;
bool GameSystem::m_physics_enabled;
float GameSystem::fps;

// Central game loop.
void GameSystem::start(unsigned int w, unsigned int h, const std::string &title, const float &time_step, bool physics_enabled)
{
    sf::RenderWindow window(sf::VideoMode({w, h}), title);

    m_physics_enabled = physics_enabled;

    m_init();
    Renderer::init(window);

    sf::Event event;

    static sf::Clock clock;

    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;

    while(window.isOpen())
    {
        currentTime = clock.getElapsedTime();

        fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());

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
        m_update(currentTime.asSeconds());
        m_render();

        // V-sync
        sf::sleep(sf::seconds(time_step));

        window.display();

        previousTime = currentTime;
    }

    window.close();
    clean();
}

float GameSystem::get_fps() { return fps; }

// Sets the active scene.
void GameSystem::setActiveScene(const std::shared_ptr<Scene> &active_sc)
{
    m_active_scene = active_sc;
}

//Cleans the GameSystem.
    // Unloads the scene.
void GameSystem::clean()
{
    m_active_scene->unload();
}

// Resets
void GameSystem::reset() {}

//Initialises
void GameSystem::m_init()
{
    return;
}

//Updates elements in the GameSystem.
void GameSystem::m_update(const float &dt)
{
    m_active_scene->update(dt);

    if(m_physics_enabled)
    {
        Physics::update(Physics::time_step);
    }

    Renderer::update(dt);
}

//Renders elements in the GameSystem.
void GameSystem::m_render()
{
    m_active_scene->render();
    Renderer::render();
}

// Updates the scene.
void Scene::update(const float &dt)
{
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->update(dt);
    }
}

// Renders elements in the scene.
void Scene::render()
{
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->render();
    }
}

// Unloads the scene
void Scene::unload()
{
    m_entities.list.clear();
}

// Makes an entity
const std::shared_ptr<Entity>& Scene::make_entity() {
    std::shared_ptr<Entity> entity = std::make_shared<Entity>();
    m_entities.list.push_back(entity);
    return m_entities.list.back();
}