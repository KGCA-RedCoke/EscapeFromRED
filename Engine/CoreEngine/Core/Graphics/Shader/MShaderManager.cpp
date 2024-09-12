#include "common_pch.h"
#include "MShaderManager.h"


void MShaderManager::Initialize()
{
	CreateOrLoad(L"Shader/Basic.hlsl");
	CreateOrLoad(L"Shader/WorldGridMaterial.hlsl");

}

MShaderManager::MShaderManager()
{
}
