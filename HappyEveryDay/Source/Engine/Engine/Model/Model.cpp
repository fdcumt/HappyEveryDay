#include "Model/Model.h"
#include "Logging/LogMarcos.h"
#include "glad/glad.h"
#include "StbImage/StbImage.h"


DEFINE_LOG_CATEGORY(ModelLog);
using namespace std;


FModel::FModel(const FStdString& InPath)
{
	LoadModel(InPath);
}

void FModel::LoadModel(const FStdString& InPath)
{
	Assimp::Importer Importer;
	const aiScene *pScene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (pScene==nullptr || pScene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || pScene->mRootNode==nullptr)
	{
		ErrorLog(ModelLog, "FModel::LoadModel Error[%s]", Importer.GetErrorString());
		return ;
	}

	m_Directory = InPath.substr(0, InPath.find_last_of('/'));
	ProcessNode(pScene->mRootNode, pScene);
}

void FModel::ProcessNode(aiNode* pNode, const aiScene* pSene)
{
	for (int32 i=0; i<pNode->mNumMeshes; ++i)
	{
		aiMesh *pMesh = pSene->mMeshes[pNode->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(pMesh, pSene));
	}

	for (int32 i = 0; i < pNode->mNumChildren; ++i)
	{
		ProcessNode(pNode->mChildren[i], pSene);
	}
}

FMesh FModel::ProcessMesh(aiMesh* pAIMesh, const aiScene* pAIScene)
{
	FMesh Mesh;
	for (int32 i=0; i<pAIMesh->mNumVertices; ++i)
	{
		FVertex Vertex;
		{ // position
			Vertex.Position.x = pAIMesh->mVertices[i].x;
			Vertex.Position.y = pAIMesh->mVertices[i].y;
			Vertex.Position.z = pAIMesh->mVertices[i].z;
		}
		
		// normal
		if (pAIMesh->HasNormals()) 
		{ 
			Vertex.Normal.x = pAIMesh->mNormals[i].x;
			Vertex.Normal.y = pAIMesh->mNormals[i].y;
			Vertex.Normal.z = pAIMesh->mNormals[i].z;
		}

		if (pAIMesh->mTextureCoords[0])
		{
			Vertex.TexCoords.x = pAIMesh->mTextureCoords[0][i].x;
			Vertex.TexCoords.y = pAIMesh->mTextureCoords[0][i].y;

			Vertex.Tangent.x = pAIMesh->mTangents[i].x;
			Vertex.Tangent.y = pAIMesh->mTangents[i].y;
			Vertex.Tangent.z = pAIMesh->mTangents[i].y;

			Vertex.Bitangent.x = pAIMesh->mBitangents[i].x;
			Vertex.Bitangent.y = pAIMesh->mBitangents[i].y;
			Vertex.Bitangent.z = pAIMesh->mBitangents[i].y;
		}
		else
		{
			Vertex.TexCoords = glm::vec2(0.f, 0.f);
		}

		Mesh.Vertices.push_back(Vertex);
	}

	for (uint32 i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		for (int32 j = 0; j < pAIMesh->mFaces[i].mNumIndices; ++j)
		{
			Mesh.Indices.push_back(pAIMesh->mFaces[i].mIndices[j]);
		}
	}

	aiMaterial* pMaterial = pAIScene->mMaterials[pAIMesh->mMaterialIndex];

	vector<FTexture> DiffuseMaps = LoadMaterialTexture(pMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
	Mesh.Textures.insert(Mesh.Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());

	vector<FTexture> SpecularMaps = LoadMaterialTexture(pMaterial, aiTextureType_SPECULAR, "texture_specular");
	Mesh.Textures.insert(Mesh.Textures.end(), SpecularMaps.begin(), SpecularMaps.end());

	vector<FTexture> NormalMaps = LoadMaterialTexture(pMaterial, aiTextureType_HEIGHT, "texture_normal");
	Mesh.Textures.insert(Mesh.Textures.end(), NormalMaps.begin(), NormalMaps.end());

	vector<FTexture> HeightMaps = LoadMaterialTexture(pMaterial, aiTextureType_AMBIENT, "texture_height");
	Mesh.Textures.insert(Mesh.Textures.end(), HeightMaps.begin(), HeightMaps.end());


	Mesh.Setup();
	return Mesh;
}

vector<FTexture> FModel::LoadMaterialTexture(aiMaterial* pMaterial, aiTextureType InTextureType, const FStdString& InTypeName)
{
	vector<FTexture> TextureList;
	for (int32 i=0; i<pMaterial->GetTextureCount(InTextureType); ++i)
	{
		aiString str;
		pMaterial->GetTexture(InTextureType, i, &str);
		map<string, FTexture>::iterator it = m_TextureLoaded.find(str.C_Str());
		if (it!=m_TextureLoaded.end())
		{
			TextureList.push_back(it->second);
		}
		else
		{
			FTexture Texture;
			if (LoadTextureFromFile(str.C_Str(), m_Directory, m_bGammaCorrection, Texture.ID))
			{
				Texture.Type = InTypeName;
				Texture.Path = str.C_Str();
				TextureList.push_back(Texture);
				m_TextureLoaded[Texture.Path] = Texture;
			}
			else
			{
				ErrorLog(ModelLog, "LoadMaterialTexture [%s] Failed", str.C_Str());
			}
		}
	}

	return TextureList;
}

bool FModel::LoadTextureFromFile(const FStdString& InFileName, const FStdString& InDirectory, bool InGamma, uint32 &OutTextureID)
{
	FStdString FullPathFileName = InDirectory+"/"+InFileName;

	int32 nWidth, nHeight, nComponent;
	uint8 *pData = FSTBImage::StbiLoad(FullPathFileName.c_str(), &nWidth, &nHeight, &nComponent, 0);
	if (pData)
	{
		glGenTextures(1, &OutTextureID);

		GLenum format;
		if (nComponent == 1)
		{
			format = GL_RED;
		}
		else if (nComponent == 3)
		{
			format = GL_RGB;
		}
		else if (nComponent == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, OutTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, pData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FSTBImage::StbiImageFree(pData);
		return true;
	}
	else
	{
		ErrorLog(ModelLog, "Load Image[%s] Failed", FullPathFileName.c_str());
		return false;
	}
}

void FModel::Draw(const FShader &InShader)
{
	for (FMesh &Mesh: m_Meshes)
	{
		Mesh.Draw(InShader);
	}
}

