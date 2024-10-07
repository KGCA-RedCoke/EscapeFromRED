#include "MShaderManager.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::Initialize_Initialize()
{
	BasicShader = CreateOrLoad(L"Shader/Basic.hlsl");
	CreateOrLoad(L"Shader/Gnomon.hlsl");
}


