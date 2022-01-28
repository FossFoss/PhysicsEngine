#include "AABB.h"
#include "Gizmos.h"


AABB::AABB(glm::vec2 position, glm::vec2 velocity, glm::vec2 extents, float mass, glm::vec4 colour, bool isStatic) : Rigidbody(AABOUNDINGBOX, position, velocity, 0, mass, false)
{
	this->extents = extents;
	this->colour = colour;
	this->isStatic = isStatic;
}

AABB::~AABB()
{
}

void AABB::Draw()
{
	//You can use this gizmo for oriented boxes by passing in a rotation matrix; by default it fills this with the identity matrix.
	//Rotated box is mostly the same code, but you need the matrix to handle rotation.
	aie::Gizmos::add2DAABBFilled(position, extents, colour);

	//Some students used a gizmo for arbitrary shape polygons, using separate-axis theorem to get collision normals
}

