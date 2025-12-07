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

	bool valid = SteeringOutput::check_valid_move(self, steering);

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
		
	bool valid = SteeringOutput::check_valid_move(self, steering);
	return steering;
}

/// <summary>
/// Intended to check if the movement is valid.
/// </summary>
/// <param name="pos">Target position</param>
/// <param name="steering">The output steering.</param>
/// <returns>True</returns>
bool SteeringOutput::check_valid_move(const sf::Vector2f& pos, SteeringOutput steering) {
	return true;
}

/// <summary>
/// Updates the entity(s).
/// </summary>
/// <param name="dt">Deltatime - linked to frame rate.</param>
void SteeringComponent::update(const float& dt) {
	
}

/// <summary>
/// Creates a new steering component.
/// </summary>
/// <param name="e">The entity being given the component.</param>
/// <param name="player">The target entity.</param>
/// <param name="max_speed">The maximum speed for travel.</param>
SteeringComponent::SteeringComponent(Entity* e, Entity* target, float max_speed):
	_player(target), _max_speed(max_speed), Component(e){ }


