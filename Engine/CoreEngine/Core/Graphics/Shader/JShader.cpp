#include "JShader.h"

#include <d3d11shader.h>
#include <d3dcompiler.h>
#include "InputLayouts.h"
#include "JConstantBuffer.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Graphics/DXUtils.h"

JShader::JShader(const JText& InName, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: JShader(String2WString(InName), VSEntryPoint, PSEntryPoint)
{}

JShader::JShader(const JWText& InName, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: mShaderFile(InName)
{
	LoadVertexShader(VSEntryPoint);
	LoadPixelShader(PSEntryPoint);

	LoadShaderReflectionData();
}

JShader::~JShader() = default;

void JShader::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
{
	if (!InDeviceContext)
	{
		LOG_CORE_ERROR("DeviceContext is nullptr");
		return;
	}

	// 입력 레이아웃 설정
	InDeviceContext->IASetInputLayout(mDefaultShaderData.InputLayout.Get());

	// 셰이더 설정
	InDeviceContext->VSSetShader(mDefaultShaderData.VertexShader.Get(), nullptr, 0);
	InDeviceContext->HSSetShader(mDefaultShaderData.HullShader.Get(), nullptr, 0);
	InDeviceContext->DSSetShader(mDefaultShaderData.DomainShader.Get(), nullptr, 0);
	InDeviceContext->GSSetShader(mDefaultShaderData.GeometryShader.Get(), nullptr, 0);
	InDeviceContext->PSSetShader(mDefaultShaderData.PixelShader.Get(), nullptr, 0);

	UpdateGlobalConstantBuffer(InDeviceContext);
}

void JShader::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const void* InData,
								 const uint32_t       InOffset)
{
	if (JConstantBuffer* buffer = GetConstantBuffer(InBufferName))
	{
		if (InData == nullptr)
		{
			// 데이터가 없을 경우 버퍼를 초기화
			memset(buffer->Data.data() + InOffset, 0, buffer->Size);
		}
		else
		{
			// 기존의 상수버퍼 데이터에 Offset부터 Size만큼 InData를 복사
			memcpy_s(buffer->Data.data() + InOffset, buffer->Size, InData, buffer->Size);
		}
		buffer->UpdateBuffer(InDeviceContext);
	}
}

void JShader::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const JText& InDataName,
								 const void*          InData)
{
	JConstantBuffer* buffer = GetConstantBuffer(InBufferName);
	if (!buffer)
	{
		LOG_CORE_ERROR("Buffer Name is not found in Shader Constant Buffer Table");
		return;
	}
	const FCBufferVariable* variable = buffer->GetVariable(InDataName);
	if (!variable)
	{
		// LOG_CORE_ERROR("Variable Name is not found in Shader Constant Buffer Table");
		return;
	}

	if (InData == nullptr)
	{
		// 데이터가 없을 경우 버퍼를 초기화
		memset(buffer->Data.data() + variable->Offset, 0, variable->Size);
	}
	else
	{
		memcpy_s(
				 buffer->Data.data() + variable->Offset,
				 buffer->Data.size(),
				 InData,
				 variable->Size
				);
	}


	buffer->UpdateBuffer(InDeviceContext);
}

void JShader::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	if (!mTargetCamera)
	{
		mTargetCamera = GetWorld.CameraManager->GetCurrentMainCam();
	}

	if (const auto viewMat = XMMatrixTranspose(mTargetCamera->GetViewMatrix()); mCachedSpaceData.View != viewMat)
	{
		mCachedSpaceData.View = viewMat;
		UpdateConstantData(InDeviceContext,
						   CBuffer::NAME_CONSTANT_BUFFER_SPACE,
						   CBuffer::NAME_CONSTANT_VARIABLE_SPACE_VIEW,
						   &mCachedSpaceData.View);
	}

	if (const auto projMat = XMMatrixTranspose(mTargetCamera->GetProjMatrix()); mCachedSpaceData.Projection != projMat)
	{
		mCachedSpaceData.Projection = projMat;
		UpdateConstantData(InDeviceContext,
						   CBuffer::NAME_CONSTANT_BUFFER_SPACE,
						   CBuffer::NAME_CONSTANT_VARIABLE_SPACE_PROJ,
						   &mCachedSpaceData.Projection);
	}

	if (const auto camPos = FVector4(mTargetCamera->GetEyePositionFVector(), 1.0f); mCachedCameraData.CameraPos != camPos)
	{
		mCachedCameraData.CameraPos = camPos;
		UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_CAMERA, &mCachedCameraData);
	}

	for (int32_t i = 0; i < mDefaultShaderData.ConstantBuffers.size(); ++i)
	{
		mDefaultShaderData.ConstantBuffers.at(i).SetConstantBuffer(InDeviceContext);
	}
}

void JShader::LoadVertexShader(LPCSTR FuncName)
{

	CheckResult(
				Utils::DX::LoadVertexShader(
											GetWorld.D3D11API->GetDevice(),
											mShaderFile.data(),
											mDefaultShaderData.VertexShader.GetAddressOf(),
											mDefaultShaderData.VertexShaderBuf.GetAddressOf(),
											FuncName
										   ));

}

void JShader::LoadPixelShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadPixelShader(
										   GetWorld.D3D11API->GetDevice(),
										   mShaderFile.data(),
										   mDefaultShaderData.PixelShader.GetAddressOf(),
										   mDefaultShaderData.PixelShaderBuf.GetAddressOf(),
										   FuncName
										  ));
}

void JShader::LoadGeometryShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadGeometryShader(
											  GetWorld.D3D11API->GetDevice(),
											  mShaderFile.data(),
											  mDefaultShaderData.GeometryShader.GetAddressOf(),
											  mDefaultShaderData.GeometryShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadHullShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadHullShaderFile(
											  GetWorld.D3D11API->GetDevice(),
											  mShaderFile.data(),
											  mDefaultShaderData.HullShader.GetAddressOf(),
											  mDefaultShaderData.HullShaderBuf.GetAddressOf(),
											  FuncName));
}

void JShader::LoadDomainShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadDomainShaderFile(
												GetWorld.D3D11API->GetDevice(),
												mShaderFile.data(),
												mDefaultShaderData.DomainShader.GetAddressOf(),
												mDefaultShaderData.DomainShaderBuf.GetAddressOf(),
												FuncName));
}

void JShader::LoadComputeShader(LPCSTR FuncName)
{
	CheckResult(
				Utils::DX::LoadComputeShaderFile(
												 GetWorld.D3D11API->GetDevice(),
												 mShaderFile.data(),
												 mDefaultShaderData.ComputeShader.GetAddressOf(),
												 mDefaultShaderData.ComputeShaderBuf.GetAddressOf(),
												 FuncName));
}

typedef HRESULT (WINAPI*pD3DReflectFunc)(LPCVOID, SIZE_T, REFIID, void**);

void JShader::LoadShaderReflectionData()
{
	// 컴파일된 셰이더 바이트코드에서 리플렉션 생성
	ComPtr<ID3D11ShaderReflection> vertexShaderReflection = nullptr;
	ComPtr<ID3D11ShaderReflection> pixelShaderReflection  = nullptr;

	CheckResult(D3DReflect(
						   mDefaultShaderData.VertexShaderBuf->GetBufferPointer(), // 셰이더의 바이트코드 포인터
						   mDefaultShaderData.VertexShaderBuf->GetBufferSize(),    // 셰이더의 바이트코드 크기
						   IID_ID3D11ShaderReflection,						// 리플렉션 인터페이스의 IID
						   (void**)vertexShaderReflection.GetAddressOf()	// 리플렉션 인터페이스 포인터
						  ));

	CheckResult(D3DReflect(
						   mDefaultShaderData.PixelShaderBuf->GetBufferPointer(), // 셰이더의 바이트코드 포인터
						   mDefaultShaderData.PixelShaderBuf->GetBufferSize(),    // 셰이더의 바이트코드 크기
						   IID_ID3D11ShaderReflection,						// 리플렉션 인터페이스의 IID
						   (void**)pixelShaderReflection.GetAddressOf()		// 리플렉션 인터페이스 포인터
						  ));

	D3D11_SHADER_DESC vertexShaderDesc;
	CheckResult(vertexShaderReflection->GetDesc(&vertexShaderDesc));
	D3D11_SHADER_DESC pixelShaderDesc;
	CheckResult(pixelShaderReflection->GetDesc(&pixelShaderDesc));

	// ---------------------------------------------- Input Layout 생성 ----------------------------------------------

	bool bInstanceEncountered = false;

	mDefaultShaderData.VertexInputLayoutSize = 0;
	JArray<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (int32_t i = 0; i < vertexShaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// Shader Input Data를 기반으로 생성..
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName         = paramDesc.SemanticName;
		elementDesc.SemanticIndex        = paramDesc.SemanticIndex;
		elementDesc.InputSlot            = 0;
		elementDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (strncmp(paramDesc.SemanticName, "INSTANCE_", 9) == 0)
		{
			elementDesc.InputSlot            = 1; // 인스턴스 데이터는 슬롯 1
			elementDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
			if (!bInstanceEncountered)
			{
				elementDesc.AlignedByteOffset = 0;
				bInstanceEncountered          = true;
			}
			else
			{
				elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			}
		}

		// Format은 Masking정보를 통해 설정
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;

			mDefaultShaderData.VertexInputLayoutSize += 4;
		} // 1 Byte
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;

			mDefaultShaderData.VertexInputLayoutSize += 8;
		}	// 2 Byte
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

			mDefaultShaderData.VertexInputLayoutSize += 12;
		}	// 3 Byte
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

			mDefaultShaderData.VertexInputLayoutSize += 16;
		} // 4 Byte
		inputLayoutDesc.push_back(elementDesc);
	}

	// ---------------------------------------------- Constant Buffer 생성 ----------------------------------------------
	// 1. Vertex Shader의 상수 버퍼 정보를 가져옴
	for (int32_t i = 0; i < vertexShaderDesc.ConstantBuffers; ++i)
	{
		// 상수 버퍼 리플렉션 가져오기
		ID3D11ShaderReflectionConstantBuffer* cBuffer = vertexShaderReflection->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC bufferDesc;
		CheckResult(cBuffer->GetDesc(&bufferDesc));

		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		CheckResult(vertexShaderReflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc));

		JConstantBuffer constantBuffer(bufferDesc.Name, bufferDesc.Size, bindDesc.BindPoint, true, false);
		constantBuffer.Data.resize(bufferDesc.Size);

		// 상수 버퍼 내부의 변수 정보를 가져옴
		for (UINT j = 0; j < bufferDesc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* pVariable = cBuffer->GetVariableByIndex(j);

			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVariable->GetDesc(&varDesc);

			FCBufferVariable variable;
			variable.Name   = varDesc.Name;
			variable.Hash   = StringHash(varDesc.Name);
			variable.Offset = varDesc.StartOffset;
			variable.Size   = varDesc.Size;

			constantBuffer.Variables.push_back(variable);
			constantBuffer.VariableHashTable.emplace(variable.Hash, j);
		}

		if (!mDefaultShaderData.ConstantBufferHashTable.contains(constantBuffer.Hash))
		{
			constantBuffer.GenBuffer(GetWorld.D3D11API->GetDevice());
			mDefaultShaderData.ConstantBufferHashTable.emplace(constantBuffer.Hash,
															   mDefaultShaderData.ConstantBuffers.size());
			mDefaultShaderData.ConstantBuffers.push_back(constantBuffer);
		}

	}

	// 2. Pixel Shader의 상수 버퍼 정보를 가져옴
	for (int32_t i = 0; i < pixelShaderDesc.ConstantBuffers; ++i)
	{
		// 상수 버퍼 리플렉션 가져오기
		ID3D11ShaderReflectionConstantBuffer* cBuffer = pixelShaderReflection->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC bufferDesc;
		CheckResult(cBuffer->GetDesc(&bufferDesc));

		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		CheckResult(pixelShaderReflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc));

		if (auto it = mDefaultShaderData.ConstantBufferHashTable.find(StringHash(bufferDesc.Name)); it !=
			mDefaultShaderData.
			ConstantBufferHashTable.
			end())
		{
			int32_t index = it->second;
			mDefaultShaderData.ConstantBuffers[index].Flags |= EConstantFlags::PassPixel;
			continue;
		}

		JConstantBuffer constantBuffer(bufferDesc.Name, bufferDesc.Size, bindDesc.BindPoint, false, true);
		constantBuffer.Data.resize(bufferDesc.Size);

		// 상수 버퍼 내부의 변수 정보를 가져옴
		for (UINT j = 0; j < bufferDesc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* pVariable = cBuffer->GetVariableByIndex(j);

			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVariable->GetDesc(&varDesc);

			FCBufferVariable variable;
			variable.Name   = varDesc.Name;
			variable.Hash   = StringHash(varDesc.Name);
			variable.Offset = varDesc.StartOffset;
			variable.Size   = varDesc.Size;

			constantBuffer.Variables.push_back(variable);
			constantBuffer.VariableHashTable.emplace(variable.Hash, j);
		}

		if (!mDefaultShaderData.ConstantBufferHashTable.contains(constantBuffer.Hash))
		{
			constantBuffer.GenBuffer(GetWorld.D3D11API->GetDevice());
			mDefaultShaderData.ConstantBufferHashTable.emplace(constantBuffer.Hash,
															   mDefaultShaderData.ConstantBuffers.size());
			mDefaultShaderData.ConstantBuffers.push_back(constantBuffer);
		}
	}

	CheckResult(
				GetWorld.D3D11API->GetDevice()->CreateInputLayout(
																  &inputLayoutDesc[0],
																  inputLayoutDesc.size(),
																  mDefaultShaderData.VertexShaderBuf->
																  GetBufferPointer(),
																  mDefaultShaderData.VertexShaderBuf->GetBufferSize(),
																  mDefaultShaderData.InputLayout.GetAddressOf()
																 ));


}

JConstantBuffer* JShader::GetConstantBuffer(const JText& InBufferName)
{
	if (auto it = mDefaultShaderData.ConstantBufferHashTable.find(StringHash(InBufferName.c_str())); it !=
		mDefaultShaderData.
		ConstantBufferHashTable.end())
	{
		return &mDefaultShaderData.ConstantBuffers[it->second];
	}
	return nullptr;
}
