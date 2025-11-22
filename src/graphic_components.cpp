#include "graphic_components.hpp"
#include "renderer.hpp"

void ShapeComponent::update(const float &dt) {
  _shape->setPosition(m_parent->get_position());
}

void ShapeComponent::render() { Renderer::queue(_shape.get()); }

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
	m_sprite->setRotation(m_parent->get_rotation() / (3.1415f * 180.0f));
}

void SpriteComponent::render()
{
	Renderer::queue(m_sprite.get());
}