#include "MShaderManager.h"

#include <ranges>

#include "Core/Graphics/Shader/JShader_Basic.h"
#include "InputLayouts.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::UpdateCamera(JCamera* InCameraObj) const
{
	for (auto& shader : mManagedList | std::ranges::views::values)
	{
		shader->SetTargetCamera(InCameraObj);
	}
}

void MShaderManager::Initialize_Initialize()
{
	BasicShader = CreateOrLoad<JShader_Basic>(NAME_SHADER_BASIC);
	IDShader    = CreateOrLoad(NAME_SHADER_ID);
	CreateOrLoad(NAME_SHADER_GNOMON);

}
