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
};