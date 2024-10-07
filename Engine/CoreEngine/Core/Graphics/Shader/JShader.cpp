#include "JShader.h"

#include "InputLayouts.h"
#include "Core/Graphics/XD3DDevice.h"
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
}

JShader::~JShader() = default;

uint32_t JShader::GetHash() const
{
	JWText fileRawName = ParseFile(mShaderFile);
	return StringHash(fileRawName.c_str());
}

void JShader::ApplyShader(ID3D11DeviceContext* InDeviceContext) const
{
	if (!InDeviceContext)
	{
		LOG_CORE_ERROR("DeviceContext is nullptr");
		return;
	}

	// 입력 레이아웃 설정
	InDeviceContext->IASetInputLayout(mShaderData.InputLayout.Get());

	// 버텍스 셰이더 설정
	InDeviceContext->VSSetShader(mShaderData.VertexShader.Get(), nullptr, 0);

	// 헐 셰이더 설정
	InDeviceContext->HSSetShader(mShaderData.HullShader.Get(), nullptr, 0);

	// 도메인 셰이더 설정
	InDeviceContext->DSSetShader(mShaderData.DomainShader.Get(), nullptr, 0);

	// 지오메트리 셰이더 설정
	InDeviceContext->GSSetShader(mShaderData.GeometryShader.Get(), nullptr, 0);

	// 픽셀 셰이더 설정
	InDeviceContext->PSSetShader(mShaderData.PixelShader.Get(), nullptr, 0);

}

void JShader::LoadVertexShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadVertexShader(
											IManager.RenderManager->GetDevice(),
											mShaderFile.data(),
											mShaderData.VertexShader.GetAddressOf(),
											mShaderData.VertexShaderBuf.GetAddressOf(),
											FuncName
										   ));

	CreateInputLayout();
}

void JShader::LoadPixelShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadPixelShader(
										   IManager.RenderManager->GetDevice(),
										   mShaderFile.data(),
										   mShaderData.PixelShader.GetAddressOf(),
										   mShaderData.PixelShaderBuf.GetAddressOf(),
										   FuncName
										  ));
}

void JShader::LoadGeometryShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadGeometryShader(
											  IManager.RenderManager->GetDevice(),
											  mShaderFile.data(),
											  mShaderData.GeometryShader.GetAddressOf(),
											  mShaderData.GeometryShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadHullShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadHullShaderFile(
											  IManager.RenderManager->GetDevice(),
											  mShaderFile.data(),
											  mShaderData.HullShader.GetAddressOf(),
											  mShaderData.HullShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadDomainShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadDomainShaderFile(
												IManager.RenderManager->GetDevice(),
												mShaderFile.data(),
												mShaderData.DomainShader.GetAddressOf(),
												mShaderData.DomainShaderBuf.GetAddressOf(),
												FuncName));
}

void JShader::LoadComputeShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadComputeShaderFile(
												 IManager.RenderManager->GetDevice(),
												 mShaderFile.data(),
												 mShaderData.ComputeShader.GetAddressOf(),
												 mShaderData.ComputeShaderBuf.GetAddressOf(),
												 FuncName));
}


void JShader::CreateInputLayout()
{
	assert(InputLayout::HASH_INPUT_LAYOUT_MAP_DESC.contains(GetHash()));
	assert(InputLayout::HASH_INPUT_LAYOUT_MAP_NUMELEMENT.contains(GetHash()));

	CheckResult(
				IManager.RenderManager->GetDevice()->CreateInputLayout(
														 InputLayout::HASH_INPUT_LAYOUT_MAP_DESC[GetHash()],
														 InputLayout::HASH_INPUT_LAYOUT_MAP_NUMELEMENT[GetHash()],
														 mShaderData.VertexShaderBuf->GetBufferPointer(),
														 mShaderData.VertexShaderBuf->GetBufferSize(),
														 mShaderData.InputLayout.GetAddressOf()
														));
}
