#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Containers/String.h"


class FModel
{
public:
	FModel();

protected:
	void LoadModel(const FStdString& InPath);

protected:
	bool GammaCorrection;
	FStdString Path;
};