#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Containers/String.h"
#include <glm/glm.hpp>
#include <vector>
#include "Mesh/Mesh.h"
#include <map>


using namespace std;


class FModel
{
public:
	FModel(const FStdString& InPath);

public:
	void Draw(const FShader& InShader);


protected:
	void LoadModel(const FStdString& InPath);

	void ProcessNode(aiNode *pNode, const aiScene *pSene);
	FMesh ProcessMesh(aiMesh *pMesh, const aiScene *pScene);

	vector<FTexture> LoadMaterialTexture(aiMaterial *pMaterial, aiTextureType InTextureType, const FStdString &InTypeName);
	bool LoadTextureFromFile(const FStdString &InFileName, const FStdString &InDirectory, bool InGamma, uint32& OutTextureID);


protected:
	bool m_bGammaCorrection;
	FStdString m_Directory;

	vector<FMesh> m_Meshes;
	map<string, FTexture> m_TextureLoaded;

};