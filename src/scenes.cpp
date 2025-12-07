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
std::shared_ptr<Scene> Scenes::deathScene;

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
        unload();
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
void MenuScene::unload(){
    Scene::unload();
}

/// <summary>
/// Updates the DeathScene
/// </summary>
void DeathScene::update(const float& dt) {
    // Static variable to prevent key hold
    static bool key_was_pressed = false;
    bool key_is_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num0);

    // Only trigger on NEW key press
    if (key_is_pressed && !key_was_pressed)
    {
        unload();
        // Return to menu
        GameSystem::setActiveScene(Scenes::menuScene);
    }

    key_was_pressed = key_is_pressed;
    Scene::update(dt);
}

/// <summary>
/// Renders the DeathScene - full black screen with red text
/// </summary>
void DeathScene::render() {
    // Get current view
    sf::View currentView = Renderer::getWindow().getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    sf::RectangleShape overlay(viewSize);
    overlay.setPosition(viewCenter - viewSize / 2.0f);
    overlay.setFillColor(sf::Color::Black);
    Renderer::getWindow().draw(overlay);

    // Draw death text centered
    _death_text.setPosition(viewCenter);
    Renderer::getWindow().draw(_death_text);

    Scene::render();
}

/// <summary>
/// Loads the death screen font and text
/// </summary>
void DeathScene::load() {
    if (!_death_font.loadFromFile(EngineUtils::GetRelativePath("resources/fonts/vcr_mono.ttf")))
    {
        std::cout << "ERROR: Could not load death screen font!" << std::endl;
    }
    else
    {
        _death_text.setFont(_death_font);
        _death_text.setCharacterSize(60);
        _death_text.setFillColor(sf::Color::Red);
        _death_text.setString("YOU DIED\n\nPress 0 to return to menu");

        // Center the text
        sf::FloatRect textBounds = _death_text.getLocalBounds();
        _death_text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        _death_text.setPosition(params::window_width / 2.0f, params::window_height / 2.0f);
    }
}

/// <summary>
/// Unloads the DeathScene
/// </summary>
void DeathScene::unload(){
    Scene::unload();
}

// Basic Level Scene
void BasicLevelScene::m_load_level(const std::string &level, int enemyCount)
{
    LevelSystem::load_level(level, params::tile_size);
    this->set_enemy_count(enemyCount);
    m_portal_spawned = false;
    m_portal.reset();
    m_alive_enemy_count = enemyCount;  // Initialise alive enemy counter
    m_active_bullets.clear();  // Clear active bullets list

    initialise_bullet_pool(100);  // Create pool of 100 bullets

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

    // Retrieve empty tiles
    std::vector<sf::Vector2i> emptyTiles = LevelSystem::find_tiles(LevelSystem::Tile::EMPTY);
    // Picks input amount of positions to create an enemy at.
    std::vector<sf::Vector2i> enemyPositions = place_enemies_randomly(emptyTiles, enemyCount);
    add_enemies(this->enemyCount, enemyPositions);

    // Build initial collision targets list
    rebuild_collision_targets();
}

void BasicLevelScene::rebuild_collision_targets()
{
    m_collision_targets.clear();
    m_collision_targets.push_back(m_player);

    for (const auto& enemy : m_enemies)
    {
        if (enemy && enemy->is_alive() && !enemy->to_be_deleted())
        {
            m_collision_targets.push_back(enemy);
        }
    }
}

void BasicLevelScene::on_enemy_death(sf::Vector2f death_position)
{
    m_alive_enemy_count--;
    m_last_enemy_position = death_position;

    std::cout << "Enemy killed at: " << death_position.x << ", " << death_position.y
              << " (" << m_alive_enemy_count << " remaining)" << std::endl;

    // Rebuild targets list since an enemy died
    rebuild_collision_targets();

    // Check if all enemies are dead
    if (m_alive_enemy_count == 0 && !m_portal_spawned)
    {
        std::cout << "All enemies defeated. Spawning portal..." << std::endl;
        spawn_portal();
    }
}

int BasicLevelScene::count_alive_enemies() const
{
    return m_alive_enemy_count;  // Now  return cached value
}

int BasicLevelScene::count_bullets() const
{
    return m_active_bullets.size();  // Return size of active bullets list
}

void BasicLevelScene::spawn_portal()
{
    if (m_portal_spawned || m_portal)
    {
        return; // Portal already spawned
    }

    // Use last enemy position if available, otherwise fallback to START tile
    sf::Vector2f portalPos = m_last_enemy_position;

    // Fallback to START tile if no enemy position tracked
    if (portalPos.x == 0.0f && portalPos.y == 0.0f)
    {
        std::vector<sf::Vector2i> endTiles = LevelSystem::find_tiles(LevelSystem::Tile::START);
        if (!endTiles.empty())
        {
            portalPos = LevelSystem::get_tile_pos(endTiles[0]);
            portalPos += sf::Vector2f(params::tile_size / 2.0f, params::tile_size / 2.0f);
        }
    }

    // Create portal entity
    m_portal = make_entity();
    m_portal->set_position(portalPos);

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
    // Check if player is dead - switch to death scene
    if (m_player && !m_player->is_alive())
    {
        // Switch to the death scene (should already exist from main.cpp)
        if (Scenes::deathScene)
        {
            GameSystem::setActiveScene(Scenes::deathScene);
        }
        return; // Stop updating this scene
    }

    Scene::update(dt);
    m_entities.update(dt);

    // Return dead bullets to pool and remove from active list
    m_active_bullets.erase(
        std::remove_if(m_active_bullets.begin(), m_active_bullets.end(),
            [this](std::shared_ptr<Entity>& bullet) {
                if (!bullet->is_alive()) {
                    return_bullet_to_pool(bullet);
                    return true;  // Remove from active list
                }
                return false;  // Keep in active list
            }),
        m_active_bullets.end()
    );

    // Check bullet collisions - only iterate through active bullets
    for (auto& bullet : m_active_bullets)
    {
        if (!bullet || !bullet->is_alive()) continue;

        auto bullet_components = bullet->get_compatible_components<BulletComponent>();
        if (!bullet_components.empty() && bullet_components[0])
        {
            // Set callback to trigger on_enemy_death when a kill happens
            bullet_components[0]->set_on_kill_callback([this](sf::Vector2f kill_position) {
                on_enemy_death(kill_position);
            });

            bullet_components[0]->check_collision(m_collision_targets);
        }
    }

    // This section is mostly to account for a scenario where the player doesn't kill any enemies.
        // Due to enemies falling out of the map through failed jumps.
    int deadCount=0;
    for each(std::shared_ptr<Entity> enemy in m_enemies)
    {
        if (!enemy->is_alive())
        {
            deadCount++;
        }
    }
    if (deadCount == this->enemyCount)
    {
        spawn_portal();
    }

    // Camera follows player position
    GameSystem::moveCamera(m_player->get_position());

    // Check if player reached portal
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
}

void BasicLevelScene::render() {
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
    this->currentLevel = 0;
    m_load_level(EngineUtils::GetRelativePath(pick_level_randomly()), this->enemyCount);
}

void BasicLevelScene::unload() {
    Scene::unload();
    m_player.reset();
    m_walls.clear();
    m_enemies.clear();
    m_portal.reset();
    m_portal_spawned = false;
    m_active_bullets.clear();
    m_collision_targets.clear();
    m_alive_enemy_count = 0;
}

std::vector<sf::Vector2i> BasicLevelScene::place_enemies_randomly(std::vector<sf::Vector2i> tiles, int enemyCount) {
    std::vector<sf::Vector2i> enemyPositions;
    std::random_device random_device;
    std::default_random_engine engine(random_device());
    std::uniform_int_distribution<> distribution(0, tiles.size() - 1);
    sf::Vector2i chosenPosition;
    for (size_t i = 0; i < enemyCount; i++)
    {
        chosenPosition = tiles[distribution(engine)];
        enemyPositions.push_back(sf::Vector2i(chosenPosition.x * params::tile_size, chosenPosition.y * params::tile_size));
    }

    return enemyPositions;
}

std::string BasicLevelScene::pick_level_randomly() {
    std::random_device random_device;
    std::default_random_engine engine(random_device());
    std::uniform_int_distribution<> distribution(1, params::getLevels().size());
    int levelInt = 0;
    do
    {
        levelInt = distribution(engine);

    } while (this->currentLevel == levelInt);
    this->currentLevel = levelInt;
    std::string level = params::getLevels().at(this->currentLevel);
    return level;
}

void BasicLevelScene::add_enemies(int enemyCount, std::vector<sf::Vector2i> positions) {
    for (size_t i = 0; i < enemyCount; i++)
    {
        m_enemies.push_back(make_entity());
        m_enemies.back()->set_position(sf::Vector2f(positions.at(i).x, positions.at(i).y));

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
        component->create_box_shape({ params::enemy_size[0] - 3, params::enemy_size[1] - 3 },
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
        enemyShooter->set_random_delay_range(0.0f, 1.0f);  // Very short delays: 0-1 seconds!
    }
}

// Bullet pool implementation
void BasicLevelScene::initialise_bullet_pool(int pool_size) {
    // Clear any existing pool
    m_bullet_pool.clear();
    while (!m_available_bullets.empty()) {
        m_available_bullets.pop();
    }

    std::cout << "Initialising bullet pool with " << pool_size << " bullets..." << std::endl;

    // Create pool of inactive bullets off-screen
    for (int i = 0; i < pool_size; i++) {
        auto bullet = make_entity();
        bullet->set_position(sf::Vector2f(-10000.0f, -10000.0f)); // Way off-screen
        bullet->set_alive(false); // Mark as inactive

        // Add visual component (will be configured when bullet is used)
        auto shape = bullet->add_component<ShapeComponent>();
        shape->set_shape<sf::CircleShape>(3.0f); // Default size
        shape->get_shape().setFillColor(sf::Color::White); // Default color
        shape->get_shape().setOrigin(3.0f, 3.0f);

        // Store in pool
        m_bullet_pool.push_back(bullet);
        m_available_bullets.push(bullet);
    }

    std::cout << "Bullet pool initialised!" << std::endl;
}

std::shared_ptr<Entity> BasicLevelScene::get_bullet_from_pool() {
    if (m_available_bullets.empty()) {
        std::cout << "Warning: Bullet pool exhausted! Creating new bullet..." << std::endl;
        // Pool exhausted - create a new bullet (fallback)
        auto bullet = make_entity();
        auto shape = bullet->add_component<ShapeComponent>();
        shape->set_shape<sf::CircleShape>(3.0f);
        shape->get_shape().setFillColor(sf::Color::White);
        shape->get_shape().setOrigin(3.0f, 3.0f);
        m_bullet_pool.push_back(bullet); // Add to pool for tracking

        // Add to active bullets list
        m_active_bullets.push_back(bullet);
        return bullet;
    }

    // Get bullet from pool
    auto bullet = m_available_bullets.front();
    m_available_bullets.pop();

    // Activate bullet
    bullet->set_alive(true);
    bullet->set_visible(true);

    // Add to active bullets list
    m_active_bullets.push_back(bullet);

    return bullet;
}

void BasicLevelScene::return_bullet_to_pool(std::shared_ptr<Entity> bullet) {
    if (!bullet) return;

    // Deactivate bullet
    bullet->set_alive(false);
    bullet->set_position(sf::Vector2f(-10000.0f, -10000.0f));

    // Remove any bullet components (they'll be re-added when reused)
    // This is important to reset state
    bullet->remove_components_by_type<BulletComponent>();

    // Return to available pool
    m_available_bullets.push(bullet);
}