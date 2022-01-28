#pragma once
#include "Rigidbody.h"

class AABB : public Rigidbody
{
public:
	AABB(glm::vec2 position, glm::vec2 velocity, glm::vec2 extents, float mass, glm::vec4 colour, bool isStatic);
	~AABB();

	virtual void Draw();

	//void Empty();	//reset min/max values to infinity
	//void AddPoints(glm::vec2 point);	//expands the AABB if necessary to contin a new point

	glm::vec2 GetExtents() { return extents; }
	//glm::vec2 GetColour() { return colour; }

protected:
	glm::vec2 extents;
	glm::vec4 colour;
};