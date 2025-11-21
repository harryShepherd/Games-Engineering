#include "control_components.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"

void KeyboardMovementComponent::update(const float& dt) {
	sf::Vector2f direction(0.0f, 0.0f);
	if (sf::Keyboard::isKeyPressed(params::controls[1])) {
		direction.x -= 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(params::controls[3])) {
		direction.x += 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(params::controls[0])) {
		direction.y -= 1.0f;
	}
	if (sf::Keyboard::isKeyPressed(params::controls[2])) {
		direction.y += 1.0f;
	}
	move(direction * speed * (float)dt);
}

//Initialise Component
KeyboardMovementComponent::BasicMovementComponent(Entity* p)
	: speed(100.0f), Component(p) {
}

// Checks that movement is valid.
	// Note: Amend this to work with levels once this is functional.
bool KeyboardMovementComponent::valid_move(const sf::Vector2f& pos) {
	if (pos.x < 0.0f || pos.x > params::window_width ||
		pos.y < 0.0f || pos.y > param::game_height) {
		return false;
	}
	return true;
}

void KeyboardMovementComponent::move(const sf::Vector2f& p) {
	auto new_pos = m_parent->get_position() + p;
	if (valid_move(new_pos)) {
		m_parent->set_position(new_pos);
	}
}

void KeyboardMovementComponent::move(float x, float y) {
	move(sf::Vector2f(x, y));
}