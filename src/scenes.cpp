#include <iostream>
#include "scenes.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"

std::shared_ptr<Scene> Scenes::testScene;

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