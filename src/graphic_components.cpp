#include "graphic_components.hpp"
#include "renderer.hpp"
#include <iostream>
#include <cmath>

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

void ShapeComponent::render() { 
	if (this->m_parent->is_visible())
	{
		Renderer::queue(_shape.get());
	}
}

sf::Shape& ShapeComponent::get_shape() const { return *_shape; }

ShapeComponent::ShapeComponent(Entity* p) : Component(p), _shape(std::make_shared<sf::CircleShape>()) {}

SpriteComponent::SpriteComponent(Entity *p) : Component(p), m_sprite(std::make_shared<sf::Sprite>()) {}

sf::Sprite& SpriteComponent::get_sprite() const 
{
	return *m_sprite;
}

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

void SpriteComponent::render()
{
	if (this->m_parent->is_visible())
	{
		Renderer::queue(m_sprite.get());
	}
}

bool SpriteComponent::load_texture(const std::string& filepath)
{
	m_texture = std::make_shared<sf::Texture>();
	
	if (!m_texture->loadFromFile(filepath))
	{
		std::cerr << "Failed to load texture: " << filepath << std::endl;
		return false;
	}
	
	m_sprite->setTexture(*m_texture);
	std::cout << "Loaded texture: " << filepath << std::endl;
	return true;
}

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