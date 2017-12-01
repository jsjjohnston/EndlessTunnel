#pragma once
#include "Application.h"
#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "Gizmos.h"
//#include "Model.h"
//#include "Glslprogram.h"
//#include "Camera.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

enum GLSLShaderType {
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER,
	GEOMETRY = GL_GEOMETRY_SHADER,
	TESS_CONTROL = GL_TESS_CONTROL_SHADER,
	TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
	COMPUTE = GL_COMPUTE_SHADER
};

class EndlessTunnel :
	public Application
{
public:
	EndlessTunnel();
	~EndlessTunnel();

	// Shader code
	void compileShader(const char* fileName);
	void compileShader(const char* filename, GLSLShaderType type);
	void compileShader(const char* source, GLSLShaderType type, const char* filename);

	void link();
	void validate();
	void use();

	// Uniform functions
	void updateTimeUniform();
	void updateResolutionUniform();
	void updateCheckerBoardUniform(float deltaTime);
	void updateModifierUniform(float deltaTime);
	void updateScrollSpeedUniform(float deltaTime);
	void updateRotateSpeedUniform(float deltaTime);
	void updateGeometryUniform(float deltaTime);
	void updateHorizonUniform(float deltaTime);


	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

private:
	unsigned int VBO, VAO, EBO;
	
	float modifier, checkerBoardSize, scrollSpeed, rotateSpeed, geometry, horizon;

	int handle;
	std::string getExtension(const char* name);

	bool fileExists(const std::string& a_fileName);
};

