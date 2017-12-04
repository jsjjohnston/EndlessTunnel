#include "EndlessTunnel.h"

using aie::Gizmos;

EndlessTunnel::EndlessTunnel()
{
	// Debug
	x = -200;
	y = 0;
	z = 0;
}


EndlessTunnel::~EndlessTunnel()
{
	delete tunnel;
}

bool EndlessTunnel::startup()
{
	// initialize
	tunnel = new Tunnel(getWindowWidth(), getWindowHeight());

	tunnel->startup();
	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);

	return true;
}

void EndlessTunnel::shutdown()
{
	Gizmos::destroy();
}

void EndlessTunnel::update(float deltaTime)
{
	// Debug
	updateXYZ(deltaTime);

	Gizmos::clear();
	
	tunnel->update(deltaTime);

	// ADD in transform
	Gizmos::addTransform(mat4(1));

	x += tunnel->getScrollSpeed() / 2;
	if (x == 100) x = -200;

	mat4 model(1);
	//model[3] = { 0, 0, 0, 1.0f };
	model[3] = { x, y, z, 1.0f };

	mat4 rX = glm::rotate(1.5f, vec3(1, 0, 0));
	mat4 rY = glm::rotate(0.0f, vec3(0, 1, 0));
	mat4 rZ = glm::rotate(1.5f, vec3(0, 0, 1));
	
	model *= rX * rY * rZ;

	Gizmos::addRing(vec3(0, 0, 0), 1, 1.5f, 8, vec4(1, 1, 0, 1), &model);
	//Gizmos::addSphere(vec3(0, 0, 0), 1, 8, 8, vec4(1, 0, 0, 0.75f));

	//Gizmos::addSphere(vec3(0, 0, 0), 0.10, 8, 8, vec4(0, 0.5, 0.5, 1.0f),&t);

	m_viewMatrix = glm::lookAt(vec3(30, 1, 1),	vec3(0), vec3(0, 1, 0));
}

void EndlessTunnel::draw()
{
	// wipe the screen to the background colour
	clearScreen();
		
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,	getWindowWidth() / (float)getWindowHeight(),0.1f, 1000.f);
	
	tunnel->draw();
	
	// draw 3D gizmos
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}


void EndlessTunnel::updateXYZ(float deltaTime)
{
	// X
	if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x += 1.0f * deltaTime;
		std::cout << "X Value: " << x << std::endl;
	}

	// horizon
	if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x -= 1.0f * deltaTime;
		std::cout << "X Value: " << x << std::endl;
	}


	// Y
	if (glfwGetKey(m_window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		y += 1.0f * deltaTime;
		std::cout << "Y Value: " << y<< std::endl;
	}

	// horizon
	if (glfwGetKey(m_window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		y -= 1.0f * deltaTime;
		std::cout << "Y Value: " << y << std::endl;
	}

	// Z
	if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		z += 1.0f * deltaTime;
		std::cout << "Z Value: " << z << std::endl;
	}

	// horizon
	if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		z -= 1.0f * deltaTime;
		std::cout << "Z Value: " << z << std::endl;
	}

}
