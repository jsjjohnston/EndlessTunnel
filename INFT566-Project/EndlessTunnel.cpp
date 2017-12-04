#include "EndlessTunnel.h"

using aie::Gizmos;

EndlessTunnel::EndlessTunnel(): speed(SCROLLSPEED_INITIAL)
{
	// initialize
	ring = new Ring();
	player = new Player();
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

	player->m_window = m_window;

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

	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		speed += TRANSITION_SPEED * deltaTime;
		if (speed >= SCROLLSPEED_MAX)
			speed = SCROLLSPEED_MAX;

		tunnel->setScrollSpeed(speed);
		ring->setSpeed(speed);
	}
	else
	{
		speed = SCROLLSPEED_INITIAL;

		tunnel->setScrollSpeed(speed);
		ring->setSpeed(speed);
	}

	ring->update(deltaTime);

	player->update(deltaTime);
}

void EndlessTunnel::draw()
{
	// wipe the screen to the background colour
	clearScreen();
		
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,	getWindowWidth() / (float)getWindowHeight(),0.1f, 1000.f);
	
	tunnel->draw();
	ring->draw();
	player->draw();

	// draw 3D gizmos
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}