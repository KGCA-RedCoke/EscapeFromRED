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

void JShader::ApplyShader(ID3D11DeviceContext* InDeviceContext) const
{
	if (!InDeviceContext)
	{
		LOG_CORE_ERROR("DeviceContext is nullptr");
		return;
	}

	// 입력 레이아웃 설정
	InDeviceContext->IASetInputLayout(mInputLayout.Get());

	// 버텍스 셰이더 설정
	InDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);

	// 헐 셰이더 설정
	InDeviceContext->HSSetShader(mHullShader.Get(), nullptr, 0);

	// 도메인 셰이더 설정
	InDeviceContext->DSSetShader(mDomainShader.Get(), nullptr, 0);

	// 지오메트리 셰이더 설정
	InDeviceContext->GSSetShader(mGeometryShader.Get(), nullptr, 0);

	// 픽셀 셰이더 설정
	InDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

}

void JShader::LoadVertexShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadVertexShader(
											DeviceRSC.GetDevice(),
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
										   DeviceRSC.GetDevice(),
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
											  DeviceRSC.GetDevice(),
											  mShaderFile.data(),
											  mGeometryShader.GetAddressOf(),
											  mGeometryShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadHullShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadHullShaderFile(
											  DeviceRSC.GetDevice(),
											  mShaderFile.data(),
											  mHullShader.GetAddressOf(),
											  mHullShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadDomainShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadDomainShaderFile(
												DeviceRSC.GetDevice(),
												mShaderFile.data(),
												mDomainShader.GetAddressOf(),
												mDomainShaderBuf.GetAddressOf(),
												FuncName));
}

void JShader::LoadComputeShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadComputeShaderFile(
												 DeviceRSC.GetDevice(),
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
				DeviceRSC.GetDevice()->CreateInputLayout(
														 InputLayout::HASH_INPUT_LAYOUT_MAP_DESC[hash],
														 InputLayout::HASH_INPUT_LAYOUT_MAP_NUMELEMENT[hash],
														 mVertexShaderBuf->GetBufferPointer(),
														 mVertexShaderBuf->GetBufferSize(),
														 mInputLayout.GetAddressOf()
														));
}
