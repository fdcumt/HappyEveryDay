#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include "HAL/Platform.h"
#include "Logging/LogMarcos.h"
#include "Logging/Log.h"
#include "OperatorFiles/OperatorFile.h"
#include "OperatorFiles/Paths.h"
#include "Shader/Shader.h"
#include "StbImage/StbImage.h"


DEFINE_LOG_CATEGORY(LearnOpenGL);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *VertexShaderSource = nullptr;
const char* FragmentShaderSource = nullptr;

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
	FStdString VertexShaderFileName = FPaths::GetContentDir() + "/ShaderFiles/VertexShader.vs";
	FStdString FragmentShaderFileName = FPaths::GetContentDir() + "/ShaderFiles/FragmentShader.fs";
	FShader Shader(VertexShaderFileName, FragmentShaderFileName);


	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// safe unbind VBO and VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	FSTBImage::SetFlipVerticallyOnLoad(true);

	uint32 TextureID1;
	{
		glGenTextures(1, &TextureID1);
		glBindTexture(GL_TEXTURE_2D, TextureID1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		{
			int32 width, height, nrChannels;
			FStdString ImgFileName = FPaths::GetContentDir() + "Texture/container.jpg";

			uint8* pData = FSTBImage::StbiLoad(ImgFileName.data(), &width, &height, &nrChannels, 0);
			if (pData)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				ErrorLog(LearnOpenGL, "Load Image[%s] Failed", ImgFileName.data());
				return -1;
			}

			FSTBImage::StbiImageFree(pData);
		}
	}

	uint32 TextureID2;
	{
		glGenTextures(1, &TextureID2);
		glBindTexture(GL_TEXTURE_2D, TextureID2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		{
			int32 width, height, nrChannels;
			FStdString ImgFileName = FPaths::GetContentDir() + "Texture/awesomeface.png";

			uint8* pData = FSTBImage::StbiLoad(ImgFileName.data(), &width, &height, &nrChannels, 0);
			if (pData)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				ErrorLog(LearnOpenGL, "Load Image[%s] Failed", ImgFileName.data());
				return -1;
			}

			FSTBImage::StbiImageFree(pData);
		}
	}

	// draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 启用该shader
	Shader.UseProgram();
	// 将fs材质中uniform sampler2D OurTexture绑定到第0个texture上.
	Shader.SetInt("Texture1", 0);
	Shader.SetInt("Texture2", 1);

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

		// 把TextureID1和第0个texture绑定
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID1);

		// 把TextureID1和第0个texture绑定
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureID2);


		//float timeValue = glfwGetTime();
		//float greenValue = sin(timeValue);
		//
		//int vertexColorLocation = glGetUniformLocation(ShaderProgram, "OurColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.f));
		transform = glm::rotate(transform, float(glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));

		Shader.UseProgram();
		uint32 transformLoc = glGetUniformLocation(Shader.GetID(), "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		float timeValue = glfwGetTime();
		float MixValue = sin(timeValue);
		MixValue = MixValue<0.f ? 0.f:MixValue;
		Shader.SetFloat("MixValue", MixValue);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	Shader.DeleteProgram();


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
