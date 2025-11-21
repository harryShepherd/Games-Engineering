#pragma once

#include "game_system.hpp"

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
    static std::shared_ptr<Scene> testScene;
    static std::shared_ptr<Scene> menuScene;
    static std::shared_ptr<Scene> steeringScene;
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
    private:
        sf::Text _text;
        sf::Font _font;
};