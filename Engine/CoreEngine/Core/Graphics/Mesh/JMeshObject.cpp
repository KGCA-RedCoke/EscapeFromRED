#include "JMeshObject.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"

JMeshObject::JMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData)
	: mName(InName),
	  mVertexSize(sizeof(Vertex::FVertexInfo_Base))
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
		JMeshObject::CreateBuffers();
	}
}

JMeshObject::JMeshObject(const JWText& InName, const JArray<Ptr<JMeshData>>& InData)
	: JMeshObject(WString2String(InName), InData) {}

JMeshObject::JMeshObject(const JMeshObject& Other)
	: mName(Other.mName),
	  mInstanceBuffer(Other.mInstanceBuffer),
	  mVertexSize(Other.mVertexSize)
{
	JArray<Ptr<JMeshData>> clonedData;
	for (const auto& data : Other.mPrimitiveMeshData)
	{
		clonedData.push_back(std::dynamic_pointer_cast<JMeshData>(data->Clone()));
	}
	mPrimitiveMeshData = clonedData;
}

Ptr<IManagedInterface> JMeshObject::Clone() const
{
	auto cloned = MakePtr<JMeshObject>(*this);
	return cloned;
}

uint32_t JMeshObject::GetHash() const
{
	return StringHash(mName.c_str());
}

uint32_t JMeshObject::GetType() const
{
	return HASH_ASSET_TYPE_STATIC_MESH;
}

bool JMeshObject::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}
	// Mesh Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Primitive Mesh Data
	int32_t meshDataSize = mPrimitiveMeshData.size();
	Utils::Serialization::Serialize_Primitive(&meshDataSize, sizeof(meshDataSize), FileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData[i]->Serialize_Implement(FileStream);
	}

	return true;
}

bool JMeshObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, this->GetType()))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto archivedData = MakePtr<JMeshData>();
		archivedData->DeSerialize_Implement(InFileStream);
		mPrimitiveMeshData.push_back(archivedData);
	}

	CreateBuffers();

	return true;
}

/** 버퍼는 메모리에 올릴 때 한번만 생성되고 공유 */
void JMeshObject::CreateBuffers()
{
	if (mInstanceBuffer.size() > 0)
	{
		return;
	}

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
									SIZE_INDEX_BUFFER,
									data->IndexArray.size(),
									instanceBuffer.Buffer_Index[j].GetAddressOf());
		}
	}
}

void JMeshObject::UpdateBuffer(const FMatrix& InWorldMatrix)
{
	mWorldMatrix = InWorldMatrix;

	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
			currMesh->UpdateWorldMatrix(deviceContext, XMMatrixTranspose(mWorldMatrix));
			currMesh->GetMaterialInstance()->UpdateConstantData(deviceContext,
																CBuffer::NAME_CONSTANT_BUFFER_MESH,
																&mMeshConstantBuffer);
		}
	}
}

void JMeshObject::Draw()
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

			uint32_t offset = 0;

			mVertexSize = currMesh->GetVertexData()->GetVertexSize();

			// Topology 설정

			currMesh->PassMaterial(deviceContext);
			int32_t indexNum = currMesh->GetVertexData()->IndexArray.size();

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}

void JMeshObject::DrawID(uint32_t ID)
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			uint32_t offset = 0;
			int32_t  indexNum;

			// Topology 설정
			IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			auto idShader = IManager.ShaderManager->IDShader;

			auto cam = IManager.CameraManager->GetCurrentMainCam();

			CBuffer::Space space;
			space.Model      = XMMatrixTranspose(mWorldMatrix);
			space.View       = XMMatrixTranspose(cam->GetViewMatrix());
			space.Projection = XMMatrixTranspose(cam->GetProjMatrix());

			idShader->UpdateConstantData(deviceContext,
										 CBuffer::NAME_CONSTANT_BUFFER_SPACE,
										 &space);

			FVector4 color = Hash2Color(ID);
			idShader->UpdateConstantData(deviceContext, CBuffer::NAME_CONSTANT_BUFFER_COLOR_ID, &color);
			idShader->BindShaderPipeline(deviceContext);

			if (subMeshes.empty())
			{
				indexNum = meshData->GetVertexData()->IndexArray.size();
			}
			else
			{
				indexNum = subMeshes[j]->GetVertexData()->IndexArray.size();
			}

			IManager.RenderManager->SetBlendState(EBlendState::Opaque);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}
