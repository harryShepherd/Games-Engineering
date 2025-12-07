#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "physics.hpp"
#include "scenes.hpp"

int main() 
{
	Physics::initialise();

	Scenes::menuScene = std::make_shared<MenuScene>();
	Scenes::menuScene->load();

	Scenes::deathScene = std::make_shared<DeathScene>();
	Scenes::deathScene->load();

	GameSystem::setActiveScene(Scenes::menuScene);
	GameSystem::start(params::window_width, params::window_height, "Cube Zone", Physics::time_step, true);

	Physics::shutdown();
	return 0;
}