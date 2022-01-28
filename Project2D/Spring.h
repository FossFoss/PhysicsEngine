#pragma once
#include "PhysicsObject.h"

class Rigidbody;

class Spring : public PhysicsObject
{
protected:

	Rigidbody* body1;				//The two bodies being connected
	Rigidbody* body2;
	float elasticity;				//
	float damping;					//
	float restLength;				//Ideal Length between points
	glm::vec4 colour;

public:

	Spring(Rigidbody* body1, Rigidbody* body2, float elasticity, float damping, glm::vec4 colour);
	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void Draw();
};