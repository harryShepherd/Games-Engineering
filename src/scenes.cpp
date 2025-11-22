#include <iostream>
#include <random>
#include "scenes.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"
#include "graphic_components.hpp"
#include "control_components.hpp"
#include "ai_components.hpp"
#include "engine_utils.hpp"

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
        GameSystem::setActiveScene(Scenes::steeringScene);
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
    _font.loadFromFile(EngineUtils::GetRelativePath("resources/fonts/vcr_mono.ttf"));
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
    shape->set_shape<sf::RectangleShape>(sf::Vector2f(10.0f, 10.0f));
    shape->get_shape().setFillColor(sf::Color::Red);
    player->add_component<KeyboardMovementComponent>();

    
    std::random_device dev;
    std::default_random_engine engine(dev());
    std::uniform_real_distribution<float> x_dist(0.0f, params::window_width);
    std::uniform_real_distribution<float> y_dist(0.0f, params::window_height);

    for (size_t e = 0; e < 100; e++)
    {
        std::shared_ptr<Entity> enemy = make_entity();
        enemy->set_position(sf::Vector2f(x_dist(engine), y_dist(engine)));
        std::shared_ptr<ShapeComponent> shape = enemy->add_component<ShapeComponent>();
        shape->set_shape<sf::RectangleShape>(sf::Vector2f(10.0f, 10.0f));
        shape->get_shape().setFillColor(sf::Color::Blue);
        enemy->add_component<SteeringComponent>(player.get(), 50.0f);
    }
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
