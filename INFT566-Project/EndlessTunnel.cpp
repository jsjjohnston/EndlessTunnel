#include "EndlessTunnel.h"
using std::ifstream;
using std::ios;
#include <sstream>

using aie::Gizmos;

struct shader_file_extension {
	const char *ext;
	GLSLShaderType type;
};

struct shader_file_extension extensions[] =
{
	{ ".vs", GLSLShaderType::VERTEX },
	{ ".vert", GLSLShaderType::VERTEX },
	{ ".gs", GLSLShaderType::GEOMETRY },
	{ ".geom", GLSLShaderType::GEOMETRY },
	{ ".tcs", GLSLShaderType::TESS_CONTROL },
	{ ".tes", GLSLShaderType::TESS_EVALUATION },
	{ ".fs", GLSLShaderType::FRAGMENT },
	{ ".frag", GLSLShaderType::FRAGMENT },
	{ ".cs", GLSLShaderType::COMPUTE }
};

EndlessTunnel::EndlessTunnel(): handle(0), modifier(-300.0f), checkerBoardSize(4.6), scrollSpeed(1.0), rotateSpeed(0.0), geometry(2.0), horizon(1.0)
{

}


EndlessTunnel::~EndlessTunnel()
{
	if (handle == 0) return;

	// Query the number of attached shaders
	int numShaders = 0;
	glGetProgramiv(handle, GL_ATTACHED_SHADERS, &numShaders);

	// Get the shader names
	GLuint * shaderNames = new GLuint[numShaders];
	glGetAttachedShaders(handle, numShaders, NULL, shaderNames);

	// Delete the shaders
	for (int i = 0; i < numShaders; i++)
		glDeleteShader(shaderNames[i]);

	// Delete the program
	glDeleteProgram(handle);

	delete[] shaderNames;
}

bool EndlessTunnel::startup()
{
	// Setup Time Manager singleton
	TimeManager::create();

	compileShader("endless.vert");
	compileShader("endless.frag");
	link();
	validate();
	use();
	
	/*
	Single Triangle

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
	*/

	float vertices[] = {
		1.0f,  1.0f, 0.0f,  // top right
		1.0f, -1.0f, 0.0f,  // bottom right
		-1.0f, -1.0f, 0.0f,  // bottom left
		-1.0f,  1.0f, 0.0f   // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,	getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);

	return true;
}

void EndlessTunnel::shutdown()
{
	Gizmos::destroy();
}

void EndlessTunnel::update(float deltaTime)
{
	updateTimeUniform();

	updateResolutionUniform();

	updateCheckerBoardUniform(deltaTime);

	updateModifierUniform(deltaTime);

	updateScrollSpeedUniform(deltaTime);

	updateRotateSpeedUniform(deltaTime);

	updateGeometryUniform(deltaTime);

	updateHorizonUniform(deltaTime);

	Gizmos::clear();

	// ADD in transform
	Gizmos::addTransform(mat4(1));

	static float x = 0, y = 0, z = 0;

	mat4 t(1);
	t[3] = vec4(x, y, z, 1);

	Gizmos::addSphere(vec3(0, 0, 0), 1, 8, 8, vec4(1, 0, 0, 0.75f));

	Gizmos::addSphere(vec3(0, 0, 0), 0.10, 8, 8, vec4(0, 0.5, 0.5, 1.0f),&t);

	m_viewMatrix = glm::lookAt(vec3(30, 1, 1),	vec3(0), vec3(0, 1, 0));
}

void EndlessTunnel::draw()
{
	// wipe the screen to the background colour
	clearScreen();
	
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,	getWindowWidth() / (float)getWindowHeight(),0.1f, 1000.f);
	
	// Draw Tunnel
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	// draw 3D gizmos
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void EndlessTunnel::compileShader(const char * fileName)
{
	int numExts = sizeof(extensions) / sizeof(shader_file_extension);

	std::string ext = getExtension(fileName);
	GLSLShaderType type;
	bool matchFound = false;
	for (int i = 0; i < numExts; i++)
	{
		if (ext == extensions[i].ext)
		{
			matchFound = true;
			type = extensions[i].type;
			break;
		}
	}

	if (!matchFound)
	{
		std::cout << "COMPLIE SHADER ERRROR: Shader File extention not found: " << fileName << std::endl;
	}
	else
	{
		compileShader(fileName, type);
	}
}

void EndlessTunnel::compileShader(const char * filename, GLSLShaderType type)
{
	// If File Doesnt Exist
	if (!fileExists(filename))
	{
		std::cout << "COMPLIE SHADER ERRROR : Shader File not found: " << filename << std::endl;
		return; //Dont crash
	}

	if (handle <= 0)
	{
		handle = glCreateProgram();
		if (handle == 0)
		{
			std::cout << "COMPLIE SHADER ERRROR: Unable to create shader program: " << filename << std::endl;
			return; //Dont crash
		}
		else
		{
			std::cout << "Handle Set to: " << handle << std::endl;
		}
	}

	std::ifstream inFile(filename, std::ios::in);
	if (!inFile) {
		std::cout << "COMPLIE SHADER ERRROR: Unable to open: " << filename << std::endl;
		return; //Dont Crash
	}
	
	// Get file contents
	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	
	std::string s_code = code.str();
	const char * c_code = s_code.c_str();

	compileShader(c_code, type, filename);
}

void EndlessTunnel::compileShader(const char * source, GLSLShaderType type, const char * filename)
{
	if (handle <= 0)
	{
		handle = glCreateProgram();
		if (handle == 0)
		{
			std::cout << "COMPLIE SHADER ERRROR: Unable to create shader program: " << filename << std::endl;
			return; //Dont crash
		}
		else
		{
			std::cout << "Handle Set to: " << handle << std::endl;
		}
	}

	// Get Handle for shader
	unsigned int shaderHandle = glCreateShader(type);

	// Assign Shader source to handle
	glShaderSource(shaderHandle, 1, (const char**)&source, 0);

	// Complie Shader
	glCompileShader(shaderHandle);

	// Check for Shader Complie Errors
	int result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);

	// If complie failed, display errors
	if (GL_FALSE == result)
	{
		//printf("%c - %c shader compilation failed!\n", stderr, type);

		std::cout << "shader compilation failed!: " << std::endl;

		int logLen;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char* log = new char[logLen];

			int written;
			glGetShaderInfoLog(shaderHandle, logLen, &written, log);

			std::cout << "Shader log: " << log << std::endl;
			delete[] log;
		}

		if (filename)
		{
			std::cout << "Shader compilation failed!: " << filename << std::endl;
		}
		else
		{
			std::cout << "Shader compilation failed!" << std::endl;
		}

		return;
	}
	else
	{
		glAttachShader(handle, shaderHandle);
		std::cout << "Shader " << filename << " complied"<< std::endl;
	}
}

std::string EndlessTunnel::getExtension(const char * name)
{
	std::string nameStr(name);

	unsigned int location = nameStr.find_last_of('.');

	if (location != std::string::npos)
	{
		return nameStr.substr(location, std::string::npos);
	}

	return "";
}

bool EndlessTunnel::fileExists(const std::string & a_fileName)
{
	struct stat buff; //System struct http://codewiki.wikidot.com/c:struct-stat 
	if (stat(a_fileName.c_str(), &buff) != -1)
	{
		return true;
	}
	return false;
}

void EndlessTunnel::link()
{
	glLinkProgram(handle);

	int status = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		int length = 0;
		std::string logString;

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0)
		{
			char* c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log);
			logString = c_log;
			delete[] c_log;
		}
		std::cout << "Program link failed:" << std::endl << logString << std::endl;
		return;
	}
	else
	{
		std::cout << "Handle: " << handle << " Linked Successfully" << std::endl;
	}
	
}

void EndlessTunnel::validate()
{
	int status;
	glValidateProgram(handle);
	glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status)
	{
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length = 0)
		{
			char* c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log);
			logString = c_log;
			delete[] c_log;
		}

		std::cout << "Program Failed to validate" << std::endl;
	}
	else
	{
		std::cout << "Handle: " << handle << " Validated successfully" << std::endl;
	}
}

void EndlessTunnel::use()
{
	glUseProgram(handle);
	std::cout << "Using Handle: " << handle << std::endl;
}

void EndlessTunnel::updateTimeUniform()
{
	// Pass in Time
	int fragTimeLocation = glGetUniformLocation(handle, "time");
	glUniform1f(fragTimeLocation, glfwGetTime());
}

void EndlessTunnel::updateResolutionUniform()
{
	// Pass in resolution
	int fragResolutionLocation = glGetUniformLocation(handle, "resolution");
	glUniform2f(fragResolutionLocation, getWindowWidth(), getWindowHeight());
}

void EndlessTunnel::updateCheckerBoardUniform(float deltaTime)
{
	// checkerBoardSize
	if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		checkerBoardSize += 1.0f * deltaTime;
		std::cout << "Checkbox Size Value: " << checkerBoardSize << std::endl;
	}

	if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		checkerBoardSize -= 1.0f * deltaTime;
		std::cout << "Checkbox Size Value: " << checkerBoardSize << std::endl;
	}

	// Pass in checkerBoardSize
	int fragCheckboxSizeLocation = glGetUniformLocation(handle, "checkboxSize");
	glUniform1f(fragCheckboxSizeLocation, checkerBoardSize);
}

void EndlessTunnel::updateModifierUniform(float deltaTime)
{
	// modifier
	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		modifier += 1.0f * deltaTime;
		std::cout << "Modifier Size Value: " << modifier << std::endl;
	}

	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		modifier -= 1.0f * deltaTime;
		std::cout << "Modifier Size Value: " << modifier << std::endl;
	}

	// Pass in modifier
	int fragModifierLocation = glGetUniformLocation(handle, "modifier");
	glUniform1f(fragModifierLocation, modifier);
}

void EndlessTunnel::updateScrollSpeedUniform(float deltaTime)
{
	// scrollSpeed
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		scrollSpeed += 1.0f * deltaTime;
		std::cout << "Scroll Speed Value: " << scrollSpeed << std::endl;
	}

	// scrollSpeed
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		scrollSpeed -= 1.0f * deltaTime;
		std::cout << "Scroll Speed Value: " << scrollSpeed << std::endl;
	}

	// Pass in modifier
	int fragScrollSpeedLocation = glGetUniformLocation(handle, "scrollSpeed");
	glUniform1f(fragScrollSpeedLocation, scrollSpeed);
}

void EndlessTunnel::updateRotateSpeedUniform(float deltaTime)
{
	// rotateSpeed
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		rotateSpeed += 1.0f * deltaTime;
		std::cout << "Rotate Speed Value: " << rotateSpeed << std::endl;
	}

	// rotateSpeed
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		rotateSpeed -= 1.0f * deltaTime;
		std::cout << "Rotate Speed Value: " << rotateSpeed << std::endl;
	}

	// Pass in rotateSpeed
	int fragScrollSpeedLocation = glGetUniformLocation(handle, "rotateSpeed");
	glUniform1f(fragScrollSpeedLocation, rotateSpeed);
}

void EndlessTunnel::updateGeometryUniform(float deltaTime)
{
	// geometry
	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		geometry += 1.0f * deltaTime;
		std::cout << "Geometry Value: " << geometry << std::endl;
	}

	// rotateSpeed
	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		geometry -= 1.0f * deltaTime;
		std::cout << "Geometry Value: " << geometry << std::endl;
	}

	// Pass in rotateSpeed
	int fragGeometryLocation = glGetUniformLocation(handle, "geometry");
	glUniform1f(fragGeometryLocation, geometry);
}

void EndlessTunnel::updateHorizonUniform(float deltaTime)
{
	// horizon
	if (glfwGetKey(m_window, GLFW_KEY_H) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		horizon += 1.0f * deltaTime;
		std::cout << "Horizon Value: " << horizon << std::endl;
	}

	// horizon
	if (glfwGetKey(m_window, GLFW_KEY_H) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		horizon -= 1.0f * deltaTime;
		std::cout << "Geometry Value: " << horizon << std::endl;
	}

	// Pass in horizon
	int fragHorizonLocation = glGetUniformLocation(handle, "horizon");
	glUniform1f(fragHorizonLocation, horizon);
}
