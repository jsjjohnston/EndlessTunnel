#pragma once
#include "Gizmos.h"
#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"

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
};

