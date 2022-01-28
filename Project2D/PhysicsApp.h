#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"

class Rigidbody;

class PhysicsApp : public aie::Application
{
public:

	PhysicsApp();
	virtual ~PhysicsApp();

	//If you want to capitalize these (as per your preferred style)
	//you'll need to change them in Application.h as well ><)
	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void SetUpScene(PhysicsScene* physicsScene);
	void SetUpGame(PhysicsScene* physicsScene);
	void CreateBall(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isStatic);
	void CreatePlane(float offset, float rotation);
	void CreateAABB(glm::vec2 position, glm::vec2 velocity, glm::vec2 extents, float mass, glm::vec4 colour, bool isStatic);
	//void CreateSpring(Rigidbody* body1, Rigidbody* body2, float restLength, float springCoefficient, float damping, glm::vec2 contact1, glm::vec2 contact2);
	void CreateRope(Rigidbody* rb1, Rigidbody* rb2, int numberOfLinks, float elasticity, float damping, glm::vec4 colour);
	void DrawArrow(glm::vec2 startPosition, glm::vec2 endPosition, glm::vec4 colour);
	void FireBall(glm::vec2 spawnPosition, glm::vec2 velocity, float velocityScale, glm::vec4 colour);
	bool CheckOutOfBounds(Rigidbody* rigidbody);
	glm::vec4 GetRandomColour();	//returns a random colour
	glm::vec2 ScreenToWorld(glm::vec2 screenPosition);

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	PhysicsScene* physicsScene;
	const float extents = 100.0f;
	const float aspectRatio = 16.0f / 9.0f;

	glm::vec2 sphereRedSpawn;
	glm::vec2 sphereGreenSpawn;
	glm::vec2 sphereBlueSpawn;

	glm::vec2 worldPosition = { 0, 0 };
	glm::vec2 downPosition = { 0, 0 };
	bool downPositionSet;
	bool mouseReleased;
};