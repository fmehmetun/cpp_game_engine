#pragma once
#include "PxPhysicsAPI.h"

#include "Entity/Entity.h"

using namespace physx;

class Character : public Entity{
public:
	Character(EntityProperties entityProperties, CharacterProperties characterProperties) : Entity(entityProperties) {
		this->facingDirection = characterProperties.facingDirection;

		this->Setup();
	}
	void Setup();

	// General values.
	float health = 1.0f;
	Vector3 facingDirection;

	// Actions
	void Walk(Vector3 displacement);

	static Vector3 worldGravity;

private:
	float movementSpeed = 0.6f;
	bool walkedBefore = false;
	Timer characterTimer;
};