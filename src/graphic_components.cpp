#include "graphic_components.hpp"
#include "renderer.hpp"
#include <iostream>
#include <cmath>

/// <summary>
/// Updates the shape component.
/// </summary>
/// <param name="dt">Delta Time - Linked to Frame Rate.</param>
void ShapeComponent::update(const float &dt) {
  _shape->setPosition(m_parent->get_position());
  if (this->m_parent->is_visible())
  {
	  // Get current scale to preserve size
	  sf::Vector2f currentScale = _shape->getScale();
	  float scaleX = std::abs(currentScale.x);
	  float scaleY = currentScale.y;

	  // Mirror shape based on facing direction
	  if (!m_parent->is_facing_right())
	  {
		  _shape->setScale(-scaleX, scaleY);
	  }
	  else
	  {
		  _shape->setScale(scaleX, scaleY);
	  }
  }
}

/// <summary>
/// Renders the shape component.
/// </summary>
void ShapeComponent::render() { 
	if (this->m_parent->is_visible())
	{
		Renderer::queue(_shape.get());
	}
}

/// <summary>
/// Gets the shape of the ShapeComponent.
/// </summary>
/// <returns>The shape.</returns>
sf::Shape& ShapeComponent::get_shape() const { return *_shape; }

/// <summary>
/// Initialises the ShapeComponent.
/// </summary>
/// <param name="p">The entity being given a shape.</param>
ShapeComponent::ShapeComponent(Entity* p) : Component(p), _shape(std::make_shared<sf::CircleShape>()) {}

/// <summary>
/// Initialises the SpriteComponent.
/// </summary>
/// <param name="p">The entity being given a sprite.</param>
SpriteComponent::SpriteComponent(Entity *p) : Component(p), m_sprite(std::make_shared<sf::Sprite>()) {}

/// <summary>
/// Gets the Sprite.
/// </summary>
/// <returns>The sprite.</returns>
sf::Sprite& SpriteComponent::get_sprite() const 
{
	return *m_sprite;
}

/// <summary>
/// Updates the sprite.
/// </summary>
/// <param name="dt">Delta Time - Linked to Frame Rate.</param>
void SpriteComponent::update(const float &dt)
{
	m_sprite->setPosition(m_parent->get_position());
	
	if (this->m_parent->is_visible())
	{
		// Get current scale to preserve size
		sf::Vector2f currentScale = m_sprite->getScale();
		float scaleX = std::abs(currentScale.x);
		float scaleY = currentScale.y;

		// Mirror sprite based on facing direction
		// If facing left, flip horizontally using negative scale
		if (!m_parent->is_facing_right())
		{
			m_sprite->setScale(-scaleX, scaleY);
		}
		else
		{
			m_sprite->setScale(scaleX, scaleY);
		}
	}
}

/// <summary>
/// Renders the SpriteComponent.
/// </summary>
void SpriteComponent::render()
{
	if (this->m_parent->is_visible())
	{
		Renderer::queue(m_sprite.get());
	}
}

/// <summary>
/// Loads the sprite texture.
/// </summary>
/// <param name="filepath">File path of the sprite texture.</param>
/// <returns>Whether or not loading was successful.</returns>
bool SpriteComponent::load_texture(const std::string& filepath)
{
	m_texture = std::make_shared<sf::Texture>();
	
	if (!m_texture->loadFromFile(filepath))
	{
		return false;
	}
	
	m_sprite->setTexture(*m_texture);
	return true;
}

/// <summary>
/// Sets the size of the sprite.
/// </summary>
/// <param name="size">Vector2f for size.</param>
void SpriteComponent::set_size(const sf::Vector2f& size)
{
	if (m_texture)
	{
		sf::Vector2u textureSize = m_texture->getSize();
		m_sprite->setScale(
			size.x / textureSize.x,
			size.y / textureSize.y
		);
	}
}

/// <summary>
/// Sets the origin of the Sprite.
/// </summary>
void SpriteComponent::set_origin_center()
{
	if (m_texture)
	{
		sf::Vector2u textureSize = m_texture->getSize();
		m_sprite->setOrigin(
			textureSize.x / 2.0f,
			textureSize.y / 2.0f
		);
	}
}