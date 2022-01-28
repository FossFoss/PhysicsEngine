#pragma once
#include "PhysicsObject.h"
#include "ContactPoint.h"
#include <vector>

class Rigidbody : public PhysicsObject
{
public:

	Rigidbody(ShapeType shapeID);	//used for plane, no mass, velocity, etc.)
	//Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float orientation, float mass);
	Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float orientation, float mass, bool isStatic);
	//Rigidbody(ShapeType shapeID, glm::vec2 velocity, float orientation, float mass);		//used for AABB; not yet defined in .cpp file!!!!
	//You might need a constructor without position and orientation for AABBs
	~Rigidbody();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	void ApplyForce(glm::vec2 force);
	void ApplyForceToActor(Rigidbody* actor, glm::vec2 force);	// You can lose this function, if you want.
	void ResolveCollision(Rigidbody* actor, glm::vec2* collisionNormal=nullptr);
	void AddToPosition(glm::vec2 offset);						//Used for rectifying intersection of colliding objects
	float GetKineticEnergy();

	//void AddContactPoint(ContactPoint* contactPoint);
	//void DrawContactPoints();

	glm::vec2 GetPosition() { return position; }
	float GetOrientation() { return orientation; }
	glm::vec2 GetVelocity() { return velocity; }
	void SetVelocity(glm::vec2 velocity) { this->velocity = velocity; }
	float GetMass() { return mass; }
	void SetMass(float mass) { this->mass = mass; }
	bool IsStatic() { return isStatic; }
	void SetStatic(bool isStatic) { this->isStatic = isStatic; }
	void SetElasticity(float elasticity) { this->elasticity = elasticity; }
	void SetDamping(float damping) { this->damping = damping; }
	void SetPosition(glm::vec2 position) { this->position = position; }

protected:

	glm::vec2 position;
	glm::vec2 velocity;
	float mass;
	float damping = 0.002f;										//used for simulating air resistance (can also have getters/setters for custom amounts per objects)
	float orientation;											//How much an object is rotatedin radians
	bool isStatic;
	float elasticity;

	//std::vector<ContactPoint*> contactPoints;
};