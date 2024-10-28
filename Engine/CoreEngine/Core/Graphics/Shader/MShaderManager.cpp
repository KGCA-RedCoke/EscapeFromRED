#include "MShaderManager.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "InputLayouts.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::Initialize_Initialize()
{
	BasicShader = CreateOrLoad<JShader_Basic>(NAME_SHADER_BASIC);
	CreateOrLoad(NAME_SHADER_GNOMON);
}
