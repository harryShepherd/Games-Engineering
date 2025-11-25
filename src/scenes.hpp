#pragma once

#include "game_system.hpp"
#include "physics.hpp"

class TestScene : public Scene
{
public:
    TestScene() = default;
    void update(const float &dt) override;
    void render() override;
    void load() override;
    void unload() override;

private:
    std::shared_ptr<Entity> m_player;
};

struct Scenes
{
    static std::shared_ptr<Scene> menuScene;
    static std::shared_ptr<Scene> steeringScene;
    static std::shared_ptr<Scene> physicsScene;
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

class SteeringScene : public Scene {
    public:
        SteeringScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
};

class PhysicsScene : public Scene
{
    public:
      PhysicsScene() = default;
      void update(const float &dt) override;
      void render() override;
      void load()override;
      void unload() override;
    private:
      b2WorldId world_id;
      std::vector<b2BodyId> bodies;
      std::vector<std::shared_ptr<sf::RectangleShape>> sprites;
};

class BasicLevelScene : public Scene
{
    public:
        BasicLevelScene() = default;
        void update(const float& dt) override;
        void render() override;
        void load() override;
        void unload() override;
};