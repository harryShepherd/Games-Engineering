#pragma once

#include "game_system.hpp"
#include "physics.hpp"
#include <queue>

struct Scenes
{
    static std::shared_ptr<Scene> menuScene;
    static std::shared_ptr<Scene> tutorialScene;
    static std::shared_ptr<Scene> basicLevelScene;
    static std::shared_ptr<Scene> deathScene;
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

class TutorialScene : public Scene {
    public:
        TutorialScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
    private:
        sf::Text _text;
        sf::Font _font;
};

class DeathScene : public Scene {
    public:
        DeathScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
    private:
        sf::Text _death_text;
        sf::Font _death_font;
};

class BasicLevelScene : public Scene
{
    public:
        BasicLevelScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;

        // Bullet pool methods - PUBLIC so ShootingComponent can access
        void initialise_bullet_pool(int pool_size = 50);
        std::shared_ptr<Entity> get_bullet_from_pool();
        void return_bullet_to_pool(std::shared_ptr<Entity> bullet);

        // Enemy death callback (changed to public so BulletComponent can call it)
        void on_enemy_death(sf::Vector2f death_position);

    private:
        std::shared_ptr<Entity> m_player;
        std::vector<std::shared_ptr<Entity>> m_walls;
        std::vector<std::shared_ptr<Entity>> m_enemies;
        std::shared_ptr<Entity> m_portal;
        bool m_portal_spawned;

        // Bullet pool - pre-created bullets for reuse
        std::vector<std::shared_ptr<Entity>> m_bullet_pool;
        std::queue<std::shared_ptr<Entity>> m_available_bullets;

        // Active bullets tracking for efficient iteration
        std::vector<std::shared_ptr<Entity>> m_active_bullets;

        // Cached collision targets (rebuilt when enemies die)
        std::vector<std::shared_ptr<Entity>> m_collision_targets;

        // Cached alive enemy count (updated on death instead of counting every frame)
        int m_alive_enemy_count = 0;

        // Reload UI
        sf::Text m_reload_text;
        sf::Font m_reload_font;

        // Bullet count limiting (prevent too many bullets)
        int m_max_bullets = 100;

        // Track last enemy position for portal spawn
        sf::Vector2f m_last_enemy_position;

        void m_load_level(const std::string& level, int enemyCount);
        std::vector<sf::Vector2i> place_enemies_randomly(std::vector<sf::Vector2i> tiles, int enemyCount);
        void add_enemies(int enemyCount, std::vector<sf::Vector2i> position);
        std::string pick_level_randomly();
        void spawn_portal();
        int count_alive_enemies() const;
        int count_bullets() const;

        // Rebuild collision targets when enemies die
        void rebuild_collision_targets();
};