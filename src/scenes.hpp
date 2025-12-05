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
    std::shared_ptr<Entity> m_portal;
    bool m_portal_spawned;

    // Death screen
    bool m_player_dead;
    sf::Text m_death_text;
    sf::Font m_death_font;

    // Reload UI
    sf::Text m_reload_text;
    sf::Font m_reload_font;

    // Track last enemy position for portal spawn
    sf::Vector2f m_last_enemy_position;

    void m_load_level(const std::string &level, int enemyCount);
    std::vector<sf::Vector2i> place_enemies_randomly(std::vector<sf::Vector2i> tiles, int enemyMax);
    std::string pick_level_randomly();
    void spawn_portal();
    int count_alive_enemies() const;
};