#pragma once
#include "Application.h"
#include "GLM\glm.hpp"
#include "GLM\fwd.hpp"
#include <iostream>
#include <string>
#include <fstream>
//#include "Model.h"
//#include "Glslprogram.h"
//#include "Camera.h"

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

	void compileShader(const char* fileName);
	void compileShader(const char* filename, GLSLShaderType type);
	void compileShader(const char* source, GLSLShaderType type, const char* filename);

	virtual bool startup();
	virtual void shutdown();
	virtual void update(float deltaTime);
	virtual void draw();

private:
	unsigned int VBO, VAO, EBO;
	
	int handle;
	std::string getExtension(const char* name);

	bool fileExists(const std::string& a_fileName);
};

