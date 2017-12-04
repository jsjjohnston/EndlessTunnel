#include "EndlessTunnel.h"

using aie::Gizmos;

EndlessTunnel::EndlessTunnel()
{
	// initialize
	ring = new Ring();
}


EndlessTunnel::~EndlessTunnel()
{
	delete tunnel;
	delete ring;
}

bool EndlessTunnel::startup()
{
	// initialize
	tunnel = new Tunnel(getWindowWidth(), getWindowHeight());

	tunnel->startup();
	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(30, 1, 1), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);
	
	ring->setSpeed(tunnel->getScrollSpeed());

	return true;
}

void EndlessTunnel::shutdown()
{
	Gizmos::destroy();
}

void EndlessTunnel::update(float deltaTime)
{
	Gizmos::clear();
		
	tunnel->update(deltaTime);

	ring->update(deltaTime);

	//m_viewMatrix = glm::lookAt(vec3(30, 1, 1),	vec3(0), vec3(0, 1, 0));
}

void EndlessTunnel::draw()
{
	// wipe the screen to the background colour
	clearScreen();
		
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,	getWindowWidth() / (float)getWindowHeight(),0.1f, 1000.f);
	
	tunnel->draw();
	ring->draw();
	
	// draw 3D gizmos
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}