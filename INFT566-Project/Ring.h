#pragma once
#include "Gizmos.h"

#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"

#include "Config.h"

#include <random>
#include <iostream>

using glm::vec3;
using glm::vec2;
using glm::vec4;
using glm::mat4;

enum RingType
{
	RED,
	GREEN,
	BLUE	
};

class Ring
{
public:
	Ring();
	~Ring();

	bool startup();
	void shutdown();
	void update(float deltaTime);
	void draw();
	
	void setSpeed(float a_speed) { speed = a_speed; }
	float getSpeed() { return speed; }

	void spawn();

	float getX(){return x;}
	float getY(){return y;}
	float getZ(){return z;}

	vec4 getColour() { return colour; }

protected:

private:
	float speed;

	float x;
	float y; 
	float z;

	glm::mat4 model;
	glm::vec4 colour;

	RingType type;

	void randomType();
	void setColour();

	float RandomFloat(float a, float b);
};

