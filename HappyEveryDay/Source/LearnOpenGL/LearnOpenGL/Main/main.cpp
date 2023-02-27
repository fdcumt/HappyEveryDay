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
#include "Model/Model.h"


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

uint32 LoadCubeMap(const vector<FStdString> &InPaths)
{
	uint32 TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	int32 Width, Height, nrChannels;
	for (int32 i=0; i<InPaths.size(); ++i)
	{
		uint8 *pData = FSTBImage::StbiLoad(InPaths[i].c_str(), &Width, &Height, &nrChannels, 0);
		if (pData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
			FSTBImage::StbiImageFree(pData);
		}
		else
		{
			ErrorLog(LearnOpenGL, "Load LoadCubeMap Index[%i] Path[%s] Failed", i, InPaths[i].c_str());
			return -1;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return TextureID;
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

const char* VertexShaderSource = nullptr;
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

	// global opengl state 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	FShader BlendingShader(FPaths::GetContentDir() + "/ShaderFiles/3.2.blending.vs", FPaths::GetContentDir() + "/ShaderFiles/3.2.blending.fs");
	FShader ScreenShader(FPaths::GetContentDir() + "/ShaderFiles/5.1.framebuffers_screen.vs", FPaths::GetContentDir() + "/ShaderFiles/5.1.framebuffers_screen.fs");


	FShader CubeMapShader(FPaths::GetContentDir() + "/ShaderFiles/6.1.cubemaps.vs", FPaths::GetContentDir() + "/ShaderFiles/6.1.cubemaps.fs");
	FShader SkyboxShader(FPaths::GetContentDir() + "/ShaderFiles/6.1.skybox.vs", FPaths::GetContentDir() + "/ShaderFiles/6.1.skybox.fs");

	float CubeVertices[] = {
			// back face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
			// front face
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
			// left face
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
			// right face
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
			// bottom face          
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
			// top face
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left              
		};

	float PlaneVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	float TransparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	float QuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};


	float SkyBoxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	uint32 CubeVAO;
	{ // for cube
		uint32 CubeVBO;
		glGenVertexArrays(1, &CubeVAO);
		glGenBuffers(1, &CubeVBO);

		glBindVertexArray(CubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}

	uint32 PlaneVAO;
	{
		uint32 PlaneVBO;
		glGenVertexArrays(1, &PlaneVAO);
		glGenBuffers(1, &PlaneVBO);

		glBindVertexArray(PlaneVAO);

		glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PlaneVertices), PlaneVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}

	uint32 TransparentVAO;
	{
		uint32 TransparentVBO;
		glGenVertexArrays(1, &TransparentVAO);
		glGenBuffers(1, &TransparentVBO);

		glBindVertexArray(TransparentVAO);

		glBindBuffer(GL_ARRAY_BUFFER, TransparentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TransparentVertices), TransparentVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}

	uint32 ScreenQuadVAO, ScreenQuadVBO;
	{
		glGenVertexArrays(1, &ScreenQuadVAO);
		glGenBuffers(1, &ScreenQuadVBO);

		glBindVertexArray(ScreenQuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, ScreenQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
	}

	uint32 SkyBoxVAO;
	{
		uint32 SkyBoxVBO;
		glGenVertexArrays(1, &SkyBoxVAO);
		glGenBuffers(1, &SkyBoxVBO);

		glBindVertexArray(SkyBoxVAO);

		glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SkyBoxVertices), SkyBoxVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
	}

	uint32 FrameBufferObject, TextureColorBufferObject, RenderBufferObject;
	{
		glGenFramebuffers(1, &FrameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObject);

		// texture
		glGenTextures(1, &TextureColorBufferObject);
		glBindTexture(GL_TEXTURE_2D, TextureColorBufferObject);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBufferObject, 0);

		// rbo
		glGenRenderbuffers(1, &RenderBufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferObject);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferObject);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ErrorLog(LearnOpenGL, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



	uint32 CubeTexture = LoadTexture(FPaths::GetContentDir() + "/Texture/marble.jpg");
	CheckSlow(CubeTexture != -1);

	uint32 FloorTexture = LoadTexture(FPaths::GetContentDir() + "/Texture/metal.png");
	CheckSlow(FloorTexture != -1);

	uint32 TransparentTexture = LoadTexture(FPaths::GetContentDir() + "/Texture/window.png");
	CheckSlow(TransparentTexture != -1);


	vector<FStdString> CubeMapTextures = 
	{
		FPaths::GetContentDir() + "/Texture/SkyBox/right.jpg",
		FPaths::GetContentDir() + "/Texture/SkyBox/left.jpg",
		FPaths::GetContentDir() + "/Texture/SkyBox/top.jpg",
		FPaths::GetContentDir() + "/Texture/SkyBox/bottom.jpg",
		FPaths::GetContentDir() + "/Texture/SkyBox/front.jpg",
		FPaths::GetContentDir() + "/Texture/SkyBox/back.jpg"
	};
	uint32 CubeMapTextureID = LoadCubeMap(CubeMapTextures);


	BlendingShader.UseProgram();
	BlendingShader.SetInt("Texture1", 0);

	SkyboxShader.UseProgram();
	SkyboxShader.SetInt("Skybox", 0);

	// transparent window locations
	// --------------------------------
	vector<glm::vec3> Windows
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	// draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	{ // init camera
		Camera.Init(glm::vec3(0.f, 0.f, 3.f), 0.f, 0.f, 45.f, 0.1f, 100.f);
	}

	glEnable(GL_SCISSOR_TEST);

	// render loop
	// -----------
	int32 SissorW=1, SissorH = 1;
	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		SissorW = SissorW < SCR_WIDTH/2 ? SissorW + 1 : SCR_WIDTH;
		SissorH = SissorH < SCR_HEIGHT / 2 ? SissorH + 1 : SCR_HEIGHT;
		if (SissorW >= SCR_WIDTH && SissorH >= SCR_HEIGHT)
		{
			SissorW = 0;
			SissorH = 0;
		}

		glScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// update logic
		Camera.DoMovement(NS_Message::MessageInfoList, MouseXDeltaMove, MouseYDeltaMove);
		MouseXDeltaMove = 0.f;
		MouseYDeltaMove = 0.f;

		// input
		// -----
		processInput(window);


		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObject);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE); 

		// render
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除color buffer 和 depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 		// 把TextureID1和第0个texture绑定
		// 		glActiveTexture(GL_TEXTURE0);
		// 		glBindTexture(GL_TEXTURE_2D, TextureID1);
		// 
		// 		// 把TextureID1和第0个texture绑定
		// 		glActiveTexture(GL_TEXTURE1);
		// 		glBindTexture(GL_TEXTURE_2D, TextureID2);

				// active shader
				//Shader.UseProgram();

				//float timeValue = glfwGetTime();
				//float greenValue = sin(timeValue);
				//
				//int vertexColorLocation = glGetUniformLocation(ShaderProgram, "OurColor");
				//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

				// 设置变换矩阵
				//glm::mat4 ViewMatrix = glm::mat4(1.0f);

		Camera.RecalculateViewMatrix();

		std::map<float, glm::vec3> SortedWindowsLocation;
		for (uint32 i = 0; i < Windows.size(); i++)
		{
			float distance = glm::length(Camera.GetPosition() - Windows[i]);
			SortedWindowsLocation[distance] = Windows[i];
		}

		//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(Camera.GetFov()), (float)SCR_WIDTH / (float)SCR_HEIGHT, Camera.GetNearPlane(), Camera.GetFarPlane());
		glm::mat4 ViewMatrix = Camera.GetViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0f);

		

		BlendingShader.UseProgram();
		BlendingShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));
		BlendingShader.SetMaterix4fv("View", glm::value_ptr(ViewMatrix));

		{ // floor
			BlendingShader.UseProgram();
			glBindVertexArray(PlaneVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FloorTexture);

			glm::mat4 PlaneModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.0f, 0.0f, -1.0f));
			BlendingShader.SetMaterix4fv("Model", glm::value_ptr(PlaneModelMatrix));

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		{ // draw cube and write 1 to stencil buffer for cube
			BlendingShader.UseProgram();
			glBindVertexArray(CubeVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, CubeTexture);

			glm::mat4 CubeModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.0f, 0.0f, -1.0f));
			BlendingShader.SetMaterix4fv("Model", glm::value_ptr(CubeModelMatrix));
			glDrawArrays(GL_TRIANGLES, 0, 36);

			CubeModelMatrix = glm::translate(ModelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
			BlendingShader.SetMaterix4fv("Model", glm::value_ptr(CubeModelMatrix));
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
		}

		{ // draw sky box
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			SkyboxShader.UseProgram();
			SkyboxShader.SetMaterix4fv("Projection", glm::value_ptr(ProjectionMatrix));
			SkyboxShader.SetMaterix4fv("View", glm::value_ptr(ViewMatrix));

			glBindVertexArray(SkyBoxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTextureID);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
		}

		{ // draw transparent windows
			glDepthMask(GL_FALSE); // 关闭深度写入
			BlendingShader.UseProgram();
			glBindVertexArray(TransparentVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TransparentTexture);

			for (std::map<float, glm::vec3>::reverse_iterator it = SortedWindowsLocation.rbegin(); it != SortedWindowsLocation.rend(); ++it)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, it->second);
				BlendingShader.SetMaterix4fv("Model", glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			glBindVertexArray(0);
			glDepthMask(GL_TRUE); // 开启深度写入

		}

		// 将framebuffer设置为默认渲染buffer, 设置SissorTest为整个屏幕. 清一遍默认buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glScissor(0, 0, SCR_WIDTH, SCR_HEIGHT); 
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, SCR_WIDTH/2, SCR_HEIGHT/2);
		{ // draw frame buffer

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glScissor(0, 0, SissorW, SissorH);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			glDepthMask(GL_FALSE); // 关闭深度写入

			ScreenShader.UseProgram();
			glBindVertexArray(ScreenQuadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureColorBufferObject);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST); 
			glDepthMask(GL_TRUE); 
		}

		glViewport(SCR_WIDTH / 2, SCR_HEIGHT / 2, SCR_WIDTH/2, SCR_HEIGHT / 2);
		{ // draw frame buffer

			//glScissor(0, 0, SissorW, SissorH);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glScissor(SCR_WIDTH / 2, SCR_HEIGHT / 2, SissorW, SissorH);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			glDepthMask(GL_FALSE); // 关闭深度写入

			ScreenShader.UseProgram();
			glBindVertexArray(ScreenQuadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureColorBufferObject);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 	glDeleteVertexArrays(1, &VAO);
	// 	glDeleteVertexArrays(1, &LightVAO);
	// 	glDeleteVertexArrays(1, &ObjectVAO);
	// 	glDeleteBuffers(1, &VBO);
	// 	glDeleteBuffers(1, &EBO);
		//Shader.DeleteProgram();


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
