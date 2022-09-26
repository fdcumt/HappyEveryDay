#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "HAL/Platform.h"
#include "Logging/LogMarcos.h"
#include "Logging/Log.h"


DEFINE_LOG_CATEGORY(LearnOpenGL);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *VertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* FragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";




int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	{ // get maximum number of vertex attribute
		int32 MaximumNumber = 0;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &MaximumNumber);
		DebugLog(LearnOpenGL, "MaximumNumber:%d", MaximumNumber);
	}

	// build and compile our shader program

	// vertex shader
	uint32 VertexShader = glCreateShader(GL_VERTEX_SHADER);

	// 将shader源码放入OpenGL管理的某个地方, 后续可以通过VertexShader直接使用
	glShaderSource(VertexShader, 1, &VertexShaderSource, nullptr);
	glCompileShader(VertexShader);

	int32 Succeed;
	char infoLog[512] = {0};

	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Succeed);

	if (!Succeed)
	{
		glGetShaderInfoLog(VertexShader, 512, nullptr, infoLog);
		ErrorLog(LearnOpenGL, "%s", infoLog);
		return -1;
	}

	// fragment shader
	uint32 FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, nullptr);
	glCompileShader(FragmentShader);

	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Succeed);

	if (!Succeed)
	{
		glGetShaderInfoLog(FragmentShader, 512, nullptr, infoLog);
		ErrorLog(LearnOpenGL, "%s", FragmentShader);
		return -1;
	}


	// link shader
	uint32 ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	// check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Succeed);
	if (!Succeed)
	{
		glGetProgramInfoLog(ShaderProgram, 512, nullptr, infoLog);
		ErrorLog(LearnOpenGL, "%s", FragmentShader);
		return -1;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);


	float Vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};

	uint32 VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO bind current VBO, then the VAO can store the VBO`s configure
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), nullptr);
	glEnableVertexAttribArray(0);

	// safe unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw out first triangle
		glUseProgram(ShaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(ShaderProgram);


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
