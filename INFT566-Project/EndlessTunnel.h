#pragma once
#include "Application.h"
#include "GLM\glm.hpp"
#include "GLM\fwd.hpp"
#include "Model.h"
#include "Glslprogram.h"
#include "Camera.h"


class EndlessTunnel : public Application
{
public:
	EndlessTunnel();
	~EndlessTunnel();

	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();

private:
	GLSLProgram* endless;
	Camera* m_cam;
};

