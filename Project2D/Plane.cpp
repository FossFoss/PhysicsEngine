#include "Plane.h"
#include "Gizmos.h"
#include "glm/ext.hpp"
#include <iostream>

Plane::Plane() : PhysicsObject(ShapeType::PLANE)
{
	distanceToOrigin = 0;
	normal = glm::vec2(0, 1);
	colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
}

//Do we need to provide info for the Physics Object constructor, like the sphere? (e.g. shape type)
Plane::Plane(glm::vec2 normal, float distance, glm::vec4 colour) : PhysicsObject(PLANE)
{
	this->normal = normal;
	this->distanceToOrigin = distance;
	this->colour = colour;
}

Plane::~Plane()
{
}

//Plane is static
void Plane::FixedUpdate(glm::vec2 gravity, float timeStep)
{
}

void Plane::Draw()
{
	float lineSegmentLength = 300;
	glm::vec2 centrePoint = normal * distanceToOrigin;
	// easy to rotate normal through 90 degrees around z
	glm::vec2 parallel(normal.y, -normal.x); //used for the colour fade, I guess?
	glm::vec4 colourFade = colour;
	colourFade.a = 0;
	glm::vec2 start = centrePoint + (parallel * lineSegmentLength);
	glm::vec2 end = centrePoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DTri(start, end, start - normal * 10.0f, colour, colour, colourFade);
	aie::Gizmos::add2DTri(end, end - normal * 10.0f, start - normal * 10.0f, colour, colourFade, colourFade);
}

void Plane::ResetPosition()
{
	normal = { 0, 0 };
	distanceToOrigin = 0;
}

//Resolve collisions between a rigidbody and a plane
//NOTE: This isn'tthe optimized implementation from the Sphere2Plane tutorial!
void Plane::ResolveCollision(Rigidbody* actor)
{
	//Calculate the collision normal (normallized difference in position -- okay for spheres
	//glm::vec2 normal = glm::normalize(actor->GetPosition() - distanceToOrigin);
	
	//Calculate relative velocity -- plane has no velocity, so just other object's velocity
	glm::vec2 relativeVelocity = actor->GetVelocity();

	//elasticityof 1 means no energy will be lost during collision
	//You could define values for each object and combine them to determine a more phisically convincing value
	float elasticity = 0.8f;

	//float elasticity = actor->GetElasticity();

	//j is impulse magnitude
	//float j = glm::dot((relativeVelocity - (1 + elasticity) * relativeVelocity), normal);
	float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal) / (1 / actor->GetMass());
	//Multiply impulse magnitude by collision normal to get force to apply to each object; apply this force (including negative force to self)
	glm::vec2 force = normal * j;

	actor->ApplyForce(force);
}