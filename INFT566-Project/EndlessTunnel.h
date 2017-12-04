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


class EndlessTunnel :
	public Application
{
public:
	EndlessTunnel();
	~EndlessTunnel();

	// Debug
	void updateXYZ(float deltaTime);

	float x, y, z;


	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

private:

	Tunnel* tunnel;
};

