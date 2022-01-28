#include "Rigidbody.h"
#include "glm/ext.hpp"
#include <iostream>
//#include "GameDefines.h"

Rigidbody::Rigidbody(ShapeType shapeID)
{
	this->shapeID = shapeID;
}

Rigidbody::Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float orientation, float mass, bool isStatic)
{
	this->shapeID = shapeID;
	this->position = position;
	this->velocity = velocity;
	this->orientation = orientation;
	this->mass = mass;
	this->isStatic = isStatic;
	this->elasticity = 0.8f;	//default value
}

Rigidbody::~Rigidbody()
{	
}

void Rigidbody::FixedUpdate(glm::vec2 gravity, float timeStep)
{
	velocity *= (1 - damping);	//simulates air reistance
	//Add updated vector to actor's position to get new position
	position += velocity * timeStep;

	ApplyForce(gravity * mass * timeStep);	//apply gravity (without applying force to a contactpoint)

	//DEBUGGING
	//DrawContactPoints();
}

//Apply Newton's second law; modifies the velocity of the object according to its mass and force applied
void Rigidbody::ApplyForce(glm::vec2 force)
{
	if (!IsStatic())
	{
		//Use GetMass() and GetMoment() here in case we ever need them to do something more than just return their values 
		//Calculate the object's acceleration
		glm::vec2 acceleration = force / GetMass();
		//Add acceleration to object's velocity
		velocity += acceleration;
	}
}

//Simulates one actor 'pushing' another
void Rigidbody::ApplyForceToActor(Rigidbody* actor, glm::vec2 force)
{
	//Apply input force to input actor (i.e. the actor this is hitting)
	actor->ApplyForce(force);

	//Apply opposite force to this actor
	ApplyForce(-force);
}

//NOTE: You should update this to check if one of the collision objects is static (or kinematic) and, if so, uses an infinite mass inthose calculations
void Rigidbody::ResolveCollision(Rigidbody* actor, glm::vec2* collisionNormal)
{
	//Calculate the collision normal (normallized difference in position -- okay for spheres
	glm::vec2 normal = collisionNormal ? *collisionNormal : glm::normalize(actor->GetPosition() - this->GetPosition());
	//glm::vec2 normal = glm::normalize(actor->GetPosition() - position);
	
	//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
	glm::vec2 relativeVelocity = actor->GetVelocity() - this->GetVelocity();

	glm::vec2 force;
	
	if (this->IsStatic() && !(actor->IsStatic()))
	{
		float j = glm::dot(-(1 + this->elasticity) * (relativeVelocity), normal) / (1 / actor->GetMass());
		force = normal * j;
		actor->ApplyForce(force);
	}
	else if (actor->IsStatic() && !(this->IsStatic()))
	{
		float j = glm::dot(-(1 + this->elasticity) * (relativeVelocity), normal) / ((1 / mass));
		force = normal * j;
		ApplyForce(-force);
	}
	else
	{
		float j = glm::dot(-(1 + this->elasticity) * (relativeVelocity), normal) / ((1 / mass) + (1 / actor->GetMass()));
		force = normal * j;
		ApplyForce(-force);
		actor->ApplyForce(force);
	}	
	//DEBUG CREATE CONTACT POINTS
	//ContactPoint* contactPoint = new ContactPoint(contact);
	//AddContactPoint(contactPoint);
}

void Rigidbody::AddToPosition(glm::vec2 offset)
{
	position += offset;
}

//Calculates linear kinetic energy
float Rigidbody::GetKineticEnergy()
{
	float velMagnitude = glm::length(velocity);
	float kineticEnergy = 0.5f * mass * pow(velMagnitude, 2);
	return kineticEnergy;
}

//void Rigidbody::AddContactPoint(ContactPoint* contactPoint)
//{
//	contactPoints.push_back(contactPoint);
//}

//void Rigidbody::DrawContactPoints()
//{
//	for (int i = 0; i < contactPoints.size(); i++)
//	{
//		aie::Gizmos::add2DCircle(contactPoints[i]->GetPosition(), 2, 24, glm::vec4(1, 1, 1, 1));
//	}
//}