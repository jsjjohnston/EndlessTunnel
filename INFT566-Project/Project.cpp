#include "Project.h"


Project::Project(): m_postIndex(0)
{
	m_model = new Model();
	phongProgram = new GLSLProgram();
	m_cam = new Camera();

	postSimple = new GLSLProgram();

	timePressed = 0;

	demoMode = false;
}


Project::~Project()
{
	delete postProgram;
	delete m_cam;
	delete phongProgram;

	postSimple;
}

bool Project::startup()
{
	// Setup Time Manager singleton
	TimeManager::create();
	
	// Set Background Colour
	setClearColour(1.0f,0.25f,0.25f);
	clearScreen();
	
	// Compile Phong Shaders
	phongProgram->compileShader("myShader.vert");
	phongProgram->compileShader("phong.frag");
	phongProgram->link();
	phongProgram->validate();
	phongProgram->use();
	
	// Compile Post Processing Shaders
	postSimple->compileShader("post.vert");
	postSimple->compileShader("postSimple.frag");
	postSimple->link();
	postSimple->validate();
	
	postProgram = postSimple;

	// Set up cam
	m_cam->setWindow(m_window);
	m_cam->setProgram(phongProgram);
	m_cam->setOriginalMousePos();
	
	// Setup Model
	m_model->loadModel("Chair.obj");
	m_model->setProgram(phongProgram);
	m_model->setCamera(m_cam);
	m_model->setWindow(m_window);
	m_model->setPostion(glm::vec3(0, -5, -15));
	m_model->loadDiffuseTexture("Wood Difuse.tga");

	// Setup fram Buffer
	setUpFrameBuffer();
	createFullScreenQuad();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Only render triangles facing the Cam

	return true;
}

void Project::shutdown()
{

}

void Project::update(float deltaTime)
{
	phongProgram->setUniform("lightPositionWorld", glm::vec3(0,6,-5)); // Light Direction
	phongProgram->setUniform("ambientLight", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

	postProgram->setUniform("time",deltaTime);

	//if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS && timePressed <= glfwGetTime())
	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS && timePressed <= glfwGetTime() && demoMode == false)
	{
		m_postIndex++;

		if (m_postIndex >= 0)
		{
			m_postIndex = 0;
		}

		switch (m_postIndex)
		{
		case 0:
			postProgram = postSimple;
			timePressed = glfwGetTime() + 1.0;
			break;

		default:
			break;
		}
	}
		
	m_cam->update(deltaTime);

	m_model->update(deltaTime);
}

void Project::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	
	// clear the target
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	clearScreen();
	
	//phongProgram->use();
	//m_model->draw();
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	
	clearScreen();

	postProgram->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_frameBufferObjectTexture);
	
	int loc = glGetUniformLocation(phongProgram->getHandle(), "target");
	glUniform1i(loc, 1);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	phongProgram->use();
}

void Project::setUpFrameBuffer()
{
	glGenFramebuffers(1, &m_frameBufferObject); //  Generate framebuffer object names
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject); // Bind a framebuffer to a framebuffer target

	// Craete a new Texture
	glGenTextures(1,&m_frameBufferObjectTexture); // Generate texture names
	glBindTexture(GL_TEXTURE_2D, m_frameBufferObjectTexture); // Bind a named texture to a texturing target
	
	/*
		Simultaneously specify storage for all levels of a two-dimensional or one-dimensional array texture
		https://www.khronos.org/opengl/wiki/GLAPI/glTexStorage2D
	*/
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, getWindowWidth(), getWindowHeight());
	
	/*
		Set texture parameters
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_frameBufferObjectTexture, 0);

	glGenRenderbuffers(1, &m_frameBufferObjectDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_frameBufferObjectDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, getWindowWidth(), getWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_frameBufferObjectDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Project::createFullScreenQuad()
{
	glm::vec2 halfTexel = 1.0f / glm::vec2(getWindowWidth(), getWindowHeight()) * 0.5f;

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
