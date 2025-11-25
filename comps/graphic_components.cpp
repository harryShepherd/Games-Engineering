#include "graphic_components.hpp"
#include "renderer.hpp"

//Updates the shape.
void ShapeComponent::update(const float& dt) {
	_shape->setPosition(m_parent->get_position());
}

//Queues up the shape in the renderer.
void ShapeComponent::render() {
	Renderer::queue(_shape.get());
}

//Gets the shape.
sf::Shape& ShapeComponent::get_shape() const {
	return *_shape;
}