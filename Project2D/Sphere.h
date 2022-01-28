#pragma once
#include "Rigidbody.h"

//We haven't added FixedUpdate(), ApplyForce(), or ApplyForceToActor() here because they are implemented in the rigidbody base class; no need to overload them.
//But since shape sub-classes use different Gizmos to represent them visually, they have unique Draw() functions.

class Sphere : public Rigidbody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isStatic);
	~Sphere();

	virtual void Draw();

	float GetRadius() { return radius; }
	glm::vec4 GetColour() { return colour; }

protected:
	float radius;
	glm::vec4 colour;
};