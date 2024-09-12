#include "common_pch.h"
#include "JShader.h"

#include "InputLayouts.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"

JShader::JShader(const JText& InShaderFile, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: JShader(String2WString(InShaderFile), VSEntryPoint, PSEntryPoint)
{}

JShader::JShader(const JWText& InShaderFile, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: mShaderFile(InShaderFile)
{
	LoadVertexShader(VSEntryPoint);
	LoadPixelShader(PSEntryPoint);

	mDefaultAlbedoTexture = IManager.TextureManager.CreateOrLoad(L"rsc/Engine/Material/T_Default_Material_Grid_M.png");
	mDefaultNormalTexture = IManager.TextureManager.CreateOrLoad(L"rsc/Engine/Material/T_Default_Material_Grid_N.png");
}

void JShader::LoadVertexShader(LPCSTR FuncName)
{

	CheckResult(
				Utils::DX::LoadVertexShader(
											G_Context.GetDevice(),
											mShaderFile.data(),
											mVertexShader.GetAddressOf(),
											mVertexShaderBuf.GetAddressOf(),
											FuncName
										   ));

	CreateInputLayout();
}

void JShader::LoadPixelShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadPixelShader(
										   G_Context.GetDevice(),
										   mShaderFile.data(),
										   mPixelShader.GetAddressOf(),
										   mPixelShaderBuf.GetAddressOf(),
										   FuncName
										  ));
}

void JShader::LoadGeometryShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadGeometryShader(
											  G_Context.GetDevice(),
											  mShaderFile.data(),
											  mGeometryShader.GetAddressOf(),
											  mGeometryShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadHullShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadHullShaderFile(
											  G_Context.GetDevice(),
											  mShaderFile.data(),
											  mHullShader.GetAddressOf(),
											  mHullShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadDomainShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadDomainShaderFile(
												G_Context.GetDevice(),
												mShaderFile.data(),
												mDomainShader.GetAddressOf(),
												mDomainShaderBuf.GetAddressOf(),
												FuncName));
}

void JShader::LoadComputeShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadComputeShaderFile(
												 G_Context.GetDevice(),
												 mShaderFile.data(),
												 mComputeShader.GetAddressOf(),
												 mComputeShaderBuf.GetAddressOf(),
												 FuncName));
}

void JShader::CreateInputLayout()
{
	uint32_t hash = StringHash(mShaderFile.c_str());
	assert(InputLayout::HASH_INPUT_LAYOUT_MAP_DESC.contains(hash));
	assert(InputLayout::HASH_INPUT_LAYOUT_MAP_NUMELEMENT.contains(hash));

	CheckResult(
				G_Context.GetDevice()->CreateInputLayout(
														 InputLayout::HASH_INPUT_LAYOUT_MAP_DESC[hash],
														 InputLayout::HASH_INPUT_LAYOUT_MAP_NUMELEMENT[hash],
														 mVertexShaderBuf->GetBufferPointer(),
														 mVertexShaderBuf->GetBufferSize(),
														 mInputLayout.GetAddressOf()
														));
}
