#pragma once
#include "glm/vec2.hpp"
#include <vector>

class PhysicsObject;							//forward declaration of PhysicObject class

class PhysicsScene
{
public:
	PhysicsScene();										//Default constructor
	~PhysicsScene();									//Default destructor

	void AddActor(PhysicsObject* actor);				//Adds actor to actors vector
	void RemoveActor(PhysicsObject* actor);				//Removes actor from actors vector
	void Update(float deltaTime);						//Updates physical simulation
	void Draw();										//Draws physical objects



	void SetGravity(const glm::vec2 gravity) { this->gravity = gravity; }
	glm::vec2 GetGravity() const { return this->gravity; }

	void SetTimeStep(const float timeStep) { this->timeStep = timeStep; }
	float GetTimeStep() const { return this->timeStep; }

	void CheckForCollision();

	static bool Plane2Plane(PhysicsObject*, PhysicsObject*);
	static bool Plane2Sphere(PhysicsObject*, PhysicsObject*);
	static bool Plane2AABB(PhysicsObject*, PhysicsObject*);
	static bool Sphere2Plane(PhysicsObject*, PhysicsObject*);
	static bool Sphere2Sphere(PhysicsObject*, PhysicsObject*);
	static bool Sphere2AABB(PhysicsObject*, PhysicsObject*);
	static bool AABB2Plane(PhysicsObject*, PhysicsObject*);
	static bool AABB2Sphere(PhysicsObject*, PhysicsObject*);
	static bool AABB2AABB(PhysicsObject*, PhysicsObject*);

protected:
	glm::vec2 gravity;
	float timeStep;
	std::vector<PhysicsObject*> actors;			//Stores all the physical objects in the scene ... This should be a list, according to tutorial!
};