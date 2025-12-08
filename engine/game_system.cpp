#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"
#include "physics.hpp"

std::shared_ptr<Scene> GameSystem::m_active_scene;
bool GameSystem::m_physics_enabled;
float GameSystem::fps;

/// <summary>
/// Central game loop
/// </summary>
/// <param name="w">Starting window width</param>
/// <param name="h">Starting window height</param>
/// <param name="title">Title of the window</param>
/// <param name="time_step">Time step value - linked to frame rate</param>
/// <param name="physics_enabled">Set whether or not physics should be enabled.</param>
void GameSystem::start(unsigned int w, unsigned int h, const std::string &title, const float &time_step, bool physics_enabled)
{
    // Sets the initial window dimensions.
    sf::RenderWindow window({w, h}, title);
    sf::View view = window.getDefaultView();

    // Sets whether or not physics is enabled.
    m_physics_enabled = physics_enabled;

    m_init();
    // Intialises the window.
    Renderer::init(window, view);

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
            // Handles closing the window properly.
            if(event.type == sf::Event::Closed)
            {
                window.close();
                clean();
                return;
            }

            // Resizes the window and the view in the window.
            if(event.type == sf::Event::Resized)
            {
                view.setSize({
                    static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height)
                });
                window.setView(view);
            }
        }

        // One method of closing the window while debugging.
        #ifdef DEBUG
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
        #endif // DEBUG

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

/// <summary>
/// Gets the FPS.
/// </summary>
/// <returns>The FPS</returns>
float GameSystem::get_fps() { return fps; }

/// <summary>
/// Moves the camera.
/// </summary>
/// <param name="pos">The position to move the camera to.</param>
void GameSystem::moveCamera(sf::Vector2f pos)
{
    Renderer::getView().setCenter(pos);
    Renderer::getWindow().setView(Renderer::getView());
}

/// <summary>
/// Sets the active scene.
/// </summary>
/// <param name="active_sc">The scene to set the active scene to.</param>
void GameSystem::setActiveScene(const std::shared_ptr<Scene> &active_sc)
{
    m_active_scene = active_sc;
    m_active_scene->load();
}

/// <summary>
/// Cleans the GameSystem.
/// Unloads the scene.
/// </summary>
void GameSystem::clean()
{
    m_active_scene->unload();
}

/// <summary>
/// Resets the scene.
/// </summary>
void GameSystem::reset() {}

/// <summary>
/// Initialises the scene.
/// </summary>
void GameSystem::m_init()
{
    return;
}

/// <summary>
/// Updates the elements in the scene.
/// </summary>
/// <param name="dt">Delta Time - Linked to frame rate.</param>
void GameSystem::m_update(const float &dt)
{
    // Updates the scene.
    m_active_scene->update(dt);

    // Updates the physics when the scene is updated.
    if(m_physics_enabled)
    {
        Physics::update(Physics::time_step);
    }

    // Updates rendered elements.
    Renderer::update(dt);
}

/// <summary>
/// Renders elements in the GameSystem.
/// </summary>
void GameSystem::m_render()
{
    // Renders in the scene.
    m_active_scene->render();

    // Renders the sprites in the scene.
    Renderer::render();
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt">Delta Time - Linked to frame rate.</param>
void Scene::update(const float &dt)
{
    std::cout << "FPS: " << GameSystem::get_fps() << std::endl;

    // Updates every entity in the scene.
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->update(dt);
    }
}

/// <summary>
/// Renders every entity in the scene.
/// </summary>
void Scene::render()
{
    for(std::shared_ptr<Entity> &ent : m_entities.list)
    {
        ent->render();
    }
}

/// <summary>
/// Unloads the scene.
/// Clears the entity list.
/// </summary>
void Scene::unload()
{
    m_entities.list.clear();
}

/// <summary>
/// Makes a basic new entity.
/// </summary>
/// <returns></returns>
const std::shared_ptr<Entity>& Scene::make_entity() {
    std::shared_ptr<Entity> entity = std::make_shared<Entity>();
    m_entities.list.push_back(entity);
    return m_entities.list.back();
}