#pragma once
#include "Containers/String.h"
#include "HAL/Platform.h"
#include "Logging/LogMarcos.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

	void SetBool(const FStdString &InUniformName, bool Value) const;
	void SetInt(const FStdString& InUniformName, int32 Value) const;
	void SetFloat(const FStdString& InUniformName, float Value) const;

	void SetVector(const FStdString& InUniformName, float InX, float InY, float InZ) const;
	void SetVector(const FStdString& InUniformName, const glm::vec3 &InPos) const;

	void SetMaterix4fv(const FStdString& InUniformName, const float *pValue, int32 InCount = 1, bool bTranspose = false) const;


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
