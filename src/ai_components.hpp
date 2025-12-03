#pragma once
#include <SFML/Graphics.hpp>
#include "ecm.hpp"

/// <summary>
/// Output from the steering behaviour.
/// </summary>
struct SteeringOutput {
	// Direction of Travel
	sf::Vector2f direction;
	// Rotation of Travel
	float rotation = 0;
	static bool check_valid_move(const sf::Vector2f&, SteeringOutput steering);
	float steering_rotation(SteeringOutput output);
};

/// <summary>
/// Stores the steering behaviours.
/// </summary>
struct SteeringBehaviours {
	static SteeringOutput seek(const sf::Vector2f&, const sf::Vector2f&);
	static SteeringOutput flee(const sf::Vector2f&, const sf::Vector2f&);
};

class SteeringComponent : public Component {
	protected:
		Entity* _player;
		float _max_speed;
	public:
		void update(const float&) override;
		void render() override {}
		explicit SteeringComponent(Entity* p, Entity* player, float max_speed);
		SteeringComponent() = delete;
};