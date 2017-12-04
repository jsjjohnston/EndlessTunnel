#include "Player.h"

using aie::Gizmos;

Player::Player(): x(0), y(0), z(0), speed(3)
{
}


Player::~Player()
{
}

bool Player::startup()
{
	return false;
}

void Player::shutdown()
{
}

void Player::update(float deltaTime)
{
	// y axis
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		y += speed * deltaTime;
	}

	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		y -= speed * deltaTime;
	}

	// z axis
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		z -= speed * deltaTime;
	}

	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		z += speed * deltaTime;
	}

	model = mat4(1);
	model[3] = { x, y, z, 1.0f };
}

void Player::draw()
{
	Gizmos::addSphere(glm::vec3(0, 0, 0), 2, 8, 8, glm::vec4(0.75, 0.75, 0.75, 0.50), &model);
}
