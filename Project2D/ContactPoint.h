#pragma once
#include "glm/vec2.hpp"
#include "glm/ext.hpp"
#include "Gizmos.h"

class ContactPoint
{
public:

	ContactPoint();
	ContactPoint(glm::vec2 position) { this->position = position; };
	~ContactPoint();

	void Draw()
	{
		aie::Gizmos::add2DCircle(position, 2, 24, glm::vec4(1, 1, 1, 1));
	}

	glm::vec2 GetPosition() { return position; }

protected:

	glm::vec2 position;
};