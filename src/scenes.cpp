#include <iostream>
#include <random>
#include "scenes.hpp"
#include <renderer.hpp>
#include <game_parameters.hpp>
#include "graphic_components.hpp"
#include "ai_components.hpp"
#include <level_system.hpp>
#include "control_components.hpp"
#include "shooting_component.hpp"
#include "character_components.hpp"

std::shared_ptr<Scene> Scenes::menuScene;
std::shared_ptr<Scene> Scenes::basicLevelScene;

/// <summary>
/// Updates the MenuScene
/// </summary>
/// <param name="dt">Delta Time - Sets frame rate</param>
void MenuScene::update(const float& dt) {
    // Static flag to reset camera once when menu becomes active
    static bool camera_reset_this_session = false;

    if (!camera_reset_this_session)
    {
        GameSystem::moveCamera(sf::Vector2f(params::window_width / 2.0f, params::window_height / 2.0f));
        camera_reset_this_session = true;
    }

    // Static variable to track if key was pressed last frame (prevents holding key)
    static bool key_was_pressed = false;

    bool key_is_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num0);

    // Only trigger on key press (not hold)
    if (key_is_pressed && !key_was_pressed)
    {
        // Create a fresh scene (important when returning from death)
        Scenes::basicLevelScene = std::make_shared<BasicLevelScene>();
        Scenes::basicLevelScene->set_enemy_count(9);
        GameSystem::setActiveScene(Scenes::basicLevelScene);
        camera_reset_this_session = false; // Reset flag so camera resets next time we come back to menu
    }

    key_was_pressed = key_is_pressed;

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

    // Center the menu text
    sf::FloatRect textBounds = _text.getLocalBounds();
    _text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    _text.setPosition(params::window_width / 2.0f, params::window_height / 2.0f);
}

/// <summary>
/// Unloads the MenuScene
/// </summary>
void MenuScene::unload(){}

void BasicLevelScene::m_load_level(const std::string &level, int enemyCount)
{
    LevelSystem::load_level(level, params::tile_size);
    this->set_enemy_count(enemyCount);
    m_portal_spawned = false;
    m_portal.reset();

    // Initialise death screen
    m_player_dead = false;

    // Use the same font as menu (vcr_mono.ttf which exists)
    if (!m_death_font.loadFromFile(EngineUtils::GetRelativePath("resources/fonts/vcr_mono.ttf")))
    {
        std::cout << "Error: Could not load death screen font" << std::endl;
        // Don't crash - just won't show text
    }
    else
    {
        m_death_text.setFont(m_death_font);
        m_death_text.setCharacterSize(60);
        m_death_text.setFillColor(sf::Color::Red);
        m_death_text.setString("YOU DIED\n\nPress 0 to return to menu");

        // Center the text
        sf::FloatRect textBounds = m_death_text.getLocalBounds();
        m_death_text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        m_death_text.setPosition(params::window_width / 2.0f, params::window_height / 2.0f);
    }

    // Initialise reload UI
    if (!m_reload_font.loadFromFile(EngineUtils::GetRelativePath("resources/fonts/vcr_mono.ttf")))
    {
        std::cout << "ERROR: Could not load reload UI font!" << std::endl;
    }
    else
    {
        m_reload_text.setFont(m_reload_font);
        m_reload_text.setCharacterSize(40);
        m_reload_text.setFillColor(sf::Color::Red);
        m_reload_text.setString("RELOADING...");
    }

    m_player = make_entity();
    m_player->set_position(LevelSystem::get_start_pos());

    // Create player with sprite
    std::shared_ptr<SpriteComponent> playerSprite = m_player->add_component<SpriteComponent>();

    // Try to load player sprite, fallback to colored rectangle if it fails
    if (playerSprite->load_texture(EngineUtils::GetRelativePath("resources/sprites/player_sprite.png")))
    {
        playerSprite->set_size(sf::Vector2f(params::player_size[0], params::player_size[1]));
        playerSprite->set_origin_center();
    }
    else
    {
        // Fallback to shape if texture loading fails
        std::cout << "Failed to load player sprite, using fallback shape" << std::endl;
        std::shared_ptr<ShapeComponent> shape = m_player->add_component<ShapeComponent>();
        shape->set_shape<sf::RectangleShape>(sf::Vector2f(params::player_size[0],params::player_size[1]));
        shape->get_shape().setFillColor(sf::Color::Yellow);
        shape->get_shape().setOrigin(sf::Vector2f(params::player_size[0]/2.f,params::player_size[1]/2.f));
    }

    // Add player physics component
    std::shared_ptr<PlayerControlComponent> component =
    m_player->add_component<PlayerControlComponent>(sf::Vector2f(params::player_size[0],params::player_size[1]));

    component->create_box_shape({params::player_size[0], params::player_size[1]},
        params::player_weight, params::player_friction, params::player_restitution);

    // Add player health component
    m_player->add_component<HealthComponent>(100.0f);

    // Add player shooting component
    m_player->add_component<PlayerShootingComponent>(this);

    // Create walls
    std::vector<std::vector<sf::Vector2i>> wall_groups = LevelSystem::get_groups(LevelSystem::WALL);
    for (const std::vector<sf::Vector2i> &walls : wall_groups) {
        m_walls.push_back(make_entity());
        m_walls.back()->add_component<PlatformComponent>(walls);
    }

    // Create enemies
    std::vector<sf::Vector2i> emptyTiles = LevelSystem::find_tiles(LevelSystem::Tile::EMPTY);

    std::vector<sf::Vector2i> enemyPositions = place_enemies_randomly(emptyTiles, this->enemyCount);

    for (const sf::Vector2i enemy_pos : enemyPositions)
    {
        m_enemies.push_back(make_entity());
        m_enemies.back()->set_position(sf::Vector2f(enemy_pos.x * 20.0f, enemy_pos.y * 20.0f));

        // Create enemy with sprite
        std::shared_ptr<SpriteComponent> enemySprite = m_enemies.back()->add_component<SpriteComponent>();

        // Try to load enemy sprite, fallback to colored rectangle if it fails
        if (enemySprite->load_texture(EngineUtils::GetRelativePath("resources/sprites/enemy_sprite.png")))
        {
            enemySprite->set_size(sf::Vector2f(params::enemy_size[0], params::enemy_size[1]));
            enemySprite->set_origin_center();
        }
        else
        {
            // Fallback to shape if texture loading fails
            std::cout << "Failed to load enemy sprite, using fallback shape" << std::endl;
            std::shared_ptr<ShapeComponent> shape = m_enemies.back()->add_component<ShapeComponent>();
            shape->set_shape<sf::RectangleShape>(sf::Vector2f(params::enemy_size[0], params::enemy_size[1]));
            shape->get_shape().setFillColor(sf::Color::Blue);
            shape->get_shape().setOrigin(sf::Vector2f(params::enemy_size[0] / 2.f, params::enemy_size[1] / 2.f));
        }

        std::shared_ptr<EnemyControlComponent> component = m_enemies.back()->add_component<EnemyControlComponent>(sf::Vector2f(params::enemy_size[0], params::enemy_size[1]));
        component->create_box_shape({ params::enemy_size[0]-3, params::enemy_size[1]-3 },
            params::enemy_weight, params::enemy_friction, params::enemy_restitution);
        component->set_target(m_player);

        // Add enemy health component
        m_enemies.back()->add_component<HealthComponent>(30.0f);

        // Add enemy shooting component with balanced parameters
        // Parameters: (entity, scene, target, clip_size, reload_time, fire_rate, bullet_speed, bullet_damage)
        auto enemyShooter = m_enemies.back()->add_component<EnemyShootingComponent>(
            this,
            m_player.get(),
            10,     // clip_size - 10 shots before reload
            2.0f,   // reload_time - 2 seconds to reload
            2.0f,   // fire_rate - 2.0 shots/second (shoot every 0.5 seconds!)
            250.0f, // bullet_speed - medium speed bullets
            2.0f    // bullet_damage - 2 damage per hit (50 hits to kill player)
        );
        enemyShooter->set_shooting_range(400.0f);  // 400 pixel range
        enemyShooter->set_shoot_chance(1.0f);  // 100% chance - ALWAYS shoot when ready!
        enemyShooter->set_random_delay_range(0.0f, 0.5f);  // Very short delays: 0-0.5 seconds!
    }
}

int BasicLevelScene::count_alive_enemies() const
{
    int count = 0;
    for (const auto& enemy : m_enemies)
    {
        if (enemy && enemy->is_alive() && !enemy->to_be_deleted())
        {
            count++;
        }
    }
    return count;
}

void BasicLevelScene::spawn_portal()
{
    if (m_portal_spawned || m_portal)
    {
        return; // Portal already spawned
    }

    // Use last enemy position if available, otherwise fallback to END tile
    sf::Vector2f portalPos = m_last_enemy_position;

    // Fallback to END tile if no enemy position tracked
    if (portalPos.x == 0.0f && portalPos.y == 0.0f)
    {
        std::vector<sf::Vector2i> endTiles = LevelSystem::find_tiles(LevelSystem::Tile::END);
        if (!endTiles.empty())
        {
            portalPos = LevelSystem::get_tile_pos(endTiles[0]);
            portalPos += sf::Vector2f(params::tile_size / 2.0f, params::tile_size / 2.0f);
        }
    }

    // Create portal entity
    m_portal = make_entity();
    m_portal->set_position(portalPos);

    // Add visual component - BRIGHT GREEN CIRCLE (very visible!)
    std::shared_ptr<ShapeComponent> shape = m_portal->add_component<ShapeComponent>();
    shape->set_shape<sf::CircleShape>(params::tile_size * 0.8f); // Bigger circle
    shape->get_shape().setFillColor(sf::Color(0, 255, 0, 220)); // Bright green, almost opaque
    shape->get_shape().setOutlineColor(sf::Color::Yellow); // Yellow outline
    shape->get_shape().setOutlineThickness(3.0f);
    shape->get_shape().setOrigin(params::tile_size * 0.8f, params::tile_size * 0.8f);

    m_portal_spawned = true;

    std::cout << "Portal spawned at position: " << portalPos.x << ", " << portalPos.y << std::endl;
}

void BasicLevelScene::update(const float& dt) {
    // Check if player is dead
    if (m_player && !m_player->is_alive())
    {
        m_player_dead = true;
    }

    // If player is dead, check for menu return key and stop updating game
    if (m_player_dead)
    {
        // Static variable to prevent key hold from menu
        static bool death_key_was_pressed = false;
        bool death_key_is_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num0);

        // Only trigger on new key press (not hold from previous screen)
        if (death_key_is_pressed && !death_key_was_pressed)
        {
            // Return to menu - menu will create a fresh game when 0 is pressed again
            GameSystem::setActiveScene(Scenes::menuScene);
        }

        death_key_was_pressed = death_key_is_pressed;
        return; // Don't update game when dead
    }

    Scene::update(dt);
    m_entities.update(dt);

    // Check bullet collisions
    std::vector<std::shared_ptr<Entity>> all_targets;
    all_targets.push_back(m_player);
    for (const auto& enemy : m_enemies)
    {
        if (enemy && enemy->is_alive())
        {
            all_targets.push_back(enemy);
        }
    }

    // Check each bullet for collisions
    for (auto& entity : m_entities.list)
    {
        if (!entity || !entity->is_alive()) continue;

        auto bullet_components = entity->get_compatible_components<BulletComponent>();
        if (!bullet_components.empty() && bullet_components[0])
        {
            // Set callback to track where enemies are killed
            bullet_components[0]->set_on_kill_callback([this](sf::Vector2f kill_position) {
                // Store the position where an enemy was killed
                m_last_enemy_position = kill_position;
                std::cout << "Enemy killed at: " << kill_position.x << ", " << kill_position.y << std::endl;
            });

            bullet_components[0]->check_collision(all_targets);
        }
    }

    // Check if all enemies are dead and spawn portal if needed
    if (!m_portal_spawned && count_alive_enemies() == 0)
    {
        std::cout << "All enemies defeated! Spawning portal..." << std::endl;
        spawn_portal();
    }

    // have the camera slightly follow the player's mouse position
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(Renderer::getWindow());
    sf::Vector2f mouse_world_pos(
        static_cast<float>(mouse_pos.x - (mouse_pos.x / 2)),
        static_cast<float>(mouse_pos.y - (mouse_pos.y / 2))
    );

    GameSystem::moveCamera({
        m_player->get_position().x + (mouse_world_pos.x / 10.0f),
        m_player->get_position().y + (mouse_world_pos.y / 12.5f) // y-axis will have less impact on the camera
    });

    // Check if player reached portal (or END tile as fallback)
    if (m_portal_spawned && m_portal)
    {
        // Check distance to portal
        sf::Vector2f toPortal = m_portal->get_position() - m_player->get_position();
        float distance = std::sqrt(toPortal.x * toPortal.x + toPortal.y * toPortal.y);

        // Debug: Print distance when player is getting close
        if (distance < params::tile_size * 2.0f)
        {
            std::cout << "Distance to portal: " << distance << " (need < " << params::tile_size << ")" << std::endl;
        }

        if (distance < params::tile_size * 1.5f)  // Made activation area bigger
        {
            std::cout << "Player reached portal! Loading next level..." << std::endl;
            int enemyCount = this->enemyCount;
            if (this->enemyCount + 1 <= 50)
            {
                enemyCount = this->enemyCount + 1;
            }
            unload();
            m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), enemyCount);
        }
    }
    else if (LevelSystem::get_tile_at(m_player->get_position()) == LevelSystem::END)
    {
        // Fallback: if player touches END tile before portal spawns (shouldn't happen normally)
        std::cout << "Player reached END tile (fallback)! Loading next level..." << std::endl;
        int enemyCount = this->enemyCount;
        if (this->enemyCount + 1 <= 50)
        {
            enemyCount = this->enemyCount + 1;
        }
        unload();
        m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), enemyCount);
    }
}

void BasicLevelScene::render() {
    // Render death screen if player is dead
    if (m_player_dead)
    {
        sf::View currentView = Renderer::getWindow().getView();
        sf::Vector2f viewCenter = currentView.getCenter();
        sf::Vector2f viewSize = currentView.getSize();

        sf::RectangleShape overlay(viewSize);
        overlay.setPosition(viewCenter - viewSize / 2.0f);
        overlay.setFillColor(sf::Color::Black); // Completely black, no transparency!
        Renderer::getWindow().draw(overlay);

        if (m_death_font.getInfo().family != "")
        {
            m_death_text.setPosition(viewCenter);
            Renderer::getWindow().draw(m_death_text);
        }
        return; // Don't render game when dead
    }

    // Normal game rendering
    LevelSystem::render(Renderer::getWindow());
    Scene::render();
    m_entities.render();

    // Render reload UI if player is reloading
    if (m_player)
    {
        auto shooting_components = m_player->get_compatible_components<PlayerShootingComponent>();
        if (!shooting_components.empty() && shooting_components[0])
        {
            if (shooting_components[0]->is_reloading())
            {
                // Get current view for positioning
                sf::View currentView = Renderer::getWindow().getView();
                sf::Vector2f viewCenter = currentView.getCenter();
                sf::Vector2f viewSize = currentView.getSize();

                // Position reload text at top center of screen
                m_reload_text.setPosition(viewCenter.x, viewCenter.y - viewSize.y / 2.0f + 50.0f);

                // Center horizontally
                sf::FloatRect bounds = m_reload_text.getLocalBounds();
                m_reload_text.setOrigin(bounds.width / 2.0f, 0.0f);

                Renderer::getWindow().draw(m_reload_text);
            }
        }
    }
}

void BasicLevelScene::load() {
    m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), this->enemyCount);
}

void BasicLevelScene::unload() {
    Scene::unload();
    m_player.reset();
    m_walls.clear();
    m_enemies.clear();
    m_portal.reset();
    m_portal_spawned = false;
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