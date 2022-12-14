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
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"


DEFINE_LOG_CATEGORY(LearnOpenGL);

#define OFFSET(Struct, Member) (&((Struct*)0)->Member)


FCamera Camera;

float MoseMoveXSpeedFactor = 5;
float MoseMoveYSpeedFactor = 5;

bool bMousePosInit = false;
float MousePosX = 0.f;
float MousePosY = 0.f;

float MouseXDeltaMove = 0.f;
float MouseYDeltaMove = 0.f;


uint32 LoadTexture(const FStdString& InTexturePath)
{
	int32 width, height, nrChannels;
	uint8* pData = FSTBImage::StbiLoad(InTexturePath.data(), &width, &height, &nrChannels, 0);
	if (pData)
	{
		GLenum eFormat;
		switch (nrChannels)
		{
		case 1:
		{
			eFormat = GL_RED;
			break;
		}
		case 3:
		{
			eFormat = GL_RGB;
			break;
		}
		case 4:
		{
			eFormat = GL_RGBA;
			break;
		}
		default:
		{
			ErrorLog(LearnOpenGL, "LoadTexture[%s] error, nrChannels[%d].", InTexturePath.data(), nrChannels);
			FSTBImage::StbiImageFree(pData);
			return -1;
		}
		}

		uint32 TextureID = -1;
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		glTexImage2D(GL_TEXTURE_2D, 0, eFormat, width, height, 0, eFormat, GL_UNSIGNED_BYTE, pData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FSTBImage::StbiImageFree(pData);

		return TextureID;
	}
	else
	{
		ErrorLog(LearnOpenGL, "Load Image[%s] Failed", InTexturePath.data());
		return -1;
	}
}

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
		//DebugLog(LearnOpenGL, "MouseYDeltaMove:%f", MouseYDeltaMove);
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


	struct FVerticeInfo
	{
		FVector Position;
		FVector2D TextureCoord;
		FVector Normal;
	};

	float vertices[] = {
		// positions          // texture coordinates    // normal
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,               0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,               0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,               0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,               0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,               0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,               0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,               0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,               0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,               0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,               0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,               0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,               0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,               1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,               1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,               0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,               0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,               0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,               0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,               0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,               0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,               0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,               0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,               0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,               0.0f,  1.0f,  0.0f
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	// positions all containers
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

	// positions of the point lights
	glm::vec3 PointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, Position));
		glEnableVertexAttribArray(0);

		// texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, TextureCoord));
		glEnableVertexAttribArray(1);
	}

	unsigned int LightVAO;
	{
		glGenVertexArrays(1, &LightVAO);
		glBindVertexArray(LightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		int32 AttributeIndex = -1;

		// position attribute
		glVertexAttribPointer(++AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, Position));
		glEnableVertexAttribArray(AttributeIndex);

		// texture coordinate attribute
		glVertexAttribPointer(++AttributeIndex, 2, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, TextureCoord));
		glEnableVertexAttribArray(AttributeIndex);

		// normal attribute
		glVertexAttribPointer(++AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, Normal));
		glEnableVertexAttribArray(AttributeIndex);
	}

	unsigned int ObjectVAO;
	{
		glGenVertexArrays(1, &ObjectVAO);
		glBindVertexArray(ObjectVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		int32 AttributeIndex = -1;

		// position attribute
		glVertexAttribPointer(++AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, Position));
		glEnableVertexAttribArray(AttributeIndex);

		// texture coordinate attribute
		glVertexAttribPointer(++AttributeIndex, 2, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, TextureCoord));
		glEnableVertexAttribArray(AttributeIndex);

		// normal attribute
		glVertexAttribPointer(++AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVerticeInfo), (void*)OFFSET(FVerticeInfo, Normal));
		glEnableVertexAttribArray(AttributeIndex);
	}


	// safe unbind VBO and VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	FSTBImage::SetFlipVerticallyOnLoad(true);

	uint32 TextureID1 = LoadTexture(FPaths::GetContentDir() + "Texture/container.jpg");
	CheckSlow(TextureID1 != -1);
	
	uint32 TextureID2 = LoadTexture(FPaths::GetContentDir() + "Texture/awesomeface.png");
	CheckSlow(TextureID2 != -1);

	uint32 DiffuseMap = LoadTexture(FPaths::GetContentDir() + "Texture/container2.png");
	CheckSlow(DiffuseMap != -1);

	//uint32 SpecularMap = LoadTexture(FPaths::GetContentDir() + "Texture/container2_specular_colored.png");
	uint32 SpecularMap = LoadTexture(FPaths::GetContentDir() + "Texture/container2_specular.png");
	CheckSlow(SpecularMap != -1);
 
	uint32 EmissionMap = LoadTexture(FPaths::GetContentDir() + "Texture/matrix.jpg");
	CheckSlow(EmissionMap != -1);

	// draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ?????????shader
	Shader.UseProgram();
	// ???fs?????????uniform sampler2D OurTexture????????????0???texture???.
	Shader.SetInt("Texture1", 0);
	Shader.SetInt("Texture2", 1);

	ObjectShader.UseProgram();
	ObjectShader.SetInt("material.Diffuse", 0);
	ObjectShader.SetInt("material.Specular", 1);
	ObjectShader.SetInt("material.Emission", 2);

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);

	{ // init camera
		Camera.Init(glm::vec3(0.f, 0.f, 3.f), 0.f, 0.f, 45.f, 0.1f, 100.f);
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
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// ??????color buffer ??? depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 		// ???TextureID1??????0???texture??????
// 		glActiveTexture(GL_TEXTURE0);
// 		glBindTexture(GL_TEXTURE_2D, TextureID1);
// 
// 		// ???TextureID1??????0???texture??????
// 		glActiveTexture(GL_TEXTURE1);
// 		glBindTexture(GL_TEXTURE_2D, TextureID2);

		// active shader
		//Shader.UseProgram();

		//float timeValue = glfwGetTime();
		//float greenValue = sin(timeValue);
		//
		//int vertexColorLocation = glGetUniformLocation(ShaderProgram, "OurColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// ??????????????????
		//glm::mat4 ViewMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

		Camera.RecalculateViewMatrix();

		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
		ProjectionMatrix = glm::perspective(glm::radians(Camera.GetFov()), (float)SCR_WIDTH /(float)SCR_HEIGHT, Camera.GetNearPlane(), Camera.GetFarPlane());

		//Shader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
		//Shader.SetMaterix4fv("View", glm::value_ptr(ViewMatrix));
		//Shader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));

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

		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

		//lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
		//lightPos.z = 1.0f + sin(glfwGetTime() / 2.f);

		glm::vec3 LightColor(0.5f, 0.5f, 0.5f);
		//LightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
		//LightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
		//LightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));

		glm::vec3 DiffuseColor = LightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 AmbientColor = DiffuseColor * glm::vec3(0.2f); // low influence

		{ // for light
			LightShader.UseProgram();

			// don't forget to use the corresponding shader program first (to set the uniform)
			//LightShader.SetVector("objectColor", 1.0f, 0.5f, 0.31f);
			//LightShader.SetVector("light.Ambient", AmbientColor);
			//LightShader.SetVector("light.Diffuse", DiffuseColor);
			//LightShader.SetVector("light.Specular", 1.0f, 1.0f, 1.0f);
			//LightShader.SetVector("light.position", lightPos);

			LightShader.SetVector("LightColor", LightColor);

			//LightShader.SetVector("material.Ambient", 1.0f, 0.5f, 0.31f);
			//LightShader.SetVector("material.Diffuse", 1.0f, 0.5f, 0.31f);
			//LightShader.SetVector("material.Specular", 0.5f, 0.5f, 0.5f);
			//LightShader.SetFloat("material.Shininess", 32.0f);
			//
			LightShader.SetVector("viewPos", Camera.GetPosition());


			LightShader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
			LightShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));


// 			glm::mat4 ModelMatrix = glm::mat4(1.0f);
// 			ModelMatrix = glm::translate(ModelMatrix, lightPos);
// 			//ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));
// 			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2f));
// 
// 			LightShader.SetMaterix4fv("Model", glm::value_ptr(ModelMatrix));

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindVertexArray(LightVAO);
			for (unsigned int i = 0; i < 4; i++)
			{
				glm::mat4  model = glm::mat4(1.0f);
				model = glm::translate(model, PointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				LightShader.SetMaterix4fv("Model", glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		
		{ // for Object
			ObjectShader.UseProgram();

			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DiffuseMap);

			// bind specular map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, SpecularMap);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, EmissionMap);
			// bind emission map
			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, EmissionMap);

			// don't forget to use the corresponding shader program first (to set the uniform)
			ObjectShader.SetVector("ViewPos", Camera.GetPosition());

			// directional light
			ObjectShader.SetVector("DirectionLight.Direction", -0.2f, -1.0f, -0.3f);
			ObjectShader.SetVector("DirectionLight.Ambient", 0.05f, 0.05f, 0.05f);
			ObjectShader.SetVector("DirectionLight.Diffuse", 0.4f, 0.4f, 0.4f);
			ObjectShader.SetVector("DirectionLight.Specular", 0.5f, 0.5f, 0.5f);

			// point light 1
			ObjectShader.SetVector("PointLights[0].Position", PointLightPositions[0]);
			ObjectShader.SetVector("PointLights[0].Ambient", 0.05f, 0.05f, 0.05f);
			ObjectShader.SetVector("PointLights[0].Diffuse", 0.8f, 0.8f, 0.8f);
			ObjectShader.SetVector("PointLights[0].Specular", 1.0f, 1.0f, 1.0f);
			ObjectShader.SetFloat("PointLights[0].Constant", 1.0f);
			ObjectShader.SetFloat("PointLights[0].Linear", 0.09f);
			ObjectShader.SetFloat("PointLights[0].Quadratic", 0.032f);

			// point light 2
			ObjectShader.SetVector("PointLights[1].Position", PointLightPositions[1]);
			ObjectShader.SetVector("PointLights[1].Ambient", 0.05f, 0.05f, 0.05f);
			ObjectShader.SetVector("PointLights[1].Diffuse", 0.8f, 0.8f, 0.8f);
			ObjectShader.SetVector("PointLights[1].Specular", 1.0f, 1.0f, 1.0f);
			ObjectShader.SetFloat("PointLights[1].Constant", 1.0f);
			ObjectShader.SetFloat("PointLights[1].Linear", 0.09f);
			ObjectShader.SetFloat("PointLights[1].Quadratic", 0.032f);
			// point light 3
			ObjectShader.SetVector("PointLights[2].Position", PointLightPositions[2]);
			ObjectShader.SetVector("PointLights[2].Ambient", 0.05f, 0.05f, 0.05f);
			ObjectShader.SetVector("PointLights[2].Diffuse", 0.8f, 0.8f, 0.8f);
			ObjectShader.SetVector("PointLights[2].Specular", 1.0f, 1.0f, 1.0f);
			ObjectShader.SetFloat("PointLights[2].Constant", 1.0f);
			ObjectShader.SetFloat("PointLights[2].Linear", 0.09f);
			ObjectShader.SetFloat("PointLights[2].Quadratic", 0.032f);
			// point light 4
			ObjectShader.SetVector("PointLights[3].Position", PointLightPositions[3]);
			ObjectShader.SetVector("PointLights[3].Ambient", 0.05f, 0.05f, 0.05f);
			ObjectShader.SetVector("PointLights[3].Diffuse", 0.8f, 0.8f, 0.8f);
			ObjectShader.SetVector("PointLights[3].Specular", 1.0f, 1.0f, 1.0f);
			ObjectShader.SetFloat("PointLights[3].Constant", 1.0f);
			ObjectShader.SetFloat("PointLights[3].Linear", 0.09f);
			ObjectShader.SetFloat("PointLights[3].Quadratic", 0.032f);
			 
			// SpotLight
			ObjectShader.SetVector("SpotLight.Position", Camera.GetPosition());
			ObjectShader.SetVector("SpotLight.Direction", Camera.GetForward());

			ObjectShader.SetVector("SpotLight.Ambient", 0.0f, 0.0f, 0.0f);
			ObjectShader.SetVector("SpotLight.Diffuse", 1.0f, 1.0f, 1.0f);
			ObjectShader.SetVector("SpotLight.Specular", 1.0f, 1.0f, 1.0f);

			ObjectShader.SetFloat("SpotLight.Constant", 1.0f);
			ObjectShader.SetFloat("SpotLight.Linear", 0.09f);
			ObjectShader.SetFloat("SpotLight.Quadratic", 0.032f);

			ObjectShader.SetFloat("SpotLight.Cutoff", glm::cos(glm::radians(12.5f)));
			ObjectShader.SetFloat("SpotLight.OuterCutoff", glm::cos(glm::radians(15.0f)));

			ObjectShader.SetVector("light.Position", Camera.GetPosition());
			ObjectShader.SetVector("light.Direction", Camera.GetForward());
			ObjectShader.SetFloat("light.Cutoff", glm::cos(glm::radians(12.5f)));
			ObjectShader.SetFloat("light.OuterCutoff", glm::cos(glm::radians(17.5f)));

			ObjectShader.SetFloat("light.Constant", 1.0f);
			ObjectShader.SetFloat("light.Linear", 0.09f);
			ObjectShader.SetFloat("light.Quadratic", 0.032f);

			ObjectShader.SetVector("material.Ambient", 1.0f, 0.5f, 0.31f);
			ObjectShader.SetFloat("material.Shininess", 32.f);


			glm::mat4 ModelMatrix = glm::mat4(1.0f);
			//glm::vec3 ObjectPos(1.2f, 1.0f, -7.0f);
			//ModelMatrix = glm::translate(ModelMatrix, ObjectPos);
			//ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));
			//ModelMatrix = glm::rotate(ModelMatrix, glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));

			ObjectShader.SetMaterix4fv("View", glm::value_ptr(Camera.GetViewMatrix()));
			ObjectShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));


			glBindVertexArray(ObjectVAO);
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			for (uint32 i=0; i<10; ++i)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, CubePositions[i]);
				float angle = 20.f*i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				ObjectShader.SetMaterix4fv("Model", glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
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
