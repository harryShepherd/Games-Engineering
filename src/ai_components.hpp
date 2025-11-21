#pragma once
#include <SFML/Graphics.hpp>

/// <summary>
/// Output from the steering behaviour.
/// </summary>
struct SteeringOutput {
	// Direction of Travel
	sf::Vector2f direction;
	// Rotation of Travel
	float rotation;
};