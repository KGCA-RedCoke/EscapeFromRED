#include "MShaderManager.h"

#include <ranges>

#include "InputLayouts.h"
#include "Core/Graphics/Shader/JShader_Basic.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::UpdateCamera(JCameraComponent* InCameraObj) const
{
	for (auto& shader : mManagedList | std::ranges::views::values)
	{
		shader->SetTargetCamera(InCameraObj);
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
	BasicShader = CreateOrLoad<JShader_Basic>(NAME_SHADER_BASIC);
	IDShader    = CreateOrLoad(NAME_SHADER_ID);
	ColorShader = CreateOrLoad(NAME_SHADER_SIMPLE_COLOR);

	CreateOrLoad(NAME_SHADER_GNOMON);

}
