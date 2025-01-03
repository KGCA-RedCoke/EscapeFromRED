﻿#include "MShaderManager.h"

#include <ranges>

#include "InputLayouts.h"
#include "JShader_Shadow.h"
#include "JShader_UI.h"
#include "Core/Graphics/Shader/JShader_Basic.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::UpdateCamera(JCameraComponent* InCameraObj)
{
	if (mCachedCamera != InCameraObj)
	{
		mCachedCamera = InCameraObj;

		for (auto& shader : mManagedList | std::ranges::views::values)
		{
			shader->SetTargetCamera(InCameraObj);
		}
	}
}

void MShaderManager::UpdateShader(ID3D11DeviceContext* InDeviceContext, JShader* InShader)
{
	if (InShader && mCachedShader != InShader)
	{
		InShader->BindShaderPipeline(InDeviceContext);
	}

	mCachedShader = InShader;
}

void MShaderManager::Initialize_Initialize()
{
	BasicShader     = Load<JShader_Basic>(NAME_SHADER_BASIC);
	IDShader        = Load(NAME_SHADER_ID);
	UIElementShader = Load(NAME_SHADER_ID_UI);
	ColorShader     = Load(NAME_SHADER_SIMPLE_COLOR);
	UIShader        = Load<JShader_UI>(NAME_SHADER_UI);
	ShadowMapShader = Load<JShader_Shadow>(NAME_SHADER_SHADOW_MAP);

	Load(NAME_SHADER_GNOMON);

}
