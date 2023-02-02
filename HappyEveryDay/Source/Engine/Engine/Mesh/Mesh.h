#pragma once
#include <glm/glm.hpp>
#include "HAL/Platform.h"
#include "Containers/String.h"
#include <vector>
#include "../RenderCore/Shader/Shader.h"

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct FVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	int32 m_BoneIDs[MAX_BONE_INFLUENCE]; // bone indexes which influence this vertex
	float m_Weights[MAX_BONE_INFLUENCE]; // weight from each bone
};

struct FTexture
{
	uint32 ID;
	FStdString Type;
	FStdString Path;
};

class FMesh
{
public:
	FMesh();
	FMesh(const vector<FVertex>& InVertices, const vector<uint32>& InIndices, const vector<FTexture>& InTextures);

	vector<FVertex> Vertices;
	vector<uint32> Indices;
	vector<FTexture> Textures;

public:
	void Setup();
	void Draw(const FShader &InShader);

protected:
	uint32 VAO;
	uint32 VBO;
	uint32 EBO;
};
