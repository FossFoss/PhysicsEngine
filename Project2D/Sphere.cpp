#include "Sphere.h"
#include "Gizmos.h"
#include <cmath>
#include <iostream>

//Calls the base class (Rigidbody) constructor, passing in shape type
Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isStatic) : Rigidbody(SPHERE, position, velocity, 0, mass, false)
{
	this->radius = radius;
	this->colour = colour;
	this->isStatic = isStatic;
}

Sphere::~Sphere()
{
}

void Sphere::Draw()
{
	//glm::vec2 end = glm::vec2(std::cos(orientation), std::sin(orientation)) * radius;
	aie::Gizmos::add2DCircle(position, radius, 24, colour);
	//Draw a line to show the facing direction of the sphere
	//aie::Gizmos::add2DLine(position, position + end, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}