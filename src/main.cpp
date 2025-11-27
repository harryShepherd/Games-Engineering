#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "physics.hpp"
#include "scenes.hpp"
#include <Windows.h>

int main() 
{
	Physics::initialise();

	Scenes::menuScene = std::make_shared<MenuScene>();
	Scenes::menuScene->load();
	GameSystem::setActiveScene(Scenes::menuScene);

	SetProcessDPIAware();
	HWND window = GetDesktopWindow();
	RECT r;
	int width = params::window_height;
	int height = params::window_width;
	if (GetClientRect(window, &r))
	{
		width = r.right;
		height = r.bottom;
	}

	GameSystem::start(width, height, "Menu", Physics::time_step, true);

	Physics::shutdown();
	return 0;
}