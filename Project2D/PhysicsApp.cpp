#include "PhysicsApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"
#include "glm/ext.hpp"
#include "Sphere.h"
#include "Plane.h"
#include "AABB.h"
#include "Spring.h"
#include <random>
#include <iostream>
#include "GameDefines.h"

Sphere* sphereRed;
Sphere* sphereGreen;
Sphere* sphereBlue;

PhysicsApp::PhysicsApp()
{
}

PhysicsApp::~PhysicsApp()
{
}

bool PhysicsApp::startup()
{
	//Increase the 2D line count to maximise the number of objects we can draw
	aie::Gizmos::create(255U, 255U, 65535U, 65535U);
	m_2dRenderer = new aie::Renderer2D();
	m_font = new aie::Font("./font/consolas.ttf", 32);	
	//m_timer = 0;

	//Create an instance of PhysicsScene and set the time step
	//Decreasing time step value will increase the accuracy of the physics simulation,
	//at the expense of processing time; setting the value to high will make the simulation stutter
	//and reduce its stability
	physicsScene = new PhysicsScene();
	physicsScene->SetGravity(glm::vec2(0, -90));	//set gravity to higher values to aply more gravity; (0, 0) is none
	physicsScene->SetTimeStep(0.01f);

	//SetUpScene(physicsScene);
	SetUpGame(physicsScene);
	mouseReleased = true;
	downPositionSet = false;

	return true;
}

void PhysicsApp::shutdown()
{
	delete m_font;
	delete m_2dRenderer;
	delete physicsScene;
}

void PhysicsApp::update(float deltaTime)
{
	// input example
	aie::Input* input = aie::Input::getInstance();

	//Removes all gizmos (from the renderer)
	aie::Gizmos::clear();

	//Updates and draws the PhysicsScene instance
	physicsScene->Update(deltaTime);
	physicsScene->Draw();

	if (input->isMouseButtonDown(0))
	{
		int xScreen, yScreen;
		input->getMouseXY(&xScreen, &yScreen);
		worldPosition = ScreenToWorld(glm::vec2(xScreen, yScreen));

		if (!downPositionSet)
		{
			downPosition = worldPosition;
			//std::cout << "set" << std::endl;
			downPositionSet = true;
			mouseReleased = false;
		}

		DrawArrow(downPosition, worldPosition, COLOUR_WHITE);
	}

	if (input->isMouseButtonUp(0) && mouseReleased == false)
	{
		glm::vec2 velocity = downPosition - worldPosition;
		float maxLength = 40.0f;

		if (glm::length(velocity) > maxLength)
		{
			glm::vec2 normal = glm::normalize(velocity);
			glm::vec2 truncatedPosition = downPosition - normal * maxLength;
			glm::vec2 truncatedVelocity = downPosition - truncatedPosition;
			FireBall(truncatedPosition, truncatedVelocity, 10.0f, COLOUR_WHITE);
			mouseReleased = true;
			downPositionSet = false;
		}
		else
		{
			FireBall(worldPosition, velocity, 10.0f, COLOUR_WHITE);
			mouseReleased = true;
			downPositionSet = false;
		}	
	}

	if (CheckOutOfBounds(sphereRed))
	{
		sphereRed->SetPosition(sphereRedSpawn);
		sphereRed->SetVelocity(glm::vec2(0, 0));
	}

	if (CheckOutOfBounds(sphereGreen))
	{
		sphereGreen->SetPosition(sphereGreenSpawn);
		sphereGreen->SetVelocity(glm::vec2(0, 0));
	}

	if (CheckOutOfBounds(sphereBlue))
	{
		sphereBlue->SetPosition(sphereBlueSpawn);
		sphereBlue->SetVelocity(glm::vec2(0, 0));
	}

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void PhysicsApp::draw()
{

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	//Draw your stuff here
	aie::Gizmos::draw2D(glm::ortho<float>(-extents, extents, -extents / aspectRatio, extents / aspectRatio, -1.0f, 1.0f));

	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 64);

	// done drawing sprites
	m_2dRenderer->end();
}

void PhysicsApp::SetUpScene(PhysicsScene* physicsScene)
{
	glm::vec4 colourWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 colourGreen = { 0.2f, 1.0f, 0.2f, 1.0f };
	
	//Create central rope
	Sphere* ropeStart = new Sphere(glm::vec2(0.0f, 40.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, true);
	//ropeStart->SetDamping(0.0f);
	physicsScene->AddActor(ropeStart);
	Sphere* ropeEnd = new Sphere(glm::vec2(-50.0f, 40.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, false);
	//ropeEnd->SetDamping(0.0f);
	physicsScene->AddActor(ropeEnd);
	float ropeElasticity = 150.0f;
	float ropeDamping = 20.0f;
	CreateRope(ropeStart, ropeEnd, 20, ropeElasticity, ropeDamping, colourWhite);

	//Create right-side rope
	Sphere* rope2Start = new Sphere(glm::vec2(60.0f, 40.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, true);
	//rope2Start->SetDamping(0.0f);
	physicsScene->AddActor(rope2Start);
	Sphere* rope2End = new Sphere(glm::vec2(90.0f, 40.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, false);
	//rope2End->SetDamping(0.0f);
	physicsScene->AddActor(rope2End);
	float rope2Elasticity = 150.0f;
	float rope2Damping = 20.0f;
	CreateRope(rope2Start, rope2End, 20, rope2Elasticity, rope2Damping, colourWhite);

	//Create bottom rope
	Sphere* rope3Start = new Sphere(glm::vec2(-30.0f, -20.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, true);
	//rope3Start->SetDamping(0.0f);
	physicsScene->AddActor(rope3Start);
	Sphere* rope3End = new Sphere(glm::vec2(30.0f, -20.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, colourWhite, true);
	//rope3End->SetDamping(0.0f);
	physicsScene->AddActor(rope3End);
	float rope3Elasticity = 150.0f;
	float rope3Damping = 10.0f;
	CreateRope(rope3Start, rope3End, 20, rope2Elasticity, rope2Damping, colourWhite);

	//Create box obstacles
	CreateAABB(glm::vec2(30.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 2.0f), 8.0f, colourGreen, true);
	CreateAABB(glm::vec2(38.0f, 10.0f), glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 8.0f), 8.0f, colourGreen, true);
	CreateAABB(glm::vec2(-40.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 2.0f), 8.0f, colourGreen, true);
	//CreateAABB(glm::vec2(0.0f, -40.0f), glm::vec2(0.0f, 0.0f), glm::vec2(16.0f, 4.0f), 8.0f, colourGreen, true);

	//Create planes
	//CreatePlane(-48.0f, 0.0f);
	CreatePlane(-70.0f, -0.5f);
	CreatePlane(-70.0f, 0.5f);

}

void PhysicsApp::SetUpGame(PhysicsScene* physicsScene)
{
	//Create red rope
	Sphere* ropeRedStart = new Sphere(glm::vec2(-85.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_RED, true);
	//ropeStart->SetDamping(0.0f);
	physicsScene->AddActor(ropeRedStart);
	Sphere* ropeRedEnd = new Sphere(glm::vec2(-35.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_RED, true);
	//ropeEnd->SetDamping(0.0f);
	physicsScene->AddActor(ropeRedEnd);
	float ropeRedElasticity = 800.0f;
	float ropeRedDamping = 20.0f;
	CreateRope(ropeRedStart, ropeRedEnd, 20, ropeRedElasticity, ropeRedDamping, COLOUR_RED);

	//Create green rope
	Sphere* ropeGreenStart = new Sphere(glm::vec2(35.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_GREEN, true);
	//ropeStart->SetDamping(0.0f);
	physicsScene->AddActor(ropeGreenStart);
	Sphere* ropeGreenEnd = new Sphere(glm::vec2(85.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_GREEN, true);
	//ropeEnd->SetDamping(0.0f);
	physicsScene->AddActor(ropeGreenEnd);
	float ropeGreenElasticity = 800.0f;
	float ropeGreenDamping = 20.0f;
	CreateRope(ropeGreenStart, ropeGreenEnd, 20, ropeGreenElasticity, ropeGreenDamping, COLOUR_GREEN);

	//Create blue rope
	Sphere* ropeBlueStart = new Sphere(glm::vec2(-25.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_BLUE, true);
	//ropeStart->SetDamping(0.0f);
	physicsScene->AddActor(ropeBlueStart);
	Sphere* ropeBlueEnd = new Sphere(glm::vec2(25.0f,-0.0f), glm::vec2(0.0f, 0.0f), 2.0f, 2.0f, COLOUR_BLUE, true);
	//ropeEnd->SetDamping(0.0f);
	physicsScene->AddActor(ropeBlueEnd);
	float ropeBlueElasticity = 800.0f;
	float ropeBlueDamping = 20.0f;
	CreateRope(ropeBlueStart, ropeBlueEnd, 20, ropeBlueElasticity, ropeBlueDamping, COLOUR_BLUE);

	//Create roof
	Plane* roof = new Plane(glm::vec2(0.0f, -1.0f), -50.0f, COLOUR_BROWN);
	physicsScene->AddActor(roof);

	//Spawn balls
	sphereRedSpawn = glm::vec2(55.0f, 70.0f);
	sphereGreenSpawn = glm::vec2(0.0f, 70.0f);
	sphereBlueSpawn = glm::vec2(-55.0f, 70.0f);
	sphereRed = new Sphere(sphereRedSpawn, glm::vec2(0, 0), 4.0f, 8.0f, COLOUR_RED, false);
	physicsScene->AddActor(sphereRed);
	sphereGreen = new Sphere(sphereGreenSpawn, glm::vec2(0, 0), 3.0f, 6.0f, COLOUR_GREEN, false);
	physicsScene->AddActor(sphereGreen);
	sphereBlue = new Sphere(sphereBlueSpawn, glm::vec2(0, 0), 2.0f, 4.0f, COLOUR_BLUE, false);
	physicsScene->AddActor(sphereBlue);

	//DEBUGGING
	//AABB* aabb = new AABB(glm::vec2(0, 20), glm::vec2(0, 0), glm::vec2(4, 4), 4.0f, GetRandomColour(), true);
	//physicsScene->AddActor(aabb);
	//	
	//Sphere* sphere = new Sphere(glm::vec2(55, 20), glm::vec2(0, 0), 6.0f, 6.0f, GetRandomColour(), true);
	//physicsScene->AddActor(sphere);
}

void PhysicsApp::CreateBall(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isStatic)
{
	Sphere* sphere = new Sphere(position, velocity, mass, radius, colour, isStatic);

	if (sphere->IsStatic())
	{
		sphere->SetMass(std::numeric_limits<float>::max());
	}
	
	physicsScene->AddActor(sphere);
}

void PhysicsApp::CreatePlane(float offset, float rotation)
{
	glm::vec2 planeNormal;
	if (rotation >= 0)
	{
		planeNormal = { 0,1 };
		planeNormal.x = rotation;
		planeNormal.y -= rotation;
		Plane* plane = new Plane(glm::vec2(sqrt(planeNormal.x), sqrt(planeNormal.y)), offset, glm::vec4(0.8, 0.6, 0.1, 1));
		physicsScene->AddActor(plane);
	}
	else
	{
		planeNormal = { 1,0 };
		planeNormal.x += rotation;
		planeNormal.y = -rotation;
		Plane* plane = new Plane(glm::vec2(-sqrt(planeNormal.x), sqrt(planeNormal.y)), offset, glm::vec4(0.8, 0.6, 0.1, 1));
		physicsScene->AddActor(plane);
	}
}

void PhysicsApp::CreateAABB(glm::vec2 position, glm::vec2 velocity, glm::vec2 extents, float mass, glm::vec4 colour, bool isStatic)
{
	AABB* aabb = new AABB(position, velocity, extents, mass, colour, isStatic);

	if (aabb->IsStatic())
	{
		aabb->SetMass(std::numeric_limits<float>::max());
	}

	physicsScene->AddActor(aabb);
}

void PhysicsApp::CreateRope(Rigidbody* rb1, Rigidbody* rb2, int numberOfLinks, float elasticity, float damping, glm::vec4 colour)
{
	Rigidbody* previous = rb1;
	Rigidbody* next;
	//Get vector between two bodies you're connecting; get its direction and magnitude
	glm::vec2 delta = rb2->GetPosition() - rb1->GetPosition();
	glm::vec2 direction = glm::normalize(delta);
	float distance = glm::length(delta);

	//Length of each link
	float increment = distance / (numberOfLinks + 1);

	for (int i = 0; i < numberOfLinks; i++)
	{
		//Create a new sphere (increment) distance from the previus sphere in (direction); add to scene
		glm::vec2 nextPosition = previous->GetPosition() + (increment * direction);
		Sphere* next = new Sphere(nextPosition, glm::vec2(0, 0), 1, 0.5, colour, false);
		next->SetDamping(0.01f);
		physicsScene->AddActor(next);
		//Create spring joint between previous and next spheres; add to scene
		Spring* joint = new Spring(previous, next, elasticity, damping, colour);
		physicsScene->AddActor(joint);
		previous = next;
	}

	//Add rb2 to the end of the rope
	float lastDistance = glm::distance(previous->GetPosition(), rb2->GetPosition());
	Spring* joint = new Spring(previous, rb2, elasticity, damping, colour);
	physicsScene->AddActor(joint);
}

void PhysicsApp::DrawArrow(glm::vec2 startPosition, glm::vec2 endPosition, glm::vec4 colour)
{
	//Get a vector that defines the arrow shaft
	glm::vec2 arrowVector = endPosition - startPosition;
	float maxLength = 40.0f;

	if (glm::length(arrowVector) > maxLength)
	{
		glm::vec2 normal = glm::normalize(arrowVector);
		endPosition = startPosition + normal * maxLength;
	}

	//Normalize to get its direction (unit vector)
	glm::vec2 arrowDirection = glm::normalize(arrowVector);
	//Get the vector perpendicular to the normal
	glm::vec2 arrowDirectionPerp(arrowDirection.y, -arrowDirection.x);

	//Draw the arrow shaft
	aie::Gizmos::add2DLine(startPosition, endPosition, colour);

	//Draw the arow head
	float triangleSize = 2.0f;
	glm::vec2 vertex1 = downPosition;
	glm::vec2 vertex2 = downPosition + (triangleSize * arrowDirection) + (triangleSize * arrowDirectionPerp);
	glm::vec2 vertex3 = downPosition + (triangleSize * arrowDirection) - (triangleSize * arrowDirectionPerp);
	aie::Gizmos::add2DTri(vertex3, vertex1, vertex2, colour);

	//Draw the placeholder projectile
	aie::Gizmos::add2DCircle(endPosition, 4, 32, colour);
}

void PhysicsApp::FireBall(glm::vec2 spawnPosition, glm::vec2 velocity, float velocityScale, glm::vec4 colour)
{
	//Scale up the velocity
	velocity.x *= velocityScale;
	velocity.y *= velocityScale;
	
	//Spawn ball
	Sphere* projectile = new Sphere(spawnPosition, velocity, 8.0f, 6.0f, colour, false);
	physicsScene->AddActor(projectile);
}

bool PhysicsApp::CheckOutOfBounds(Rigidbody* rigidbody)
{
	if (rigidbody->GetPosition().x < -120 || rigidbody->GetPosition().x > 120 || rigidbody->GetPosition().y < -80)
	{
		return true;
	}
	
	return false;
}

glm::vec4 PhysicsApp::GetRandomColour()
{
	glm::vec4 randomColour;
	randomColour.w = 1;

	//Obtain a random number from hardware
	std::random_device rd;
	//Seed the generator
	std::mt19937 gen(rd());
	//Define the range(inclusive)
	std::uniform_real_distribution<> distr(0, 1);

	for (int i = 0; i < 40; i++)
	{
		randomColour.x = distr(gen);
	}

	for (int i = 0; i < 40; i++)
	{
		randomColour.y = distr(gen);
	}

	for (int i = 0; i < 40; i++)
	{
		randomColour.z = distr(gen);
	}

	return randomColour;
}

glm::vec2 PhysicsApp::ScreenToWorld(glm::vec2 screenPosition)
{
	glm::vec2 worldPosition = screenPosition;

	//Move the centre of he screen to (0, 0)
	worldPosition.x -= getWindowWidth() / 2.0f;
	worldPosition.y -= getWindowHeight() / 2.0f;

	//Scale according to extents
	worldPosition.x *= 2.0f * extents / getWindowWidth();
	worldPosition.y *= 2.0f * extents / (aspectRatio * getWindowHeight());

	return worldPosition;
}