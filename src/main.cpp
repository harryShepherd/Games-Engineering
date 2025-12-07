#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "physics.hpp"
#include "scenes.hpp"

int main() 
{
	Physics::initialise();

	Scenes::menuScene = std::make_shared<MenuScene>();
	Scenes::menuScene->load();
	GameSystem::setActiveScene(Scenes::menuScene);
	GameSystem::start(params::window_width, params::window_height, "Menu", Physics::time_step, false);

	Physics::shutdown();
	return 0;
}