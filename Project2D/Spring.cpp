#include "Spring.h"
#include "Gizmos.h"
#include <glm/ext.hpp>
#include "Rigidbody.h"

Spring::Spring(Rigidbody* body1, Rigidbody* body2, float elasticity, float damping, glm::vec4 colour) : PhysicsObject(ShapeType::SPRING)
{
	this->body1 = body1;
	this->body2 = body2;
	this->elasticity = elasticity;
	this->damping = damping;
	this->restLength = glm::distance(body1->GetPosition(), body2->GetPosition());
	this->colour = colour;
}

void Spring::FixedUpdate(glm::vec2 gravity, float timeStep)
{
	/*
	//Get a vector between the two bodies
	glm::vec2 delta = body1->GetPosition() - body2->GetPosition();
	//Normalize to get its direction (unit vector)
	glm::vec2 forceDirection = glm::normalize(delta);
	//Getthe spring's current displacement (i.e. how much greater or less than the rest length the spring currently is
	float displacement = glm::length(delta) - restLength;
	//Apply forces to the bodies
	body1->ApplyForce(-forceDirection * displacement * elasticity);
	body2->ApplyForce(forceDirection * displacement * elasticity);
	*/

	glm::vec2 p2 = body2->GetPosition();
	glm::vec2 p1 = body1->GetPosition();
	glm::vec2 dist = p2 - p1;
	float length = sqrtf(dist.x * dist.x + dist.y * dist.y);
	//Apply damping
	glm::vec2 relativeVelocity = body2->GetVelocity() - body1->GetVelocity();
	//F = -kX - bv
	glm::vec2 force = dist * elasticity * (restLength - length) - damping * relativeVelocity;
	body1->ApplyForce(-force * timeStep);
	body2->ApplyForce(force * timeStep);
}

void Spring::Draw()
{
	aie::Gizmos::add2DLine(body1->GetPosition(), body2->GetPosition(), colour);
}
