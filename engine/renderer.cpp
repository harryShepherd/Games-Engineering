#include "renderer.hpp"
#include <queue>

static std::queue<const sf::Drawable *> sprites;
static sf::RenderWindow *window;
static sf::View *view;

/// <summary>
/// Intialises the render window.
/// </summary>
/// <param name="win">The window</param>
/// <param name="v">The view</param>
void Renderer::init(sf::RenderWindow &win, sf::View &v)
{
    window = &win;
    view = &v;
}

/// <summary>
/// Gets the render window.
/// </summary>
/// <returns></returns>
sf::RenderWindow &Renderer::getWindow()
{
    return *window;
}

/// <summary>
/// Gets the view
/// </summary>
/// <returns>The view</returns>
sf::View &Renderer::getView()
{
    return *view;
}

/// <summary>
/// Shuts down the renderer.
/// Pops all sprites.
/// </summary>
void Renderer::shutdown()
{
    while (!sprites.empty())
    {
        sprites.pop();
    }
}

/// <summary>
/// Renders sprites.
/// </summary>
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

/// <summary>
/// Queues the input sprite.
/// </summary>
/// <param name="sprite">The sprite to queue.</param>
void Renderer::queue(const sf::Drawable *sprite)
{
    sprites.push(sprite);
}

/// <summary>
/// Updates items in the renderer.
/// </summary>
/// <param name="dt">Delta Time - Linked to frame rate.</param>
void Renderer::update(const float &dt)
{
    
}