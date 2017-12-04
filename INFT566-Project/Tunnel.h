#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>

#include "GLM\glm.hpp"
#include "GLM\ext.hpp"
#include "GLM\fwd.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include "TimeManager.h"

#include "Gizmos.h"

#include "Config.h"

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
class Tunnel
{
public:
	// Constructor Destructor
	Tunnel(float a_width, float a_height);
	~Tunnel();

	// Game loop functions
	bool startup();
	void shutdown();
	void update(float deltaTime);
	void draw();

	// update uniform functions
	void updateTimeUniform();
	void updateResolutionUniform();
	void updateCheckerBoardUniform();
	void updateScrollSpeedUniform();
	void updateRotateSpeedUniform();
	void updateGeometryUniform();
	void updateHorizonUniform();

	// get & set uniform parameters
	float getCheckerBoardSize() { return checkerBoardSize; };

	void setCheckerBoardSize(float a_checkerBoardSize) { 
		if(a_checkerBoardSize <= CHECKERBOARD_MAX && a_checkerBoardSize >= CHECKERBOARD_MIN) 
			checkerBoardSize = a_checkerBoardSize; 
	};

	float getScrollSpeed() { return scrollSpeed; };
	
	void setScrollSpeed(float a_scrollSpeed) { 
		if (a_scrollSpeed <= SCROLLSPEED_MAX && a_scrollSpeed >= SCROLLSPEED_MIN)
			scrollSpeed = a_scrollSpeed; 
	};
	
	float getRotateSpeed() { return rotateSpeed; };
	
	void setRotateSpeed(float a_rotateSpeed) {
		if (a_rotateSpeed <= ROTATESPEED_MAX && a_rotateSpeed >= ROTATESPEED_MIN)
			rotateSpeed = a_rotateSpeed; 
	};

	float getGeomtry() { return geometry; };

	void setGeomtry(float a_geomtry) { 
		if (a_geomtry <= GEOMETRY_MAX && a_geomtry >= GEOMETRY_MIN) 
			geometry = a_geomtry; 
	};
	
	float getHorizon() { return geometry; };

	void setHorizon(float a_horizon) { 
		if (a_horizon <= HORIZON_MAX && a_horizon >= HORIZON_MIN)
			horizon = a_horizon; 
	};

	// get & set resolution
	float getWindowWidth() { return windowWidth; };
	void setWindowWidth(float a_width) { windowWidth = a_width; };
	float getWindowHeight() { return windowHeight; };
	void setWindowHeight(float a_height) { windowHeight = a_height; };

private:
	// Buffer Handles
	unsigned int VBO, VAO, EBO;

	// Shader functions
	void compileShader(const char* fileName);
	void compileShader(const char* filename, GLSLShaderType type);
	void compileShader(const char* source, GLSLShaderType type, const char* filename);

	void link();
	void validate();
	void use();

	// Shader Helper Functions
	std::string getExtension(const char* name);
	bool fileExists(const std::string& a_fileName);

	// uniform Variables
	float checkerBoardSize;
	float scrollSpeed; 
	float rotateSpeed; 
	float geometry; 
	float horizon;

	// shader Handle
	int handle;

	// resolution
	float windowWidth;
	float windowHeight;
};

