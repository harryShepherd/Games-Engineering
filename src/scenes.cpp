#include <iostream>
#include <random>
#include "scenes.hpp"
#include <renderer.hpp>
#include <game_parameters.hpp>
#include "graphic_components.hpp"
#include "ai_components.hpp"
#include <level_system.hpp>
#include "control_components.hpp"

std::shared_ptr<Scene> Scenes::menuScene;
std::shared_ptr<Scene> Scenes::basicLevelScene;

/// <summary>
/// Updates the MenuScene
/// </summary>
/// <param name="dt">Delta Time - Sets frame rate</param>
void MenuScene::update(const float& dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) 
    {
        Scenes::basicLevelScene = std::make_shared<BasicLevelScene>();
        GameSystem::setActiveScene(Scenes::basicLevelScene);
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
    _text.setString("Cube Zone\n\n\nPress 0 for Basic Level");
}

/// <summary>
/// Unloads the MenuScene
/// </summary>
void MenuScene::unload(){}

void BasicLevelScene::m_load_level(const std::string &level, int enemyCount)
{
    LevelSystem::load_level(level, params::tile_size);

    m_player = make_entity();
    m_player->set_position(LevelSystem::get_start_pos());

    // Create player
    std::shared_ptr<ShapeComponent> shape = m_player->add_component<ShapeComponent>();
    shape->set_shape<sf::RectangleShape>(sf::Vector2f(params::player_size[0],params::player_size[1]));
    shape->get_shape().setFillColor(sf::Color::Yellow);
    shape->get_shape().setOrigin(sf::Vector2f(params::player_size[0]/2.f,params::player_size[1]/2.f));

    // Add player physics component
    std::shared_ptr<PlayerControlComponent> component = 
    m_player->add_component<PlayerControlComponent>(sf::Vector2f(params::player_size[0],params::player_size[1]));

    component->create_box_shape({params::player_size[0], params::player_size[1]}, 
        params::player_weight, params::player_friction, params::player_restitution);

    // Create walls
    std::vector<std::vector<sf::Vector2i>> wall_groups = LevelSystem::get_groups(LevelSystem::WALL);
    for (const std::vector<sf::Vector2i> &walls : wall_groups) {
        m_walls.push_back(make_entity());
        m_walls.back()->add_component<PlatformComponent>(walls);
    }

    // Create enemies
    std::vector<sf::Vector2i> emptyTiles = LevelSystem::find_tiles(LevelSystem::Tile::EMPTY);

    std::vector<sf::Vector2i> enemyPositions = place_enemies_randomly(emptyTiles, enemyCount);

    for (const sf::Vector2i enemy_pos : enemyPositions)
    {
        m_enemies.push_back(make_entity());
        m_enemies.back()->set_position(sf::Vector2f(enemy_pos.x * 20.0f, enemy_pos.y * 20.0f));

        std::shared_ptr<ShapeComponent> shape = m_enemies.back()->add_component<ShapeComponent>();
        shape->set_shape<sf::RectangleShape>(sf::Vector2f(params::enemy_size[0], params::enemy_size[1]));
        shape->get_shape().setFillColor(sf::Color::Blue);
        shape->get_shape().setOrigin(sf::Vector2f(params::enemy_size[0] / 2.f, params::enemy_size[1] / 2.f));

        std::shared_ptr<EnemyControlComponent> component = m_enemies.back()->add_component<EnemyControlComponent>(sf::Vector2f(params::enemy_size[0], params::enemy_size[1]));
        component->create_box_shape({ params::enemy_size[0]-3, params::enemy_size[1]-3 },
            params::enemy_weight, params::enemy_friction, params::enemy_restitution);
        component->set_target(m_player);
    }
}

void BasicLevelScene::update(const float& dt) {
    Scene::update(dt);
    m_entities.update(dt);

    GameSystem::moveCamera(m_player->get_position());

    if(LevelSystem::get_tile_at(m_player->get_position()) == LevelSystem::END){
        unload();
        m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), params::enemy_count);
    }
}

void BasicLevelScene::render() {
    LevelSystem::render(Renderer::getWindow());
    Scene::render();
    m_entities.render();
}

void BasicLevelScene::load() {
    int enemyCount = params::enemy_count;
    m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), enemyCount);
}

void BasicLevelScene::unload() {
    Scene::unload();
    m_player.reset();
    m_walls.clear();
}

std::vector<sf::Vector2i> BasicLevelScene::place_enemies_randomly(std::vector<sf::Vector2i> tiles, int enemyMax) {
    std::vector<sf::Vector2i> enemyPositions;
    std::random_device random_device;
    std::default_random_engine engine(random_device());
    std::uniform_int_distribution<> distribution(0, tiles.size() - 1);

    for (size_t i = 0; i < enemyMax; i++)
    {
        enemyPositions.push_back(tiles[distribution(engine)]);
    }

    return enemyPositions;
}

std::string BasicLevelScene::pick_level_randomly() {
    std::random_device random_device;
    std::default_random_engine engine(random_device());
    std::uniform_int_distribution<> distribution(1, params::getLevels().size() - 1);

    std::string level = params::getLevels().at(distribution(engine));
    return level;
}