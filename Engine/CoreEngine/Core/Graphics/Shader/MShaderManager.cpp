#include "common_pch.h"
#include "MShaderManager.h"


void MShaderManager::Initialize()
{
	CreateOrLoad(L"Shader/Basic.hlsl");
	CreateOrLoad(L"Shader/WorldGridMaterial.hlsl");
	CreateOrLoad(L"Shader/Toon.hlsl");
	CreateOrLoad(L"Shader/UVAnim.hlsl");
}

MShaderManager::MShaderManager()
{
}
