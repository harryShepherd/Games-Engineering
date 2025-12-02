#pragma once

#include "game_system.hpp"
#include "physics.hpp"

struct Scenes
{
    static std::shared_ptr<Scene> menuScene;
    static std::shared_ptr<Scene> basicLevelScene;
};

class MenuScene : public Scene {
    public:
        MenuScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
    private:
        sf::Text _text;
        sf::Font _font;
};

class BasicLevelScene : public Scene
{
    public:
        BasicLevelScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
    private:
        std::shared_ptr<Entity> m_player;
        std::vector<std::shared_ptr<Entity>> m_walls;
        std::vector<std::shared_ptr<Entity>> m_enemies;
        void m_load_level(const std::string &level);
        std::vector<sf::Vector2i> place_enemies_randomly(std::vector<sf::Vector2i> tiles, int enemyMax);
};