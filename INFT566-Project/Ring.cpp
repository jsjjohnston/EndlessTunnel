#include "Ring.h"

using aie::Gizmos;

Ring::Ring() : speed(0), x(-500), y(0), z(0)
{
	randomType();
	setColour();
}


Ring::~Ring()
{

}

bool Ring::startup()
{
	return true;
}

void Ring::shutdown()
{
}

void Ring::update(float deltaTime)
{
	model = mat4(1);
	x += speed / 2;
	model[3] = { x, y, z, 1.0f };
	if (x >= 100) spawn();

	mat4 rX = glm::rotate(1.5f, vec3(1, 0, 0));
	mat4 rY = glm::rotate(0.0f, vec3(0, 1, 0));
	mat4 rZ = glm::rotate(1.5f, vec3(0, 0, 1));
	
	model *= rX * rY * rZ;
}

void Ring::draw()
{
	Gizmos::addRing(vec3(0, 0, 0), 1, 1.5f, 8, colour, &model);
}

void Ring::spawn()
{
	randomType();
	setColour();

	x = -500;
	y = RandomFloat(-10.0f,10.0f);
	z = RandomFloat(-10.0f,10.0f);
}


void Ring::randomType()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(RingType::RED, RingType::BLUE);

	type = (RingType) dis(gen);

	std::cout << "Type Set to " << type << std::endl;
}

void Ring::setColour()
{
	switch (type)
	{
	case RED:
		colour = vec4(1, 0, 0, 1);
		break;
	case GREEN:
		colour = vec4(0, 1, 0, 1);
		break;
	case BLUE:
		colour = vec4(0, 0, 1, 1);
		break;
	default:
		break;
	}
}

float Ring::RandomFloat(float a, float b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(a, b);

	float rand = (float)dis(gen);

	std::cout << "Random: " << rand << std::endl;

	return (float)dis(gen);
}


