#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "physics.hpp"
#include "scenes.hpp"

int main() 
{
	Physics::initialise();

	Scenes::testScene = std::make_shared<TestScene>();
	Scenes::testScene->load();
	
	GameSystem::setActiveScene(Scenes::testScene);
	GameSystem::start(params::window_width, params::window_height, "Test", Physics::time_step, true);

	Physics::shutdown();
	return 0;
}