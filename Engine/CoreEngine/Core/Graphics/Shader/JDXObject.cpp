#include "JDXObject.h"
#include "JShader.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Mesh/JMesh.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "Core/Window/Application.h"

extern FVector4 g_DirectionalLightPos;
extern FVector4 g_DirectionalLightColor;

JDXObject::JDXObject()
{
	

	CreateBuffers();

	mShader = IManager.ShaderManager.CreateOrLoad(L"Basic");

	mScale = 0.1f;
}

JDXObject::JDXObject(Utils::Fbx::FbxFile* InFbxObj)
{
	mPrimitiveType     = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mPrimitiveMeshData = InFbxObj->mMeshList;

	uint32_t vertexBufOffset = 0;
	uint32_t indexBufOffset  = 0;

	for (int32_t dataIdx = 0; dataIdx < mPrimitiveMeshData.size(); ++dataIdx)
	{
		auto mesh = mPrimitiveMeshData[dataIdx].get();
		auto data = mesh->GetVertexData().get();

		if (!mesh->ApplyMaterial())
		{}


		if (!mesh->GetSubMesh().empty())
		{
			//TODO: handle submesh
		}
		else
		{
			if (data->VertexArray.size() < 3)
				continue;

			mVertexNum  = data->VertexArray.size();
			mVertexSize = sizeof(Vertex::FVertexInfo_Base);

			// TODO: Vertex Buffer Update -> Create
			mVertexBegin = vertexBufOffset;
			vertexBufOffset += mVertexNum;


			mIndexNum  = data->IndexArray.size();
			mIndexSize = sizeof(WORD);

			// TODO: Index Buffer Update -> Create
			mIndexBegin = indexBufOffset;
			indexBufOffset += mIndexNum;
		}
	}

	CreateBuffers();

	// mShader = IManager.ShaderManager.CreateOrLoad(L"Basic");
	// mShader = IManager.ShaderManager.CreateOrLoad(L"Toon");
	mShader = IManager.ShaderManager.CreateOrLoad(L"UVAnim");
	// mShader = IManager.ShaderManager.CreateOrLoad(L"WorldGridMaterial");
	// JText texPath = mPrimitiveMeshData[0]->GetMaterial()->GetMaterialParam("DiffuseColor")->StringValue;
	// mTexture      = IManager.TextureManager.CreateOrLoad(texPath);

	mScale = 10.f;
}

void JDXObject::Update(float DeltaTime)
{
	UpdateTransform();
}

void JDXObject::Release() {}

void JDXObject::CreateBuffers()
{
	UpdateTransform();

	ID3D11Device* device = G_Context.GetDevice();
	assert(device);

	// Vertex 버퍼 생성
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_VERTEX_BUFFER,
							(void**)&mPrimitiveMeshData[0]->GetVertexData()->VertexArray.at(0),
							mVertexSize,
							mVertexNum,
							mVertexBuffer.GetAddressOf());

	// Index 버퍼 생성
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_INDEX_BUFFER,
							(void**)&mPrimitiveMeshData[0]->GetVertexData()->IndexArray.at(0),
							mIndexSize,
							mIndexNum,
							mIndexBuffer.GetAddressOf());

	// 상수 버퍼 생성 (WVP)
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Space),
							1,
							mConstantBuffer_Space.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);


	// 상수 버퍼 생성 (World Directional Light)
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Light),
							1,
							mConstantBuffer_Light.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);

	// 상수 버퍼 생성 (World Time)
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Time),
							1,
							mConstantBuffer_Time.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);
}

void JDXObject::UpdateTransform()
{
	// 위치 행렬
	mTranslationMat = XMMatrixTranslation(mTranslation.x, mTranslation.y, mTranslation.z);

	// 회전 행렬 (Yaw-Pitch-Roll 방식 사용 예)
	mRotationMat = XMMatrixRotationRollPitchYaw(
												XMConvertToRadians(mRotation.x),
												XMConvertToRadians(mRotation.y),
												XMConvertToRadians(mRotation.z)
											   );

	// 스케일 행렬
	mScaleMat = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	mWorldMat = mScaleMat * mRotationMat * mTranslationMat;
}

void JDXObject::UpdateSpaceBuffer()
{
	Update(0);

	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);

	JCamera* currentCam = IManager.CameraManager.GetCurrentMainCam();
	assert(currentCam);

	CBuffer::Space wvpMat =
	{
		XMMatrixTranspose(mWorldMat),
		XMMatrixTranspose(currentCam->GetViewMatrix()),
		XMMatrixTranspose(currentCam->GetProjMatrix())
	};

	Utils::DX::UpdateDynamicBuffer(deviceContext,
								   mConstantBuffer_Space.Get(),
								   &wvpMat,
								   sizeof(CBuffer::Space));

	XMVECTOR camWorld = IManager.CameraManager.GetCurrentMainCam()->GetEyePosition();

	CBuffer::Light light;
	light.CameraPos  = FVector4{XMVectorGetX(camWorld), XMVectorGetY(camWorld), XMVectorGetZ(camWorld), 1.f},
	light.LightPos   = g_DirectionalLightPos;
	light.LightColor = g_DirectionalLightColor;

	Utils::DX::UpdateDynamicBuffer(deviceContext,
								   mConstantBuffer_Light.Get(),
								   &light,
								   sizeof(CBuffer::Light));


	auto          curTime = (float)Application::s_AppInstance->GetFramePerSeconds() * (float)Application::s_AppInstance->GetDeltaSeconds();
	CBuffer::Time time    = CBuffer::Time{FVector4{curTime, 0, 0, 0}};

	Utils::DX::UpdateDynamicBuffer(deviceContext,
								   mConstantBuffer_Time.Get(),
								   &time,
								   sizeof(CBuffer::Time));
}

void JDXObject::PreRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);

	uint32_t offset = 0;

	// World, View, Projection 버퍼 업데이트
	UpdateSpaceBuffer();

	// Topology 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Input Layout 설정
	deviceContext->IASetInputLayout(mShader->GetInputLayout());

	// 버퍼 설정
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &mVertexSize, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


	// 버텍스 버퍼 설정
	deviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer_Space.GetAddressOf());
	deviceContext->VSSetConstantBuffers(1, 1, mConstantBuffer_Light.GetAddressOf());
	deviceContext->VSSetConstantBuffers(2, 1, mConstantBuffer_Time.GetAddressOf());
	deviceContext->PSSetConstantBuffers(1, 1, mConstantBuffer_Light.GetAddressOf());
	deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);

	// 헐 셰이더 설정
	deviceContext->HSSetShader(mShader->GetHullShader(), nullptr, 0);

	// 도메인 셰이더 설정
	deviceContext->DSSetShader(mShader->GetDomainShader(), nullptr, 0);

	// 지오메트리 셰이더 설정
	deviceContext->GSSetShader(mShader->GetGeometryShader(), nullptr, 0);

	// 레스터라이저 상태 설정
	deviceContext->RSSetState(G_Context.GetDXTKCommonStates()->CullNone());

	// 픽셀 셰이더 설정
	auto* sampler = G_Context.GetDXTKCommonStates()->LinearWrap();
	deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetMaterial()->GetMaterialParams().size(); ++i)
	{
		JTexture* tex = mPrimitiveMeshData[0]->GetMaterial()->GetMaterialParams().at(i).TextureValue;
		if (tex)
			tex->PreRender(i);
	}
	// mShader->GetDefaultAlbedoTexture()->PreRender(0);
	// mShader->GetDefaultNormalTexture()->PreRender(1);
	// TODO: Texture Sampler 슬롯 Enum값으로 설정
	deviceContext->PSSetSamplers(0, 1, &sampler); // diffuse(albedo) 텍스처 샘플러 설정


	deviceContext->OMSetBlendState(G_Context.GetDXTKCommonStates()->AlphaBlend(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(G_Context.GetDXTKCommonStates()->DepthDefault(), 0);
}

void JDXObject::Render() {}

void JDXObject::PostRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);
	// deviceContext->Draw(mVertexNum, 0);

	if (mIndexNum != 0)
	{
		deviceContext->DrawIndexed(mIndexNum, 0, 0);
	}
	else
	{
		deviceContext->Draw(mVertexNum, 0);
	}
}

void JDXObject::UpdateVertexData(void*          InSrcData, const uint32_t InVertexNum, const uint32_t InVertexSize,
								 const uint32_t InVertexBegin)
{
	mVertexNum   = InVertexNum;
	mVertexSize  = InVertexSize;
	mVertexBegin = InVertexBegin;

	Utils::DX::UpdateBuffer(G_Context.GetImmediateDeviceContext(), mVertexBuffer.Get(), InSrcData);
}

void JDXObject::UpdateIndexData(void* InSrcData, const uint32_t InIndexNum, const uint32_t InIndexBegin)
{
	mIndexNum   = InIndexNum;
	mIndexBegin = InIndexBegin;

	Utils::DX::UpdateBuffer(G_Context.GetImmediateDeviceContext(), mIndexBuffer.Get(), InSrcData);
}

void JDXObject::SetShaderFile(JWTextView InFileName)
{
	mShaderFile = InFileName;

	mShader = IManager.ShaderManager.CreateOrLoad(InFileName.data());
}

void JDXObject::SetShaderFile(const JWText& InFileName)
{
	mShaderFile = InFileName;

	mShader = IManager.ShaderManager.CreateOrLoad(InFileName);
}
