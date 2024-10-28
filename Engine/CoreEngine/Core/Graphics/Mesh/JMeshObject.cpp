#include "JMeshObject.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"


JMeshObject::JMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: mName(InName),
	  mVertexSize(sizeof(Vertex::FVertexInfo_Base)),
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

JMeshObject::JMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: JMeshObject(WString2String(InName), InData) {}

Ptr<IManagedInterface> JMeshObject::Clone() const
{
	JArray<Ptr<JMeshData>> clonedData;
	for (const auto& data : mPrimitiveMeshData)
	{
		clonedData.push_back(std::dynamic_pointer_cast<JMeshData>(data->Clone()));
	}
	Ptr<JMeshObject> cloned = MakePtr<JMeshObject>(mName, clonedData);
	return cloned;
}

uint32_t JMeshObject::GetHash() const
{
	return StringHash(mName.c_str());
}

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
	// Mesh Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

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
		}
	}
}

void JMeshObject::Update(float DeltaTime)
{}

void JMeshObject::UpdateBuffer(const FMatrix& InWorldMatrix)
{
	mWorldMatrix = InWorldMatrix;
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
			uint32_t offset   = 0;
			int32_t  indexNum = 0;

			// Topology 설정
			IManager.RenderManager->SetPrimitiveTopology(mPrimitiveType);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			if (subMeshes.empty())
			{
				meshData->GetMaterialInstance()->UpdateWorldMatrix(deviceContext, XMMatrixTranspose(mWorldMatrix));
				meshData->PassMaterial(deviceContext);
				indexNum = meshData->GetVertexData()->IndexArray.size();
			}
			else
			{
				subMeshes[j]->GetMaterialInstance()->UpdateWorldMatrix(deviceContext, XMMatrixTranspose(mWorldMatrix));
				subMeshes[j]->PassMaterial(deviceContext);
				indexNum = subMeshes[j]->GetVertexData()->IndexArray.size();
			}

			int32_t slots[2] = {0, 1};
			IManager.RenderManager->SetSamplerState(ESamplerState::LinearWrap, slots, 2);
			IManager.RenderManager->SetRasterState(IManager.GUIManager->IsRenderWireFrame()
													   ? ERasterState::WireFrame
													   : ERasterState::CullNone);
			IManager.RenderManager->SetBlendState(EBlendState::AlphaBlend);
			IManager.RenderManager->SetDepthStencilState(EDepthStencilState::DepthDefault);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}
