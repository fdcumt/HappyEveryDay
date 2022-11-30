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
#include "Camera/Camera.h"
#include "Message/MessageDefine.h"
#include "Message/MessageBase.h"


DEFINE_LOG_CATEGORY(LearnOpenGL);


FCamera Camera;

float MoseMoveXSpeedFactor = 5;
float MoseMoveYSpeedFactor = 5;

bool bMousePosInit = false;
float MousePosX = 0.f;
float MousePosY = 0.f;

float MouseXDeltaMove = 0.f;
float MouseYDeltaMove = 0.f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void ProcessKeyEvent(GLFWwindow* window, int key, int action, int mods)
{
	NS_Message::EInputEvent InputType = NS_Message::IE_Released;
	NS_Message::EMesssageType MessageType = NS_Message::E_MessageType_None;

	switch (action)
	{
	case GLFW_RELEASE:
	{
		InputType = NS_Message::IE_Released;
		break;
	}
	case GLFW_PRESS:
	{
		InputType = NS_Message::IE_Pressed;
		break;
	}
	case GLFW_REPEAT:
	{
		InputType = NS_Message::IE_Repeat;
		break;
	}
	}

	switch (key)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		MessageType = NS_Message::E_MouseButton_Left;
		break;
	}
	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		MessageType = NS_Message::E_MouseButton_Right;
		break;
	}
	case GLFW_KEY_A:
	{
		MessageType = NS_Message::E_Keyboard_A;
		break;
	}
	case GLFW_KEY_S:
	{
		MessageType = NS_Message::E_Keyboard_S;
		break;
	}
	case GLFW_KEY_D:
	{
		MessageType = NS_Message::E_Keyboard_D;
		break;
	}
	case GLFW_KEY_W:
	{
		MessageType = NS_Message::E_Keyboard_W;
		break;
	}
	}

	if (MessageType != NS_Message::E_MessageType_None)
	{
		NS_Message::MessageInfoList[MessageType].InputEvent = InputType;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ProcessKeyEvent(window, button, action, mods);
}

void cb_key(GLFWwindow* win, int key, int code, int action, int mods) 
{
	ProcessKeyEvent(win, key, action, mods);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (bMousePosInit)
	{
		MouseXDeltaMove = MousePosX - xpos;
		MouseYDeltaMove = MousePosY - ypos;
		DebugLog(LearnOpenGL, "MouseYDeltaMove:%f", MouseYDeltaMove);
	}
	else
	{
		bMousePosInit = true;
	}

	MousePosX = xpos;
	MousePosY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		// The cursor entered the content area of the window
	}
	else
	{
		// The cursor left the content area of the window
	}
}

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

	glfwSetKeyCallback(window, cb_key);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);

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


	// build and compile our shader program
	FStdString LightVertexShaderFileName = FPaths::GetContentDir() + "/ShaderFiles/LightVertexShader.vs";
	FStdString LightFragmentShaderFileName = FPaths::GetContentDir() + "/ShaderFiles/LightFragmentShader.fs";
	FShader LightShader(LightVertexShaderFileName, LightFragmentShaderFileName);

	// build and compile our shader program
	FStdString ObjectVSFileName= FPaths::GetContentDir() + "/ShaderFiles/ObjectVertexShader.vs";
	FStdString ObjectFSFileName = FPaths::GetContentDir() + "/ShaderFiles/ObjectFragmentShader.fs";
	FShader ObjectShader(ObjectVSFileName, ObjectFSFileName);

// 	float vertices[] = {
// 		// positions          // texture coordinate
// 		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
// 		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
// 		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
// 		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
// 	};

	float vertices[] = {
    // positions          // texture coordinates
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glm::vec3 CubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	unsigned int VBO, VAO, EBO;
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	unsigned int LightVAO;
	{
		glGenVertexArrays(1, &LightVAO);
		glBindVertexArray(LightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	unsigned int ObjectVAO;
	{
		glGenVertexArrays(1, &ObjectVAO);
		glBindVertexArray(ObjectVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}


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

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);

	{ // init camera
		Camera.Init(glm::vec3(0.f, 0.f, 3.f), 0.f, 0.f, 45.f, 0.1f, 1000.f);
	}

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// update logic
		Camera.DoMovement(NS_Message::MessageInfoList, MouseXDeltaMove, MouseYDeltaMove);
		MouseXDeltaMove = 0.f;
		MouseYDeltaMove = 0.f;

		// input
		// -----
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// 清除color buffer 和 depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 把TextureID1和第0个texture绑定
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID1);

		// 把TextureID1和第0个texture绑定
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureID2);

		// active shader
		Shader.UseProgram();

		//float timeValue = glfwGetTime();
		//float greenValue = sin(timeValue);
		//
		//int vertexColorLocation = glGetUniformLocation(ShaderProgram, "OurColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// 设置变换矩阵
		//glm::mat4 ViewMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

		Camera.RecalculateViewMatrix();

		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
		ProjectionMatrix = glm::perspective(glm::radians(Camera.GetFov()), (float)SCR_WIDTH /(float)SCR_HEIGHT, Camera.GetNearPlane(), Camera.GetFarPlane());

		Shader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
		//Shader.SetMaterix4fv("View", glm::value_ptr(ViewMatrix));
		Shader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));

// 		{ // for normal object
// 			glBindVertexArray(VAO);
// 
// 			for (const glm::vec3& ItemPosition : CubePositions)
// 			{
// 				glm::mat4 ModelMatrix = glm::mat4(1.0f);
// 				ModelMatrix = glm::translate(ModelMatrix, ItemPosition);
// 				ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));
// 
// 				Shader.SetMaterix4fv("Model", glm::value_ptr(ModelMatrix));
// 
// 				//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// 				glDrawArrays(GL_TRIANGLES, 0, 36);
// 			}
// 		}

		{ // for light
			LightShader.UseProgram();
			// don't forget to use the corresponding shader program first (to set the uniform)
			//LightShader.SetVector("objectColor", 1.0f, 0.5f, 0.31f);
			LightShader.SetVector("lightColor", 1.0f, 1.0f, 1.0f);

			LightShader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
			LightShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));

			glBindVertexArray(LightVAO);

			glm::mat4 ModelMatrix = glm::mat4(1.0f);
			glm::vec3 lightPos(1.2f, 1.0f, -2.0f);
			ModelMatrix = glm::translate(ModelMatrix, lightPos);
			ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));

			LightShader.SetMaterix4fv("Model", glm::value_ptr(ModelMatrix));

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		{ // for Object
			ObjectShader.UseProgram();
			// don't forget to use the corresponding shader program first (to set the uniform)
			ObjectShader.SetVector("objectColor", 1.0f, 0.5f, 0.31f);
			ObjectShader.SetVector("lightColor", 1.0f, 1.0f, 1.0f);

			ObjectShader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
			ObjectShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));

			glBindVertexArray(LightVAO);

			glm::mat4 ModelMatrix = glm::mat4(1.0f);
			glm::vec3 ObjectPos(1.2f, 1.0f, -7.0f);
			ModelMatrix = glm::translate(ModelMatrix, ObjectPos);
			ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));

			ObjectShader.SetMaterix4fv("Model", glm::value_ptr(ModelMatrix));

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &LightVAO);
	glDeleteVertexArrays(1, &ObjectVAO);
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
