#pragma once
#include "ecm.hpp"

class KeyboardMovementComponent : public Component {
	protected:
		// The speed at which the entity can travel.
		float speed;
		// Checking if movement is valid.
		bool valid_move(const sf::Vector2f&);

	public:
		// Checks the keyboard and moves the component's parent.
		void update(const float& dt) override;
		// Moves the component parent
		void move(const sf::Vector2f&);
		// Moves the component parent.
		void move(float x, float y);
		// This component doesn't need to be rendered and should be marked as such.
		void render() override {}
		// Creates the component for an entity.
		explicit KeyboardMovementComponent(Entity* p);

		KeyboardMovementComponent() = delete;
};