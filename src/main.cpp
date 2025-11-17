#include <SFML/Graphics.hpp>
#include "game_system.hpp"
#include "game_parameters.hpp"

int main() {
	GameSystem::start(params::window_width, params::window_height, "Cube Zone", params::time_step);
	return 0;
}