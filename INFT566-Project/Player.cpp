#include "Player.h"

using aie::Gizmos;

Player::Player()
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
}

void Player::draw()
{
	Gizmos::addSphere(glm::vec3(0, 0, 0), 1, 8, 8, glm::vec4(1, 0, 0, 0.75));
}
