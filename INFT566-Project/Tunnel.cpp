#include "Tunnel.h"

using std::ifstream;
using std::ios;

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

Tunnel::Tunnel(float a_width, float a_height): checkerBoardSize(CHECKERBOARD_INITIAL), scrollSpeed(SCROLLSPEED_INITIAL),rotateSpeed(ROTATESPEED_INITIAL),geometry(GEOMETRY_INITIAL), horizon(HORIZON_INITIAL), windowWidth(a_width), windowHeight(a_height), demoMode(DEMOMODE)
{
	//transition = Transitions::SHRINK_GEOMETRY;
	transition = Transitions::SHRINK_CHECKERBOARDSIZE;
}


Tunnel::~Tunnel()
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

bool Tunnel::startup()
{
	// Setup Time Manager singleton
	TimeManager::create();
	if (demoMode)
		timer = glfwGetTime() + TRANSITION_FREQUENCY;

	compileShader("endless.vert");
	compileShader("endless.frag");
	link();
	validate();
	use();

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

	return true;
}

void Tunnel::shutdown()
{
	Gizmos::destroy();
}

void Tunnel::update(float deltaTime)
{
	if (demoMode && timer <= glfwGetTime())
	{
		switch (transition)
		{
		case SHRINK_GEOMETRY:
			shrinkGeometry(deltaTime);
			break;
		case RESET_GEOMETRY:
			resetGeometry(deltaTime);
			break;
		case EXPAND_GEOMETRY:
			expandGeometry(deltaTime);
			break;
		case EXPAND_CHECKERBOARDSIZE:
			expandCheckerBoardSize(deltaTime);
			break;
		case SHRINK_CHECKERBOARDSIZE:
			shrinkCheckerBoardSize(deltaTime);
			break;
		case RESET_CHECKERBOARDSIZE:
			resetCheckerBoardSize(deltaTime);
			break;
		case TRANSITION_COUNT:
			break;
		default:
			break;
		}
	}

	updateTimeUniform();
	updateResolutionUniform();
	updateCheckerBoardUniform();
	updateScrollSpeedUniform();
	updateRotateSpeedUniform();
	updateGeometryUniform();
	updateHorizonUniform();

}

void Tunnel::draw()
{
	// Draw Tunnel
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Tunnel::updateTimeUniform()
{
	// Pass in Time 
	int fragTimeLocation = glGetUniformLocation(handle, "time");
	glUniform1f(fragTimeLocation, glfwGetTime());
}

void Tunnel::updateResolutionUniform()
{
	// Pass in resolution
	int fragResolutionLocation = glGetUniformLocation(handle, "resolution");
	glUniform2f(fragResolutionLocation, windowWidth, windowHeight);
}

void Tunnel::updateCheckerBoardUniform()
{
	// Pass in checkerBoardSize
	int fragCheckboxSizeLocation = glGetUniformLocation(handle, "checkboxSize");
	glUniform1f(fragCheckboxSizeLocation, checkerBoardSize);
}

void Tunnel::updateScrollSpeedUniform()
{
	// Pass in modifier
	int fragScrollSpeedLocation = glGetUniformLocation(handle, "scrollSpeed");
	glUniform1f(fragScrollSpeedLocation, scrollSpeed);
}

void Tunnel::updateRotateSpeedUniform()
{
	// Pass in rotateSpeed
	int fragScrollSpeedLocation = glGetUniformLocation(handle, "rotateSpeed");
	glUniform1f(fragScrollSpeedLocation, rotateSpeed);
}

void Tunnel::updateGeometryUniform()
{
	// Pass in geometry
	int fragGeometryLocation = glGetUniformLocation(handle, "geometry");
	glUniform1f(fragGeometryLocation, geometry);
}

void Tunnel::updateHorizonUniform()
{
	// Pass in horizon
	int fragHorizonLocation = glGetUniformLocation(handle, "horizon");
	glUniform1f(fragHorizonLocation, horizon);
}

void Tunnel::compileShader(const char * fileName)
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

void Tunnel::compileShader(const char * filename, GLSLShaderType type)
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

void Tunnel::compileShader(const char * source, GLSLShaderType type, const char * filename)
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
		std::cout << "Shader " << filename << " complied" << std::endl;
	}
}

void Tunnel::link()
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

void Tunnel::validate()
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

void Tunnel::use()
{
	glUseProgram(handle);
	std::cout << "Using Handle: " << handle << std::endl;
}

std::string Tunnel::getExtension(const char * name)
{
	std::string nameStr(name);

	unsigned int location = nameStr.find_last_of('.');

	if (location != std::string::npos)
	{
		return nameStr.substr(location, std::string::npos);
	}

	return "";
}

bool Tunnel::fileExists(const std::string & a_fileName)
{
	struct stat buff; //System struct http://codewiki.wikidot.com/c:struct-stat 
	if (stat(a_fileName.c_str(), &buff) != -1)
	{
		return true;
	}
	return false;
}

void Tunnel::shrinkGeometry(float deltaTime)
{
	geometry -= TRANSITION_SPEED * deltaTime;
	if (geometry <= GEOMETRY_MIN)
	{
		timer = glfwGetTime() + TRANSITION_FREQUENCY;
		transition = Transitions::RESET_GEOMETRY;
	}
}

void Tunnel::resetGeometry(float deltaTime)
{
	if (geometry < GEOMETRY_INITIAL)
	{
		geometry += TRANSITION_SPEED * deltaTime;
		if (geometry >= GEOMETRY_INITIAL)
		{
			timer = glfwGetTime() + TRANSITION_FREQUENCY;
			transition = Transitions::EXPAND_GEOMETRY;
			return;
		}
	}
	else if (geometry > GEOMETRY_INITIAL)
	{
		geometry -= TRANSITION_SPEED * deltaTime;
		if (geometry <= GEOMETRY_INITIAL)
		{
			timer = glfwGetTime() + TRANSITION_FREQUENCY;
			transition = Transitions::SHRINK_CHECKERBOARDSIZE;
			return;
		}
	}
}

void Tunnel::expandGeometry(float deltaTime)
{
	geometry += TRANSITION_SPEED * deltaTime;
	if (geometry >= GEOMETRY_MAX)
	{
		timer = glfwGetTime() + TRANSITION_FREQUENCY;
		transition = Transitions::RESET_GEOMETRY;
	}
}

void Tunnel::shrinkCheckerBoardSize(float deltaTime)
{
	checkerBoardSize -= TRANSITION_SPEED * deltaTime;
	if (checkerBoardSize <= CHECKERBOARD_MIN)
	{
		timer = glfwGetTime() + TRANSITION_FREQUENCY;
		transition = Transitions::RESET_CHECKERBOARDSIZE;
	}
}

void Tunnel::resetCheckerBoardSize(float deltaTime)
{
	if (checkerBoardSize < CHECKERBOARD_INITIAL)
	{
		checkerBoardSize += TRANSITION_SPEED * deltaTime;
		if (checkerBoardSize >= CHECKERBOARD_INITIAL)
		{
			timer = glfwGetTime() + TRANSITION_FREQUENCY;
			transition = Transitions::SHRINK_GEOMETRY;
			return;
		}
	}
	else if (checkerBoardSize > CHECKERBOARD_INITIAL)
	{
		checkerBoardSize -= TRANSITION_SPEED * deltaTime;
		if (checkerBoardSize <= CHECKERBOARD_INITIAL)
		{
			timer = glfwGetTime() + TRANSITION_FREQUENCY;
			transition = Transitions::EXPAND_CHECKERBOARDSIZE;
			return;
		}
	}
}

void Tunnel::expandCheckerBoardSize(float deltaTime)
{
	checkerBoardSize += TRANSITION_SPEED * deltaTime;
	if (checkerBoardSize >= CHECKERBOARD_MAX)
	{
		timer = glfwGetTime() + TRANSITION_FREQUENCY;
		transition = Transitions::RESET_CHECKERBOARDSIZE;
	}
}
