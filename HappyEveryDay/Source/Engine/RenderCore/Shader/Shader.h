#pragma once
#include "Containers/String.h"
#include "HAL/Platform.h"
#include "Logging/LogMarcos.h"

DECLARE_LOG_CATEGORY_EXTERN(Shader);

enum class EShaderStatusType : uint8
{
	eCompile,
	eLink,
};

extern FStdString EShaderStatusTypeToString(EShaderStatusType eShaderStatusType);

class FShader
{


public:
	FShader(const FStdString & InVertexShaderFileName, const FStdString & InFragmentShaderFileName);
	~FShader();


public:
	bool IsInitSucceed() const { return bInitSucceed; }
	uint32 GetID() const { return ProgramID; }

public:
	void UseProgram();
	void DeleteProgram();

	void SetBool(const FStdString &InUniformName, bool Value);
	void SetInt(const FStdString& InUniformName, int32 Value);
	void SetFloat(const FStdString& InUniformName, float Value);


protected:
	bool LoadShaderFile(FStdString &OutShaderContent, const FStdString &InShaderFileName);
	bool CheckShaderCompileError(uint32 ShaderID, EShaderStatusType ShaderStatusType, const FStdString & ShaderFileName);


protected:
	bool bInitSucceed = false;
	bool bDeleteProgram = false;

	uint32 ProgramID = 0;
	FStdString VertexShaderFileName;
	FStdString VertexShaderContent;

	FStdString FragmentShaderFileName;
	FStdString FragmentShaderContent;
};
