#include <iostream>
#include <random>
#include "scenes.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_parameters.hpp"
#include "graphic_components.hpp"
#include "control_components.hpp"
#include "ai_components.hpp"
#include "engine_utils.hpp"
#include "physics.hpp"
#include "physics_components.hpp"

std::shared_ptr<Scene> Scenes::menuScene;
std::shared_ptr<Scene> Scenes::steeringScene;
std::shared_ptr<Scene> Scenes::physicsScene;
std::shared_ptr<Scene> Scenes::basicLevelScene;

/// <summary>
/// Updates the MenuScene
/// </summary>
/// <param name="dt">Delta Time - Sets frame rate</param>
void MenuScene::update(const float& dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) 
    {
        Scenes::steeringScene = std::make_shared<SteeringScene>();
        GameSystem::setActiveScene(Scenes::steeringScene);
    } 
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
    {
        Scenes::physicsScene = std::make_shared<PhysicsScene>();
        GameSystem::setActiveScene(Scenes::physicsScene);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
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
    _text.setString("Cube Zone\n\n\nPress 0 for Physics\nPress 1 for Steering\nPress 2 for the basic level scene");
}

/// <summary>
/// Unloads the MenuScene
/// </summary>
void MenuScene::unload(){}

/// <summary>
/// Loads the steering Scene.
///     Not permanent
/// </summary>
void SteeringScene::load() {
    std::shared_ptr<Entity> player = make_entity();
    player->set_position(sf::Vector2f(params::window_height / 2, params::window_height / 2));
    std::shared_ptr<ShapeComponent> shape = player->add_component<ShapeComponent>();
    shape->set_shape<sf::RectangleShape>(sf::Vector2f(10.0f, 10.0f));
    shape->get_shape().setFillColor(sf::Color::Red);
    player->add_component<KeyboardMovementComponent>();

    
    std::random_device dev;
    std::default_random_engine engine(dev());
    std::uniform_real_distribution<float> x_dist(0.0f, params::window_width);
    std::uniform_real_distribution<float> y_dist(0.0f, params::window_height);

    for (size_t e = 0; e < 100; e++)
    {
        std::shared_ptr<Entity> enemy = make_entity();
        enemy->set_position(sf::Vector2f(x_dist(engine), y_dist(engine)));
        std::shared_ptr<ShapeComponent> shape = enemy->add_component<ShapeComponent>();
        shape->set_shape<sf::RectangleShape>(sf::Vector2f(10.0f, 10.0f));
        shape->get_shape().setFillColor(sf::Color::Blue);
        enemy->add_component<SteeringComponent>(player.get(), 50.0f);
    }
}

/// <summary>
/// Unloads the scene.
/// </summary>
void SteeringScene::unload() {
    Scene::unload();
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt"></param>
void SteeringScene::update(const float& dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) 
    {
        Scenes::physicsScene = std::make_shared<PhysicsScene>();
        GameSystem::setActiveScene(Scenes::physicsScene);
    } 

    Scene::update(dt);
}

/// <summary>
/// Renders the scene.
/// </summary>
void SteeringScene::render() {
    Scene::render();
}

/// <summary>
/// Loads the scene.
/// </summary>
void PhysicsScene::load(){
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2({0.0f, params::g});  
    world_id = b2CreateWorld(&world_def);

    // Create Boxes
  for (int i = 1; i < 11; ++i) {
    // Create SFML shapes for each box
    std::shared_ptr<sf::RectangleShape> s = std::make_shared<sf::RectangleShape>();
    s->setPosition(sf::Vector2f(i * (params::window_width / 12.f), params::window_height * .7f));
    s->setSize(sf::Vector2f(50.0f, 50.0f));
    s->setOrigin(sf::Vector2f(25.0f, 25.0f));
    s->setFillColor(sf::Color::White);
    sprites.push_back(s);
    
    // Create a dynamic physics body for the box
    b2BodyId b = Physics::create_physics_box(world_id, true, s);
    // Give the box a spin
    b2Body_ApplyAngularImpulse(b,5.0f, true);
    bodies.push_back(b);
  }

  sf::Vector2f walls[] = {
      // Top
      sf::Vector2f(params::window_width * .5f, 5.f), sf::Vector2f(params::window_width, 10.f),
      // Bottom
      sf::Vector2f(params::window_width * .5f, params::window_height - 5.f), sf::Vector2f(params::window_width, 10.f),
      // left
      sf::Vector2f(5.f, params::window_height * .5f), sf::Vector2f(10.f, params::window_height),
      // right
      sf::Vector2f(params::window_width - 5.f, params::window_height * .5f), sf::Vector2f(10.f, params::window_height)
  };

  // Build Walls
  for (int i = 0; i < 7; i += 2) {
      // Create SFML shapes for each wall
      std::shared_ptr<sf::RectangleShape> s = std::make_shared<sf::RectangleShape>();
      s->setPosition(walls[i]);
      s->setSize(walls[i+1]);
      s->setOrigin(walls[i+1]/2.f);
      s->setFillColor(sf::Color::White);
      sprites.push_back(s);
      // Create a static physics body for the wall
      b2BodyId b = Physics::create_physics_box(world_id,false,s);
      bodies.push_back(b);
  }
}

/// <summary>
/// Updates the scene.
/// </summary>
/// <param name="dt">Takes the delta time used for frequency of frames.</param>
void PhysicsScene::update(const float &dt){

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) 
    {
        Scenes::steeringScene = std::make_shared<SteeringScene>();
        GameSystem::setActiveScene(Scenes::steeringScene);
    } 

    // Step Physics world by time_step
    b2World_Step(world_id,params::time_step,params::sub_step_count);

    for (int i = 0; i < bodies.size(); ++i) {
        // Sync Sprites to physics position
        sprites[i]->setPosition(Physics::invert_height(Physics::bv2_to_sv2(b2Body_GetPosition(bodies[i])), params::window_height));
        // Sync Sprites to physics Rotation
        sprites[i]->setRotation((180 / 3.1415f) * asin(b2Body_GetRotation(bodies[i]).s));
    }
}
/// <summary>
/// Renders the scene.
/// </summary>
void PhysicsScene::render(){
  for(std::shared_ptr<sf::RectangleShape> sprite: sprites)
    Renderer::queue(sprite.get());

}
/// <summary>
/// Unloads the scene.
/// </summary>
void PhysicsScene::unload(){
  for(std::shared_ptr<sf::RectangleShape> &shape: sprites)
    shape.reset();
  sprites.clear();

  for(b2BodyId body: bodies)
    b2DestroyBody(body);
  bodies.clear();
  b2DestroyWorld(world_id);
}

void BasicLevelScene::m_load_level(const std::string &level)
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

    //component->create_capsule_shape(
    //    sf::Vector2f(params::player_size[0],params::player_size[1]),
    //    params::player_weight, params::player_friction, params::player_restitution
    //);

    // Create walls
    std::vector<std::vector<sf::Vector2i>> wall_groups = LevelSystem::get_groups(LevelSystem::WALL);
    for (const std::vector<sf::Vector2i> &walls : wall_groups) {
        m_walls.push_back(make_entity());
        m_walls.back()->add_component<PlatformComponent>(walls);
    }

    // Create enemies
    std::vector<sf::Vector2i> enemies = LevelSystem::find_tiles(LevelSystem::Tile::ENEMY);

    float enemyHeight = 10.0f;
    float enemyWidth = 10.0f;

    for (const sf::Vector2i enemy_pos : enemies)
    {
        std::cout << "creating enemy at pos: ()" << enemy_pos.x << ", " << enemy_pos.y << ")" <<std::endl;

        m_enemies.push_back(make_entity());
        m_enemies.back()->set_position(sf::Vector2f(enemy_pos.x * 20.0f, enemy_pos.y * 25.0f));

        std::shared_ptr<ShapeComponent> shape = m_enemies.back()->add_component<ShapeComponent>();
        shape->set_shape<sf::RectangleShape>(sf::Vector2f(20.0f, 25.0f));
        shape->get_shape().setFillColor(sf::Color::Blue);

        m_enemies.back()->add_component<SteeringComponent>(m_player.get(), 50.0f);
        m_enemies.back()->add_component<PhysicsComponent>(true);
    }
}

void BasicLevelScene::update(const float& dt) {
    Scene::update(dt);
    m_entities.update(dt);

    if(LevelSystem::get_tile_at(m_player->get_position()) == LevelSystem::END){
        unload();
        m_load_level(EngineUtils::GetRelativePath(params::level_2));
    }

}

void BasicLevelScene::render() {
    LevelSystem::render(Renderer::getWindow());
    Scene::render();
    m_entities.render();
}

void BasicLevelScene::load() {
    m_load_level(EngineUtils::GetRelativePath(params::level_1));
}

void BasicLevelScene::unload() {
    Scene::unload();
    m_player.reset();
    m_walls.clear();
}