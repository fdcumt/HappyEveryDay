#include "Shader.h"
#include "OperatorFiles/OperatorFile.h"
#include <glad/glad.h>
#include "Misc/AssertionMacros.h"

DEFINE_LOG_CATEGORY(Shader);

#define CheckShaderValid(Desc) Checkf(bInitSucceed && !bDeleteProgram, Desc);

FShader::FShader(const FStdString& InVertexShaderFileName, const FStdString& InFragmentShaderFileName)
	: VertexShaderFileName(InVertexShaderFileName)
	, FragmentShaderFileName(InFragmentShaderFileName)
{
	bool bLoadVertexShaderOK = LoadShaderFile(VertexShaderContent, VertexShaderFileName);
	bool bLoadFragmentShaderOK = LoadShaderFile(FragmentShaderContent, FragmentShaderFileName);

	if (!bLoadVertexShaderOK || !bLoadFragmentShaderOK)
	{
		return ;
	}

	bool VertexShaderOK = false;
	uint32 VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	{ // Vertex shader
		const char* pContent = VertexShaderContent.data();
		glShaderSource(VertexShaderID, 1, &pContent, nullptr);
		glCompileShader(VertexShaderID);
		VertexShaderOK = CheckShaderCompileError(VertexShaderID, EShaderStatusType::eCompile, VertexShaderFileName);
	}

	bool ProgramShaderOK = false;
	uint32 FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	{ // Fragment shader
		const char* pContent = FragmentShaderContent.data();
		glShaderSource(FragmentShaderID, 1, &pContent, nullptr);
		glCompileShader(FragmentShaderID);

		ProgramShaderOK = CheckShaderCompileError(FragmentShaderID, EShaderStatusType::eCompile, FragmentShaderFileName);
	}

	bool LinkShaderOK = false;
	{ // link program
		ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		FStdString ErrorStr = FStdString("glLinkProgram LinkError---VertexShaderFileName:") + VertexShaderFileName+ FStdString(",FragmentShaderFileName:")+ FragmentShaderFileName;
		LinkShaderOK = CheckShaderCompileError(ProgramID, EShaderStatusType::eLink, ErrorStr);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	bInitSucceed = VertexShaderOK && ProgramShaderOK && LinkShaderOK;
}

FShader::~FShader()
{
	
}

void FShader::UseProgram()
{
	CheckShaderValid("FShader::UseProgram shader init failed, please check");
	glUseProgram(ProgramID);
}

void FShader::DeleteProgram()
{
	CheckShaderValid("FShader::DeleteProgram shader init failed, please check");

	glDeleteProgram(ProgramID);
	bDeleteProgram = true;
}

void FShader::SetBool(const FStdString& InUniformName, bool Value)
{
	CheckShaderValid("FShader::SetBool shader init failed, please check");

	int32 UniformID = glGetUniformLocation(ProgramID, InUniformName.data());
	glUniform1i(UniformID, Value);
}

void FShader::SetInt(const FStdString& InUniformName, int32 Value)
{
	CheckShaderValid("FShader::SetInt shader init failed, please check");

	int32 UniformID = glGetUniformLocation(ProgramID, InUniformName.data());
	glUniform1i(UniformID, Value);
}

void FShader::SetFloat(const FStdString& InUniformName, float Value)
{
	CheckShaderValid("FShader::SetFloat shader init failed, please check");

	int32 UniformID = glGetUniformLocation(ProgramID, InUniformName.data());
	glUniform1f(UniformID, Value);
}

bool FShader::LoadShaderFile(FStdString& OutShaderContent, const FStdString& InShaderFileName)
{
	int32 ContentLen = 0;
	char *pContent = FOperatorFile::ReadFile(InShaderFileName.data(), ContentLen);

	if (pContent)
	{
		OutShaderContent = pContent;
		return true;
	}
	else
	{
		ErrorLog(Shader, "Load Shader [%s] error.", InShaderFileName.data());
		return false;
	}
}

bool FShader::CheckShaderCompileError(uint32 ShaderID, EShaderStatusType ShaderStatusType, const FStdString& ShaderFileName)
{
	int32 bSucceed = 0;
	switch (ShaderStatusType)
	{
	case EShaderStatusType::eCompile:
	{
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &bSucceed);
		break;
	}
	case EShaderStatusType::eLink:
	{
		glGetProgramiv(ShaderID, GL_LINK_STATUS, &bSucceed);
		break;
	}
	}


	if (!bSucceed)
	{
#define MaxShaderInfoLogLen 1024
		char InfoLog[MaxShaderInfoLogLen] = {0};
		switch (ShaderStatusType)
		{
		case EShaderStatusType::eCompile:
		{
			glGetShaderInfoLog(ShaderID, MaxShaderInfoLogLen, NULL, InfoLog);
			break;
		}
		case EShaderStatusType::eLink:
		{
			glGetProgramInfoLog(ShaderID, MaxShaderInfoLogLen, NULL, InfoLog);
			break;
		}
		}

		FStdString ShaderStatusTypeStr = EShaderStatusTypeToString(ShaderStatusType);
		BreakLog(Shader, "CheckShaderCompileError---ShaderStatusType:[%s], Error[%s], FileName[%s]", ShaderStatusTypeStr.data(), InfoLog, ShaderFileName.data());
#undef MaxShaderInfoLog
	}

	return bSucceed!=0;
}

FStdString EShaderStatusTypeToString(EShaderStatusType eShaderStatusType)
{
	switch (eShaderStatusType)
	{
	case EShaderStatusType::eCompile: return FStdString("EShaderStatusType::eCompile");
	case EShaderStatusType::eLink: return FStdString("EShaderStatusType::eLink");
	}
	return FStdString("EShaderStatusType::Unknown");
}

#undef CheckShaderValid
