#include "EndlessTunnel.h"
using std::ifstream;
using std::ios;
#include <sstream>

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

EndlessTunnel::EndlessTunnel(): handle(0)
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
	
	return true;
}

void EndlessTunnel::shutdown()
{
}

void EndlessTunnel::update(float deltaTime)
{
	// Pass in time as green uniform
	//float timeValue = glfwGetTime();
	//float greenValue = sin(timeValue) / 2.0f + 0.5f;
	//int vertexColorLocation = glGetUniformLocation(handle, "ourColor");
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

	// Pass in Time
	int fragTimeLocation = glGetUniformLocation(handle, "time");
	glUniform1f(fragTimeLocation, glfwGetTime());

	// Pass in resolution
	int fragResolutionLocation = glGetUniformLocation(handle, "resolution");
	glUniform2f(fragResolutionLocation, getWindowWidth(), getWindowHeight());
}

void EndlessTunnel::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
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