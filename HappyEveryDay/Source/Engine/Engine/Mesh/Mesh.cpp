#include "Mesh.h"
#include <glad/glad.h> // holds all OpenGL type declarations

FMesh::FMesh(const vector<FVertex>& InVertices, const vector<uint32>& InIndices, const vector<FTexture>& InTextures)
: Vertices(InVertices)
, Indices(InIndices)
, Textures(InTextures)
{
	
}

FMesh::FMesh()
{
	
}

void FMesh::Setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 绑定VAO
	glBindVertexArray(VAO);

	// 绑定VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 将数据拷贝到buffer中
	glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(FVertex), &Vertices[0], GL_STATIC_DRAW);

	// 绑定索引对象
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 向内存中填充索引数据
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(uint32), &Indices[0], GL_STATIC_DRAW);

	int32 AttributeIndex = -1;
	// position
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Position));

	// normal
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Position));

	// TexCooridnate
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 2, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, TexCoords));

	// Tangent
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Tangent));

	// Bitangent
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Bitangent));

	// ids
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribIPointer(AttributeIndex, 4, GL_INT, sizeof(FVertex), (void*)offsetof(FVertex, m_BoneIDs));

	// weights
	glEnableVertexAttribArray(++AttributeIndex);
	glVertexAttribPointer(AttributeIndex, 4, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, m_Weights));

	// back to default
	glBindVertexArray(0);
}

void FMesh::Draw(const FShader& InShader)
{
	uint32 DiffuseIndex = 0;
	uint32 SpecularIndex = 0;
	uint32 NormalIndex = 0;
	uint32 HeightIndex = 0;

	for (int32 i=0; i<Textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0+i);

		FStdString Name = Textures[i].Type;
		FStdString Number;
		if (Name=="texture_diffuse")
		{
			Number = std::to_string(++DiffuseIndex);
		}
		else if (Name == "texture_specular")
		{
			Number = std::to_string(++SpecularIndex);
		}
		else if (Name == "texture_normal")
		{
			Number = std::to_string(++NormalIndex);
		}
		else if (Name == "texture_height")
		{
			Number = std::to_string(++HeightIndex);
		}
		InShader.SetInt(Name+Number, i);
		glBindTexture(GL_TEXTURE_2D, Textures[i].ID);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<uint32>(Indices.size()), GL_UNSIGNED_INT, 0);

	// back to default
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

