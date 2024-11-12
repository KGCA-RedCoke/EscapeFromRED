#include "JShader.h"

#include <d3d11shader.h>
#include <d3dcompiler.h>
#include "InputLayouts.h"
#include "JConstantBuffer.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"
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
	InDeviceContext->IASetInputLayout(mShaderData.InputLayout.Get());

	// 셰이더 설정
	InDeviceContext->VSSetShader(mShaderData.VertexShader.Get(), nullptr, 0);
	InDeviceContext->HSSetShader(mShaderData.HullShader.Get(), nullptr, 0);
	InDeviceContext->DSSetShader(mShaderData.DomainShader.Get(), nullptr, 0);
	InDeviceContext->GSSetShader(mShaderData.GeometryShader.Get(), nullptr, 0);
	InDeviceContext->PSSetShader(mShaderData.PixelShader.Get(), nullptr, 0);

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
		mTargetCamera = IManager.CameraManager->GetCurrentMainCam();
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

	for (int32_t i = 0; i < mShaderData.ConstantBuffers.size(); ++i)
	{
		mShaderData.ConstantBuffers.at(i).SetConstantBuffer(InDeviceContext);
	}
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

typedef HRESULT (WINAPI*pD3DReflectFunc)(LPCVOID, SIZE_T, REFIID, void**);

void JShader::LoadShaderReflectionData()
{
	// 컴파일된 셰이더 바이트코드에서 리플렉션 생성
	ComPtr<ID3D11ShaderReflection> vertexShaderReflection = nullptr;
	ComPtr<ID3D11ShaderReflection> pixelShaderReflection  = nullptr;
	
	CheckResult(D3DReflect(
						   mShaderData.VertexShaderBuf->GetBufferPointer(), // 셰이더의 바이트코드 포인터
						   mShaderData.VertexShaderBuf->GetBufferSize(),    // 셰이더의 바이트코드 크기
						   IID_ID3D11ShaderReflection,						// 리플렉션 인터페이스의 IID
						   (void**)vertexShaderReflection.GetAddressOf()	// 리플렉션 인터페이스 포인터
						  ));
	
	CheckResult(D3DReflect(
						   mShaderData.PixelShaderBuf->GetBufferPointer(), // 셰이더의 바이트코드 포인터
						   mShaderData.PixelShaderBuf->GetBufferSize(),    // 셰이더의 바이트코드 크기
						   IID_ID3D11ShaderReflection,						// 리플렉션 인터페이스의 IID
						   (void**)pixelShaderReflection.GetAddressOf()		// 리플렉션 인터페이스 포인터
						  ));

	D3D11_SHADER_DESC vertexShaderDesc;
	CheckResult(vertexShaderReflection->GetDesc(&vertexShaderDesc));
	D3D11_SHADER_DESC pixelShaderDesc;
	CheckResult(pixelShaderReflection->GetDesc(&pixelShaderDesc));

	// ---------------------------------------------- Input Layout 생성 ----------------------------------------------
	mShaderData.VertexInputLayoutSize = 0;
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

		// Format은 Masking정보를 통해 설정
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;

			mShaderData.VertexInputLayoutSize += 4;
		} // 1 Byte
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;

			mShaderData.VertexInputLayoutSize += 8;
		}	// 2 Byte
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

			mShaderData.VertexInputLayoutSize += 12;
		}	// 3 Byte
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

			mShaderData.VertexInputLayoutSize += 16;
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

		if (!mShaderData.ConstantBufferHashTable.contains(constantBuffer.Hash))
		{
			constantBuffer.GenBuffer(IManager.RenderManager->GetDevice());
			mShaderData.ConstantBufferHashTable.emplace(constantBuffer.Hash, mShaderData.ConstantBuffers.size());
			mShaderData.ConstantBuffers.push_back(constantBuffer);
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

		if (auto it = mShaderData.ConstantBufferHashTable.find(StringHash(bufferDesc.Name)); it != mShaderData.
			ConstantBufferHashTable.
			end())
		{
			int32_t index = it->second;
			mShaderData.ConstantBuffers[index].Flags |= EConstantFlags::PassPixel;
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

		if (!mShaderData.ConstantBufferHashTable.contains(constantBuffer.Hash))
		{
			constantBuffer.GenBuffer(IManager.RenderManager->GetDevice());
			mShaderData.ConstantBufferHashTable.emplace(constantBuffer.Hash, mShaderData.ConstantBuffers.size());
			mShaderData.ConstantBuffers.push_back(constantBuffer);
		}
	}

	CheckResult(
				IManager.RenderManager->GetDevice()->CreateInputLayout(
																	   &inputLayoutDesc[0],
																	   inputLayoutDesc.size(),
																	   mShaderData.VertexShaderBuf->GetBufferPointer(),
																	   mShaderData.VertexShaderBuf->GetBufferSize(),
																	   mShaderData.InputLayout.GetAddressOf()
																	  ));


}

JConstantBuffer* JShader::GetConstantBuffer(const JText& InBufferName)
{
	if (auto it = mShaderData.ConstantBufferHashTable.find(StringHash(InBufferName.c_str())); it != mShaderData.
		ConstantBufferHashTable.end())
	{
		return &mShaderData.ConstantBuffers[it->second];
	}
	return nullptr;
}
