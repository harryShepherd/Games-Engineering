#pragma once

#include <SFML/Graphics.hpp>

namespace Renderer
{
    void init(sf::RenderWindow &win);
    sf::RenderWindow& getWindow();
    
    void shutdown();
    void update(const float &dt);
    void queue(const sf::Drawable *sprite);
    void render();
};