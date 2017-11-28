#pragma once
#include "Application.h"


class EndlessTunnel : public Application
{
public:
	EndlessTunnel();
	~EndlessTunnel();

	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();
};

