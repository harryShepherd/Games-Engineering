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
	auto distance = [](const sf::Vector2f& a, const sf::Vector2f& b) -> float {
		return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	};

	//If player is further than 100 pixels away then seek.
		// This is temporary.
	if (distance(m_parent->get_position(), _player->get_position()) > 100.0f) {
		SteeringOutput output = SteeringBehaviours::seek(_player->get_position(), m_parent->get_position());
		move(output.direction * _max_speed * (float)params::time_step);
	}
	//If player is closer than 50 pixels away then flee.
		// This is temporary.
	else if (distance(m_parent->get_position(), _player->get_position()) < 50.0f) {
		SteeringOutput output = SteeringBehaviours::flee(_player->get_position(), m_parent->get_position());
		move(output.direction * _max_speed * (float)params::time_step);
	}
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

/// <summary>
/// Moves the entity if their move is valid.
/// </summary>
/// <param name="p">Position to move to.</param>
void SteeringComponent::move(const sf::Vector2f& p) {
	sf::Vector2f new_pos = m_parent->get_position() + p;
	if (valid_move(new_pos)) {
		m_parent->set_position(new_pos);
	}
}

/// <summary>
/// Takes an x and y and makes a vector, then moves towards that if valid.
/// </summary>
/// <param name="x">X-Coordinate</param>
/// <param name="y">Y-Coordinate</param>
void SteeringComponent::move(float x, float y) {
	move(sf::Vector2f(x, y));
}