#pragma once
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"	//used by actors for storing colour

//Pure abstract base class; can't construct an instance of it; classes derived from this will have to provide
//implementations of FixedUpdate and Draw in order to be constructable.
//Using an enumerated type (over a seriesof hard-coded integers) is that the code is easier to read.

//CHANGE BOX TO AABB

enum ShapeType
{
	SPRING = -1,
	PLANE = 0,
	SPHERE,
	AABOUNDINGBOX,
	SHAPE_COUNT
};
//Shape count is the third element in this enum, so it's a constant of the number of shapes you're checking

class PhysicsObject
{
protected:
	PhysicsObject() {};			//Default constructor
	PhysicsObject(ShapeType shapeID) : shapeID(shapeID) {};

public:
	virtual void FixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void Draw() = 0;
	virtual void ResetPosition() {};
	ShapeType GetShapeID() { return shapeID; }

protected:
	ShapeType shapeID;		//Used for collision detection

	//You'll need to keep track of the previous and current position of each physics object
	//so that you can interpolate between the two states when drawing. This will prevent temporal aliasing.
	//
	//Would this be storedand updated within each object?
	//glm::vec2 previousPos;
	//glm::vec2 currentPos;
};
