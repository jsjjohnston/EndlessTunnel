#pragma once
#include "Application.h"
#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"

#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

#include "Tunnel.h"
#include "Ring.h"
#include "Player.h"

class EndlessTunnel :
	public Application
{
public:
	EndlessTunnel();
	~EndlessTunnel();

	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

private:
	float speed;

	Tunnel* tunnel;
	Ring* ring;
	Player* player;
};

