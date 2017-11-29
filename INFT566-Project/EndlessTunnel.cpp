#include "EndlessTunnel.h"



EndlessTunnel::EndlessTunnel()
{
	m_endlessProgram = new GLSLProgram();
	m_camera = new Camera();
}


EndlessTunnel::~EndlessTunnel()
{
	delete m_endlessProgram;
	delete m_camera;
}

bool EndlessTunnel::startup()
{
	// Setup Time Manager singleton
	TimeManager::create();

	// Set Background Colour
	setClearColour(0.25f, 0.25f, 0.25f);
	clearScreen();

	// Setup Shader
	m_endlessProgram->compileShader("endless.vert");
	// Complile Fragment Shader here
	m_endlessProgram->link();
	m_endlessProgram->validate();
	m_endlessProgram->use();

	// Setup Camera
	m_camera->setWindow(m_window);
	m_camera->setProgram(m_endlessProgram);
	m_camera->setOriginalMousePos();

	return false;
}
