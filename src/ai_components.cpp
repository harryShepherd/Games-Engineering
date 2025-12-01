#include "ai_components.hpp"
#include "game_parameters.hpp"

/// <summary>
/// Seeks out the target.
/// </summary>
/// <param name="target">Usually the player, but could be a destination.</param>
/// <param name="self">The entity which is seeking.</param>
/// <returns>The SteeringOutput to reach the target.</returns>
SteeringOutput SteeringBehaviours::seek(const sf::Vector2f& target, const sf::Vector2f& self) {
	auto length = [](const sf::Vector2f& v) -> float {
		return std::sqrt(v.x * v.x + v.y * v.y);
	};
	SteeringOutput steering;
	steering.direction = target - self;
	steering.direction = steering.direction/length(steering.direction);
	steering.rotation = 0.0f;

	return steering;
}

/// <summary>
/// Flees from the target.
/// </summary>
/// <param name="target">Usually the player, but could be a destination.</param>
/// <param name="self">The entity which is seeking.</param>
/// <returns>SteeringOutput to flee the target.</returns>
SteeringOutput SteeringBehaviours::flee(const sf::Vector2f& target, const sf::Vector2f& self) {
	auto length = [](const sf::Vector2f& v) -> float {
		return std::sqrt(v.x * v.x + v.y * v.y);
	};
	SteeringOutput steering;
	steering.direction = self - target;
	steering.direction = steering.direction/length(steering.direction);
	steering.rotation = 0.0f;

	return steering;
}

/// <summary>
/// Updates the entity(s).
/// </summary>
/// <param name="dt">Deltatime - linked to frame rate.</param>
void SteeringComponent::update(const float& dt) {
	
}

SteeringComponent::SteeringComponent(Entity* p, Entity* player, float max_speed):
	_player(player), _max_speed(max_speed), Component(p){ }

//Checks if the entity is making a valid move.
	// This is temporary and should change when terrain factors in more.
bool SteeringComponent::valid_move(const sf::Vector2f& pos) const {
	if (pos.x <0.0f || pos.x > params::window_width || pos.y < 0.0f || pos.y > params::window_height) {
		return false;
	}
	return true;
}
