#pragma once
#include "Gizmos.h"
#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"
#include "GLFW\glfw3.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Player
{
public:
	Player();
	~Player();

	bool startup();
	void shutdown();
	void update(float deltaTime);
	void draw();

	GLFWwindow* m_window;

	float getSpeed() { return speed; };
	void setSpeed(float a_speed) { speed = a_speed; };

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

private:
	float x;
	float y;
	float z;

	float speed;

	glm::mat4 model;

};

