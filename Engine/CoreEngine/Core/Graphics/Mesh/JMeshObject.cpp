#include "JMeshObject.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"

extern FVector4 g_DirectionalLightPos;
extern FVector4 g_DirectionalLightColor;

JMeshObject::JMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: mVertexSize(sizeof(Vertex::FVertexInfo_Base)),
	  mIndexSize(sizeof(uint32_t))
{
	if (InData.empty())
	{
		if (!Utils::Serialization::DeSerialize(InName.c_str(), this))
		{
			LOG_CORE_ERROR("Failed to load mesh object(Invalid Path maybe...): {0}", InName);
		}
	}
	else
	{
		mPrimitiveMeshData = InData;
		CreateBuffers();
	}
}

JMeshObject::JMeshObject(const JWText& InName, std::vector<Ptr<JMeshData>> InData)
	: JMeshObject(WString2String(InName), InData) {}

uint32_t JMeshObject::GetType() const
{
	return StringHash("J3DObject");
}

bool JMeshObject::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Primitive Mesh Data
	int32_t meshDataSize = mPrimitiveMeshData.size();
	Utils::Serialization::Serialize_Primitive(&meshDataSize, sizeof(meshDataSize), FileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData[i]->Serialize_Implement(FileStream);
	}

	// Model Data
	Utils::Serialization::Serialize_Primitive(&mVertexSize, sizeof(mVertexSize), FileStream);
	Utils::Serialization::Serialize_Primitive(&mIndexSize, sizeof(mIndexSize), FileStream);

	return true;
}

bool JMeshObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto archivedData = MakePtr<JMeshData>();
		archivedData->DeSerialize_Implement(InFileStream);
		mPrimitiveMeshData.push_back(archivedData);
	}

	// Model Data
	Utils::Serialization::DeSerialize_Primitive(&mVertexSize, sizeof(mVertexSize), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mIndexSize, sizeof(mIndexSize), InFileStream);

	CreateBuffers();

	return true;
}

void JMeshObject::CreateBuffers()
{
	ID3D11Device* device = IManager.RenderManager->GetDevice();
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

			if (data->FaceCount == 0)
			{
				continue;
			}

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

			instanceBuffer.CBuffer_SpaceLightTime[j].CreateBuffer(device);

		}
	}
}

void JMeshObject::Update(float DeltaTime)
{
	UpdateBuffer();
}

void JMeshObject::UpdateBuffer(const FMatrix&  InWorldMatrix, const Ptr<JCamera>& InCameraObj, const FVector4& InLightLoc,
							   const FVector4& InLightColor, const FVector4&      InWorldTime)
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	Ptr<JCamera> currentCam = InCameraObj ? InCameraObj : IManager.CameraManager->GetCurrentMainCam();
	assert(currentCam);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];

		for (int32_t j = 0; j < instanceBuffer.CBuffer_SpaceLightTime.size(); ++j)
		{
			if (instanceBuffer.Buffer_Index[j] == nullptr)
			{
				continue;
			}

			instanceBuffer.CBuffer_SpaceLightTime[j].UpdateSpace(deviceContext,
																 XMMatrixTranspose(InWorldMatrix),
																 XMMatrixTranspose(currentCam->GetViewMatrix()),
																 XMMatrixTranspose(currentCam->GetProjMatrix()));
			instanceBuffer.CBuffer_SpaceLightTime[j].UpdateLight(deviceContext,
																 InLightLoc,
																 InLightColor);

			instanceBuffer.CBuffer_SpaceLightTime[j].UpdateTime(deviceContext,
																InWorldTime);
		}

	}
}

void JMeshObject::PreRender()
{}

void JMeshObject::Render()
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		if (instanceBuffer.Buffer_Vertex.empty())
		{
			continue;
		}

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			if (!subMeshes.empty() && subMeshes[j]->GetVertexData()->FaceCount == 0)
			{
				continue;
			}
			
			uint32_t offset = 0;

			// Topology 설정
			deviceContext->IASetPrimitiveTopology(mPrimitiveType);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);


			instanceBuffer.CBuffer_SpaceLightTime[j].PreRender(deviceContext);
			IManager.CameraManager->SetCameraConstantBuffer();


			if (subMeshes.empty())
			{
				meshData->GetMaterial()->ApplyMaterialParams(deviceContext);
			}
			else
			{
				subMeshes[j]->GetMaterial()->ApplyMaterialParams(deviceContext);
				if (subMeshes[j]->GetVertexData()->FaceCount == 0)
					continue;
			}

			// 레스터라이저 상태 설정
			// deviceContext->RSSetState(IManager.RenderManager->GetDXTKCommonStates()->CullNone());

			// 픽셀 셰이더 설정
			auto* sampler = IManager.RenderManager->GetDXTKCommonStates()->LinearWrap();
			
			deviceContext->PSSetSamplers(0, 1, &sampler); // diffuse(albedo) 텍스처 샘플러 설정
			
			deviceContext->OMSetBlendState(IManager.RenderManager->GetDXTKCommonStates()->AlphaBlend(),
										   nullptr,
										   0xFFFFFFFF);
			deviceContext->OMSetDepthStencilState(IManager.RenderManager->GetDXTKCommonStates()->DepthDefault(), 0);
			
			const int32_t indexNum = subMeshes.empty()
										 ? meshData->GetVertexData()->IndexArray.size()
										 : subMeshes[j]->GetVertexData()->IndexArray.size();
			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}

void JMeshObject::PostRender()
{}

void JMeshObject::Draw()
{
	Render();
}
