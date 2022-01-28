#include "PhysicsScene.h"
#include <algorithm>		//For std::remove function
#include "PhysicsObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "AABB.h"
#include <glm/ext.hpp>
#include <iostream>

PhysicsScene::PhysicsScene()
{
	timeStep = 0.01f;
	gravity = { 0,0 };
}

PhysicsScene::~PhysicsScene()
{
	//Deallocate memory when cleaing up scene
	for (auto actor : actors)
	{
		delete actor;
	}
}

//Adds as actor to the end of actors vector
void PhysicsScene::AddActor(PhysicsObject* actor)
{
	actors.push_back(actor);
}

//Removes a specific actor from actors vector (erase-remove method)
void PhysicsScene::RemoveActor(PhysicsObject* actor)
{
	actors.erase(std::remove(actors.begin(), actors.end(), actor), actors.end());
}

//Updates physics at a fixed time step
void PhysicsScene::Update(float deltaTime)
{
	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;

	while (accumulatedTime >= timeStep)
	{
		for (auto actor : actors)
		{
			actor->FixedUpdate(gravity, timeStep);
		}

		accumulatedTime -= timeStep;
	}

	CheckForCollision();
}

//Draws the physics objects
void PhysicsScene::Draw()
{
	for (auto actor : actors)
	{
		actor->Draw();
	}
}

//Function pointer array for handling collisions
typedef bool (*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	PhysicsScene::Plane2Plane, PhysicsScene::Plane2Sphere, PhysicsScene::Plane2AABB,
	PhysicsScene::Sphere2Plane, PhysicsScene::Sphere2Sphere, PhysicsScene::Sphere2AABB,
	PhysicsScene::AABB2Plane, PhysicsScene::AABB2Sphere, PhysicsScene::AABB2AABB
};

void PhysicsScene::CheckForCollision()
{
	//Check for collisions (ideally you'd have some sort of scene management, like spatial partitioning, in place)
	int actorCount = actors.size();

	//Need to check for collisions against all objects except this one
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		//Starts at outer + 1 rather than 0; only want to compare each pair of objects once rather than twice, and not object against itself
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = actors[outer];
			PhysicsObject* object2 = actors[inner];
			int shapeID1 = object1->GetShapeID();
			int shapeID2 = object2->GetShapeID();

			//This check will ensure we don't include any springs in the collision checks
			if (shapeID1 < 0 || shapeID2 < 0)
				continue;

			//Using function pointers
			//Combine the shape IDs of both objects to form an index into the array
			int functionIndex = (shapeID1 * SHAPE_COUNT) + shapeID2; //SHAPE_COUNT is the final element in the ShapeType enum, and comes out as the number of physics object types you're interested in
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];

			//Provides function pointer for the function you need to perform the correct collision type
			if (collisionFunctionPtr != nullptr)
			{
				//Did a collision occur?
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}

//Planes are static and require no collision response
bool PhysicsScene::Plane2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

//Reverses the objects and calls Sphere2Plane
bool PhysicsScene::Plane2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return Sphere2Plane(object2, object1);
}

bool PhysicsScene::Plane2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Plane* plane = dynamic_cast<Plane*>(object1);
	AABB* aabb = dynamic_cast<AABB*>(object2);

	if (plane != nullptr && aabb != nullptr)
	{
		glm::vec2 collisionNormal = plane->GetNormal();

		//Create an array of four vertices and set them to the corners of the AABB
		glm::vec2 vertices[4];
		vertices[0] = aabb->GetPosition() - (aabb->GetExtents()); //bottom left
		vertices[1] = aabb->GetPosition() + (aabb->GetExtents()); //top right
		vertices[2].x = vertices[0].x; //top left
		vertices[2].y = vertices[1].y;
		vertices[3].x = vertices[1].x; //bottom right
		vertices[3].y = vertices[0].y;

		//Loop through the points ...
		for (int i = 0; i < 4; i++)
		{
			//Same as Sphere2Plane check, but with points rather than position.
			//Perform dot product of the plane's normal against each corner of the AABB; get a scalar
			float dotProduct = glm::dot(collisionNormal, vertices[i]);

			//Check the length of the vector between each point and the plane's distance from origin (i.e. the closest point on the plane)
			float distance = dotProduct - plane->GetDistance();

			if (distance < 0)
			{
				//Rectify overlap
				aabb->AddToPosition(-collisionNormal * distance * 1.01f); //i.e. overlap plus a bit extra
				plane->ResolveCollision(aabb);
				return true;
			}
		}
	}

	return false;
}

bool PhysicsScene::Sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	//Cast the objects to the correct shapes
	Sphere* sphere = dynamic_cast<Sphere*>(object1);
	Plane* plane = dynamic_cast<Plane*>(object2);

	//If cast is successful, check for collisions
	if (sphere != nullptr && plane != nullptr)
	{	
		glm::vec2 collisionNormal = plane->GetNormal();
		//Calculate dot product of plane's normal and sphere's position to get distance
		//from the sphere to the origin projected along plane's normal.

		//Calculate the contact point (i.e. the point on the sphere's circumference that lies along the collision normal
		glm::vec2 contact = sphere->GetPosition() + (collisionNormal * -sphere->GetRadius());

		//Subtract distance of the plane to the origin (plane's distance member variable) to
		//get distance from sphere's centre to plane.

		float sphereToPlane = glm::dot(sphere->GetPosition(), collisionNormal) - plane->GetDistance();

		//Subtract sphere's radius from this distance to get distance between sphere's surface
		//and plane.

		float intersection = sphere->GetRadius() - sphereToPlane;

		//For checking that sphere isn't travelling OUT of the plane (i.e. frame after bounce)
		float velocityOutOfPlane = glm::dot(sphere->GetVelocity(), collisionNormal);

		if (intersection > 0 && velocityOutOfPlane < 0)
		{
			//Rectify overlap
			sphere->AddToPosition(collisionNormal * intersection * 1.01f); //i.e. overlap plus a bit extra
			plane->ResolveCollision(sphere);
			return true;
		}
	}
	return false;
}

bool PhysicsScene::Sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	//Try to cast objects to sphere and sphere -- common use of dynamic casting in OOP, casitng up from a base class to a derived class
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(object2);

	//If cast is successful, test for collision
	if (sphere1 != nullptr && sphere2 != nullptr)
	{
		//Calculate a vector between the spheres and compare it to the sum of their radii
		glm::vec2 distanceVector = sphere1->GetPosition() - sphere2->GetPosition();
		float distance = glm::length(distanceVector);
		float overlap = sphere1->GetRadius() + sphere2->GetRadius() - distance;

		if (overlap > 0)
		{
			glm::vec2 collisionNormal = glm::normalize(distanceVector);
			glm::vec2 collisionPoint = sphere1->GetPosition() + (collisionNormal * sphere1->GetRadius());

			//Calculate the contact point of the spheres (i.e. half the sum of their positions
			//(By adding their positions and diving by 2 you get the position that's halfway between them)
			glm::vec2 contact = 0.5f * (sphere1->GetPosition() + sphere2->GetPosition());

			//Rectify intersection
			//This rectification calculation could be moved into a separate function that takes in shape types, casts them and returns a vector
			if (sphere1->IsStatic() && !(sphere2->IsStatic()))
			{
				sphere2->AddToPosition(-collisionNormal * overlap * 1.01f);
			}
			else if (!(sphere1->IsStatic()) && sphere2->IsStatic())
			{
				sphere1->AddToPosition(collisionNormal * overlap * 1.01f);
			}
			else
			{
				sphere1->AddToPosition(collisionNormal * overlap * 0.51f); //i.e. half the overlap plus a bit extra
				sphere2->AddToPosition(-collisionNormal * overlap * 0.51f);		
			}
		
			sphere1->ResolveCollision(sphere2);
			return true;
		}
	}
	return false;
}

bool PhysicsScene::Sphere2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object1);
	AABB* aabb = dynamic_cast<AABB*>(object2);

	if (sphere != nullptr && aabb != nullptr)
	{
		glm::vec2 distanceVector = sphere->GetPosition() - aabb->GetPosition();
		glm::vec2 collisionNormal = glm::normalize(distanceVector);

		
		//find closest point on edge of AABB to the centre of the circle (an optimized check Tony found online)
		glm::vec2 closestPoint;

		//Get the top right and bottom left points on the box
		glm::vec2 aabbTopRight = aabb->GetPosition() + aabb->GetExtents();
		glm::vec2 aabbBottomLeft = aabb->GetPosition() - aabb->GetExtents();

		//If the sphere's position is less than aabbBottomLeft, set it equal to aabbBottomLeft; if it's greater than aabbTopRight, set it equal to aabbTopRight
		//arguments are point to check, minValue, maxValue
		//By clamping this way, you get the point on the box which is as close as possible to the shhere's centre
		closestPoint = glm::clamp(sphere->GetPosition(), aabbBottomLeft, aabbTopRight);
		
		//Get a vector from the sphere's position to this point and get its length
		glm::vec2 delta = sphere->GetPosition() - closestPoint;
		float distance = glm::length(delta);

		if (distance < sphere->GetRadius())
		{
			float overlap = distance - sphere->GetRadius();
			
			if (sphere->IsStatic() && !(aabb->IsStatic()))
			{
				aabb->AddToPosition(collisionNormal * overlap * 1.01f);
			}
			else if (!(sphere->IsStatic()) && aabb->IsStatic())
			{
				sphere->AddToPosition(-collisionNormal * overlap * 1.01f);
			}
			else
			{
				sphere->AddToPosition(-collisionNormal * overlap * 0.51f); //i.e. half the overlap plus a bit extra
				aabb->AddToPosition(collisionNormal * overlap * 0.51f);
			}

			sphere->ResolveCollision(aabb);
			return true;
		}
	}
	return false;
}

bool PhysicsScene::AABB2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return Plane2AABB(object2, object1);
}

bool PhysicsScene::AABB2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return Sphere2AABB(object2, object1);
}

bool PhysicsScene::AABB2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	AABB* aabb1 = dynamic_cast<AABB*>(object1);
	AABB* aabb2 = dynamic_cast<AABB*>(object2);

	if (aabb1 != nullptr && aabb2 != nullptr)
	{
		//glm::vec2 distanceVector = aabb1->GetPosition() - aabb2->GetPosition();
		glm::vec2 collisionNormal = { 0, 0 };
		
		//Subtract each AABB's position from the sum of their half extents; the difference being how far they need to be 'pushed' to not overlap
		glm::vec2 collisionPoint = 0.5f * (aabb1->GetPosition() + aabb2->GetPosition());	//this is a fudge
		float hOverlap = (aabb1->GetExtents().x + aabb2->GetExtents().x) - abs(aabb1->GetPosition().x - aabb2->GetPosition().x);
		float vOverlap = (aabb1->GetExtents().y + aabb2->GetExtents().y) - abs(aabb1->GetPosition().y - aabb2->GetPosition().y);

		//Early exit if there is only one axis of overlap (i.e. no collision)
		if (hOverlap < 0 || vOverlap < 0)
		{
			return false;
		}

		glm::vec2 overlapCorrection = { 0, 0 };

		if (hOverlap < vOverlap)
		{
			overlapCorrection.x = hOverlap * 1.01f;
			collisionNormal = { 1, 0 };

			//If aabb1 is 'to the right' of aabb2, reverse the collision normal and the sign of the correction
			if (aabb1->GetPosition().x > aabb2->GetPosition().x)
			{
				overlapCorrection.x *= -1;
				collisionNormal.x *= -1;
			}
		}
		else
		{
			overlapCorrection.y = vOverlap * 1.01f;
			collisionNormal = { 0, 1 };

			//If aabb1 is 'above' aabb2, reverse the collision normal and the sign of the correction
			if (aabb1->GetPosition().y > aabb2->GetPosition().y)
			{
				overlapCorrection.y *= -1;
				collisionNormal.y *= -1;
			}
		}

		if (aabb1->IsStatic() && !(aabb2->IsStatic()))
		{
			aabb2->AddToPosition(overlapCorrection);
		}
		else if (!(aabb1->IsStatic()) && aabb2->IsStatic())
		{
			aabb1->AddToPosition(-overlapCorrection);
		}
		else
		{
			aabb1->AddToPosition(-overlapCorrection);
			aabb2->AddToPosition(overlapCorrection);
		}

		aabb1->ResolveCollision(aabb2);
		return true;
	}
	return false;
}