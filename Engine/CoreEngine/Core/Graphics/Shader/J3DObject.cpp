#include "J3DObject.h"
#include "JShader.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"

extern FVector4 g_DirectionalLightPos;
extern FVector4 g_DirectionalLightColor;

J3DObject::J3DObject(const JWText& ModelFileName)
{
	std::ifstream archive(ModelFileName, std::ios::binary);

	size_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), archive);
	mPrimitiveMeshData.reserve(meshDataSize);
	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData.push_back(MakePtr<JMeshData>());
		mPrimitiveMeshData[i]->DeSerialize(archive);
	}

	mVertexSize = sizeof(Vertex::FVertexInfo_Base);
	mIndexSize  = sizeof(WORD);

	CreateBuffers();

	XMVECTOR scale;
	XMVECTOR rotation;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotation, &translation, mPrimitiveMeshData[0]->GetInitialModelTransform());

	mScale       = scale;
	mRotation    = rotation;
	mTranslation = translation;
}

void J3DObject::Serialize(std::ofstream& FileStream)
{
	// Primitive Mesh Data
	int32_t meshDataSize = mPrimitiveMeshData.size();
	Utils::Serialization::Serialize_Primitive(&meshDataSize, sizeof(meshDataSize), FileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData[i]->Serialize(FileStream);
	}

	// Model Data
	Utils::Serialization::Serialize_Primitive(&mVertexSize, sizeof(mVertexSize), FileStream);
	Utils::Serialization::Serialize_Primitive(&mIndexSize, sizeof(mIndexSize), FileStream);
	Utils::Serialization::Serialize_Primitive(&mVertexBegin, sizeof(mVertexBegin), FileStream);
	Utils::Serialization::Serialize_Primitive(&mIndexBegin, sizeof(mIndexBegin), FileStream);

	// Model Transform Data
	Utils::Serialization::Serialize_Primitive(&mTranslation, sizeof(mTranslation), FileStream);
	Utils::Serialization::Serialize_Primitive(&mRotation, sizeof(mRotation), FileStream);
	Utils::Serialization::Serialize_Primitive(&mScale, sizeof(mScale), FileStream);
}

void J3DObject::DeSerialize(std::ifstream& InFileStream)
{
	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	mPrimitiveMeshData.resize(meshDataSize, MakePtr<JMeshData>());

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData[i]->DeSerialize(InFileStream);
	}

	// Model Data
	Utils::Serialization::DeSerialize_Primitive(&mVertexSize, sizeof(mVertexSize), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mIndexSize, sizeof(mIndexSize), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mVertexBegin, sizeof(mVertexBegin), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mIndexBegin, sizeof(mIndexBegin), InFileStream);

	// Model Transform Data
	Utils::Serialization::DeSerialize_Primitive(&mTranslation, sizeof(mTranslation), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mRotation, sizeof(mRotation), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mScale, sizeof(mScale), InFileStream);

	CreateBuffers();
}

void J3DObject::CreateBuffers()
{
	UpdateTransform();

	ID3D11Device* device = DeviceRSC.GetDevice();
	assert(device);

	const int32_t meshDataSize = mPrimitiveMeshData.size();

	mInstanceBuffer.resize(meshDataSize);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto& mesh           = mPrimitiveMeshData[i];
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& subMeshes      = mesh->GetSubMesh();

		instanceBuffer.Resize(subMeshes.empty() ? 1 : subMeshes.size());

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& data = subMeshes.empty()
							 ? mesh->GetVertexData()
							 : subMeshes[j]->GetVertexData();

			// Vertex 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_VERTEX_BUFFER,
									reinterpret_cast<void**>(&data->VertexArray.at(0)),
									mVertexSize,
									data->VertexArray.size(),
									instanceBuffer.Buffer_Vertex[j].GetAddressOf());

			// Index 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_INDEX_BUFFER,
									reinterpret_cast<void**>(&data->IndexArray.at(0)),
									mIndexSize,
									data->IndexArray.size(),
									instanceBuffer.Buffer_Index[j].GetAddressOf());

			// 상수 버퍼 생성 (WVP)
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_CONSTANT_BUFFER,
									nullptr,
									sizeof(CBuffer::Space),
									1,
									instanceBuffer.CBuffer_Space[j].GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);


			// 상수 버퍼 생성 (World Directional Light)
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_CONSTANT_BUFFER,
									nullptr,
									sizeof(CBuffer::Light),
									1,
									instanceBuffer.CBuffer_Light[j].GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);
		}
	}
}

void J3DObject::Update(float DeltaTime)
{
	UpdateTransform();
	UpdateBuffer();
}

void J3DObject::UpdateTransform()
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

void J3DObject::UpdateBuffer()
{
	auto* deviceContext = DeviceRSC.GetImmediateDeviceContext();
	assert(deviceContext);

	JCamera* currentCam = IManager.CameraManager.GetCurrentMainCam();
	assert(currentCam);

	CBuffer::Space wvpMat =
	{
		XMMatrixTranspose(mWorldMat),
		XMMatrixTranspose(currentCam->GetViewMatrix()),
		XMMatrixTranspose(currentCam->GetProjMatrix())
	};

	CBuffer::Light light =
	{
		g_DirectionalLightPos,
		g_DirectionalLightColor
	};

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		for (int32_t j = 0; j < instanceBuffer.CBuffer_Space.size(); ++j)
		{
			Utils::DX::UpdateDynamicBuffer(deviceContext,
										   instanceBuffer.CBuffer_Space[j].Get(),
										   &wvpMat,
										   sizeof(CBuffer::Space));

			Utils::DX::UpdateDynamicBuffer(deviceContext,
										   instanceBuffer.CBuffer_Light[j].Get(),
										   &light,
										   sizeof(CBuffer::Light));
		}
	}
}

void J3DObject::PreRender()
{
	Update(0);

	auto* deviceContext = DeviceRSC.GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			uint32_t offset = 0;

			// Topology 설정
			deviceContext->IASetPrimitiveTopology(mPrimitiveType);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R16_UINT, 0);

			// 버텍스 버퍼 설정
			deviceContext->VSSetConstantBuffers(0, 1, instanceBuffer.CBuffer_Space[j].GetAddressOf());
			deviceContext->VSSetConstantBuffers(1, 1, instanceBuffer.CBuffer_Light[j].GetAddressOf());
			IManager.CameraManager.SetCameraConstantBuffer(2);
			deviceContext->PSSetConstantBuffers(1, 1, instanceBuffer.CBuffer_Light[j].GetAddressOf());

			if (subMeshes.empty())
			{
				meshData->GetMaterial()->BindMaterial(deviceContext);
			}
			else
			{
				subMeshes[j]->GetMaterial()->BindMaterial(deviceContext);
			}

			// 레스터라이저 상태 설정
			deviceContext->RSSetState(DeviceRSC.GetDXTKCommonStates()->CullNone());

			// 픽셀 셰이더 설정
			auto* sampler = DeviceRSC.GetDXTKCommonStates()->LinearWrap();

			// for (int32_t k = 0; k < 2; ++k)
			// {
			// 	if (subMeshes.empty())
			// 	{
			// 		JMaterial* material = meshData->GetMaterial();
			// 		if (material)
			// 		{
			// 			material->GetMaterialParams().at(k).TextureValue->PreRender(k);
			// 		}
			// 	}
			// 	else
			// 	{
			// 		JMaterial* material = subMeshes[j]->GetMaterial();
			// 		if (material)
			// 		{
			// 			auto& materialParams = material->GetMaterialParams();
			// 			if (materialParams.empty())
			// 			{
			// 				continue;
			// 			}
			// 			JTexture* tex = materialParams.at(k).TextureValue;
			// 			if (tex)
			// 				tex->PreRender(k);
			// 		}
			//
			// 	}
			//
			// }

			deviceContext->PSSetSamplers(0, 1, &sampler); // diffuse(albedo) 텍스처 샘플러 설정


			deviceContext->OMSetBlendState(DeviceRSC.GetDXTKCommonStates()->AlphaBlend(), nullptr, 0xFFFFFFFF);
			deviceContext->OMSetDepthStencilState(DeviceRSC.GetDXTKCommonStates()->DepthDefault(), 0);

			const int32_t indexNum = subMeshes.empty()
										 ? meshData->GetVertexData()->IndexArray.size()
										 : subMeshes[j]->GetVertexData()->IndexArray.size();
			deviceContext->DrawIndexed(indexNum, 0, 0);

		}
	}
}

void J3DObject::Render() {}

void J3DObject::PostRender()
{
	auto* deviceContext = DeviceRSC.GetImmediateDeviceContext();
	assert(deviceContext);
	// deviceContext->Draw(mVertexNum, 0);

	// if (mIndexNum != 0)
	// {
	// 	deviceContext->DrawIndexed(mIndexNum, 0, 0);
	// }
	// else
	// {
	// 	deviceContext->Draw(mVertexNum, 0);
	// }
}
