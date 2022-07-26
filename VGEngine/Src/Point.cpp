#include "ChaosGlobals.h"


Point::Point(Vector2D position) : GameObject()
{
	currentPos = position;
	lastPos = currentPos;
	lastDeltaTime = 0;
	acceleration = Vector2D(0,0);
	this->mass = mass;
}

Point::Point(Vector2D position, float mass) : GameObject( )
{	
	currentPos = position;
	lastPos = currentPos;
	lastDeltaTime = 0;
	acceleration = Vector2D(0,0);
	this->mass = mass;
}

Point::~Point()
{
}

void Point::Update(const int& dt )
{
	float deltaTimeMs = (float)dt/1000.0f;
	if( lastDeltaTime == 0 ) lastDeltaTime = deltaTimeMs;
	
	const float drag = 1;
	
	Vector2D vel = GetVelocity();
	AddForce(Vector2D(-vel.x*drag,-vel.y*drag));
	
	Vector2D newPos;
	newPos.x = currentPos.x + (currentPos.x - lastPos.x) * (deltaTimeMs / lastDeltaTime) + acceleration.x * deltaTimeMs * deltaTimeMs;
	newPos.y = currentPos.y + (currentPos.y - lastPos.y) * (deltaTimeMs / lastDeltaTime) + acceleration.y * deltaTimeMs * deltaTimeMs;

	lastPos = currentPos;
	currentPos = newPos;

	lastDeltaTime = deltaTimeMs;

	acceleration = Vector2D(0,0);
}

Vector2D Point::GetPosition()
{
	return currentPos;
}

void Point::Draw()
{	
	Primitive_Point(Vector2D(currentPos.x,currentPos.y)).Draw(Color(0.5,0.5,0.5,0.2));
}

void Point::AddForce(Vector2D force)
{
	acceleration.x += force.x/mass;
	acceleration.y += force.y/mass;
}

float Point::GetMass()
{
	return mass;
}

Vector2D Point::GetVelocity()
{
	return Vector2D((currentPos.x - lastPos.x) / lastDeltaTime,(currentPos.y - lastPos.y) / lastDeltaTime );
}

void Point::SetPosition(Vector2D pos)
{
	currentPos = pos;
	lastPos = pos;
}

