#pragma once
#include "PhysicsObject.h"
#include "Rigidbody.h"

class Plane : public PhysicsObject
{
public:
	Plane();
	Plane(glm::vec2 normal, float distance, glm::vec4 colour);
	~Plane();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void Draw();
	virtual void ResetPosition();
	void ResolveCollision(Rigidbody* actor);

	glm::vec2 GetNormal() { return normal; }
	float GetDistance() { return distanceToOrigin; }

protected:
	glm::vec2 normal;
	float distanceToOrigin;
	glm::vec4 colour;

};
