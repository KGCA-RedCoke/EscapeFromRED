#include "MShaderManager.h"

#include "SFXAAEffect.h"


void MShaderManager::Initialize()
{
	CreateOrLoad(L"Shader/Basic.hlsl");
	CreateOrLoad(L"Shader/WorldGridMaterial.hlsl");
	CreateOrLoad(L"Shader/Toon.hlsl");
	CreateOrLoad(L"Shader/UVAnim.hlsl");

	// g_FXAAEffect = CreateOrLoad<SFXAAEffect>(L"Shader/FXAA.hlsl");
}

JShader* MShaderManager::GetBasicShader()
{
	return FetchResource(L"Basic");
}
