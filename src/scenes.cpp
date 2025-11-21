#include <iostream>
#include "scenes.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"
#include "graphic_components.hpp"
#include "control_components.hpp"

std::shared_ptr<Scene> Scenes::testScene;
std::shared_ptr<Scene> Scenes::menuScene;

void TestScene::update(const float &dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        GameSystem::reset();
    }

    #ifdef DEBUG
    std::cout << "FPS: " << GameSystem::get_fps() << std::endl;
    #endif

    Scene::update(dt);
}

void TestScene::render()
{
    Scene::render();
}

void TestScene::load()
{

}

void TestScene::unload()
{

}

/// <summary>
/// Updates the MenuScene
/// </summary>
/// <param name="dt">Delta Time - Sets frame rate</param>
void MenuScene::update(const float& dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        
    }
    Scene::update(dt);
}

/// <summary>
/// Renders the menu scene.
/// Includes text.
/// </summary>
void MenuScene::render() {
    Renderer::queue(&_text);
    Scene::render();
}

/// <summary>
/// Loads the font and text into the menu scene.
/// </summary>
void MenuScene::load() {
    _font.loadFromFile("resources/fonts/vcr_mono.ttf");
    _text.setFont(_font);
    _text.setCharacterSize(60);
    _text.setString("Cube Zone\n\n\n\nPress 1 for Steering");
}

/// <summary>
/// Unloads the MenuScene
/// </summary>
void MenuScene::unload(){}

// Loads the Steering Scene
    // Not permanent
void SteeringScene::load() {
    std::shared_ptr<Entity> player = make_entity();
    player->set_position(sf::Vector2f(params::window_height / 2, params::window_height / 2));
    std::shared_ptr<ShapeComponent> shape = player->add_component<ShapeComponent>();
    shape->set_shape<sf::RectangleShape>(10.0f);
    shape->get_shape().setFillColor(sf::Color::Red);
    player->add_component<KeyboardMovementComponent>();
}

// Unloads the scene.
void SteeringScene::unload() {
    Scene::unload();
}

void SteeringScene::update(const float& dt) {
    Scene::update(dt);
}

void SteeringScene::render() {
    Scene::render();
}
