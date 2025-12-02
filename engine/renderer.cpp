#include "renderer.hpp"
#include <queue>

static std::queue<const sf::Drawable *> sprites;
static sf::RenderWindow *window;
static sf::View *view;

// Initialise the render window
void Renderer::init(sf::RenderWindow &win, sf::View &v)
{
    window = &win;
    view = &v;
}

// Get the render window
sf::RenderWindow &Renderer::getWindow()
{
    return *window;
}

sf::View &Renderer::getView()
{
    return *view;
}

// Shutdown the renderer
void Renderer::shutdown()
{
    while (!sprites.empty())
    {
        sprites.pop();
    }
}

void Renderer::render()
{
    if(window == nullptr)
    {
        throw std::logic_error("No render window is set.");
    }

    while(!sprites.empty())
    {
        window->draw(*sprites.front());
        sprites.pop();
    }
}

void Renderer::queue(const sf::Drawable *sprite)
{
    sprites.push(sprite);
}

void Renderer::update(const float &dt)
{
    
}