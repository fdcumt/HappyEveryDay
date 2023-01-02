#include "Model/Model.h"
#include "Logging/LogMarcos.h"


DEFINE_LOG_CATEGORY(ModelLog);


void FModel::LoadModel(const FStdString& InPath)
{
	Assimp::Importer Importer;
	const aiScene *pScene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (pScene==nullptr || pScene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || pScene->mRootNode==nullptr)
	{
		ErrorLog(ModelLog, "FModel::LoadModel Error[%s]", Importer.GetErrorString());
		return ;
	}

	Path = InPath;
}

