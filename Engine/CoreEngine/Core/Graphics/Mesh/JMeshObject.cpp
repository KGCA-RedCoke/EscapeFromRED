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
	}
}

JMeshObject::JMeshObject(const JWText& InName, const JArray<Ptr<JMeshData>>& InData)
	: JMeshObject(WString2String(InName), InData) {}

JMeshObject::JMeshObject(const JMeshObject& Other)
	: mName(Other.mName),
	  mWorldMatrix(Other.mWorldMatrix),
	  mMeshConstantBuffer(Other.mMeshConstantBuffer),
	  mPrimitiveMeshData(Other.mPrimitiveMeshData),
	  mVertexSize(Other.mVertexSize)
{}

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

	return true;
}

void JMeshObject::UpdateBuffer(const FMatrix& InWorldMatrix)
{
	mWorldMatrix = InWorldMatrix;

	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);


	auto& meshData  = mPrimitiveMeshData[0];
	auto& subMeshes = meshData->GetSubMesh();

	for (int32_t j = 0; j < subMeshes.size(); ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
		currMesh->UpdateWorldMatrix(deviceContext, XMMatrixTranspose(mWorldMatrix));
		currMesh->GetMaterialInstance()->UpdateConstantData(deviceContext,
															CBuffer::NAME_CONSTANT_BUFFER_MESH,
															&mMeshConstantBuffer);
	}

}

void JMeshObject::Draw(ID3D11DeviceContext* InDeviceContext)
{

	auto& meshData  = mPrimitiveMeshData[0];
	auto& subMeshes = meshData->GetSubMesh();

	const int32_t count = subMeshes.empty() ? 1 : subMeshes.size();

	for (int32_t j = 0; j < count; ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

		uint32_t offset = 0;

		mVertexSize = currMesh->GetVertexData()->GetVertexSize();

		// Topology 설정

		currMesh->PassMaterial(InDeviceContext);
		int32_t indexNum = currMesh->GetVertexData()->IndexArray.size();

		// 버퍼 설정
		InDeviceContext->IASetVertexBuffers(0,
											1,
											MMeshManager::Get().mMeshList[GetHash()][j].BufferGeometry.Buffer_Vertex[j].GetAddressOf(),
											&mVertexSize,
											&offset);
		InDeviceContext->IASetIndexBuffer(MMeshManager::Get().mMeshList[GetHash()][j].BufferGeometry.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

		InDeviceContext->DrawIndexed(indexNum, 0, 0);
	}
}

void JMeshObject::DrawID(ID3D11DeviceContext* InDeviceContext, uint32_t ID)
{
	// for (int32_t i = 0; i < mGeometryBuffer.size(); ++i)
	// {
	// 	auto& instanceBuffer = mGeometryBuffer[i];
	// 	auto& meshData       = mPrimitiveMeshData[i];
	// 	auto& subMeshes      = meshData->GetSubMesh();
	//
	// 	for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
	// 	{
	// 		uint32_t offset = 0;
	// 		int32_t  indexNum;
	//
	// 		// Topology 설정
	// 		IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	// 		// 버퍼 설정
	// 		InDeviceContext->IASetVertexBuffers(0,
	// 											1,
	// 											instanceBuffer.Buffer_Vertex[j].GetAddressOf(),
	// 											&mVertexSize,
	// 											&offset);
	// 		InDeviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);
	//
	// 		auto idShader = IManager.ShaderManager->IDShader;
	//
	// 		auto cam = IManager.CameraManager->GetCurrentMainCam();
	//
	// 		CBuffer::Space space;
	// 		space.Model      = XMMatrixTranspose(mWorldMatrix);
	// 		space.View       = XMMatrixTranspose(cam->GetViewMatrix());
	// 		space.Projection = XMMatrixTranspose(cam->GetProjMatrix());
	//
	// 		idShader->UpdateConstantData(InDeviceContext,
	// 									 CBuffer::NAME_CONSTANT_BUFFER_SPACE,
	// 									 &space);
	//
	// 		FVector4 color = Hash2Color(ID);
	// 		idShader->UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_COLOR_ID, &color);
	// 		idShader->BindShaderPipeline(InDeviceContext);
	//
	// 		if (subMeshes.empty())
	// 		{
	// 			indexNum = meshData->GetVertexData()->IndexArray.size();
	// 		}
	// 		else
	// 		{
	// 			indexNum = subMeshes[j]->GetVertexData()->IndexArray.size();
	// 		}
	//
	// 		IManager.RenderManager->SetBlendState(EBlendState::Opaque);
	//
	// 		InDeviceContext->DrawIndexed(indexNum, 0, 0);
	// 	}
	// }
}
