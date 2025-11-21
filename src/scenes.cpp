#include <iostream>
#include "scenes.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"

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
    _text.setString("Cube Zone");
}

/// <summary>
/// Unloads the MenuScene
/// </summary>
void MenuScene::unload(){}