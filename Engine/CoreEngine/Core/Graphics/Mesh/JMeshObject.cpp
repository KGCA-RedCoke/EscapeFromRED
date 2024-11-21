#include "JMeshObject.h"

#include "MMeshManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Interface/JWorld.h"

JMeshObject::JMeshObject(const JText& InAssetPath, const JArray<Ptr<JMeshData>>& InData)
	: mName(ParseFile(InAssetPath)),
	  mMeshConstantBuffer(),
	  mVertexSize(sizeof(Vertex::FVertexInfo_Base))
{
	mPath = InAssetPath;
	mName = ParseFile(mPath);

	mPrimitiveMeshData = InData;
}

JMeshObject::JMeshObject(const JWText& InAssetPath, const JArray<Ptr<JMeshData>>& InData)
	: JMeshObject(WString2String(InAssetPath), InData) {}

JMeshObject::JMeshObject(const JMeshObject& Other)
	: mName(Other.mName),
	  mGeometryBuffer(Other.mGeometryBuffer),
	  mMeshConstantBuffer(Other.mMeshConstantBuffer),
	  mVertexSize(Other.mVertexSize)
{
	mPath = Other.mPath;

	mPrimitiveMeshData = Other.mPrimitiveMeshData;

	for (int32_t i = 0; i < Other.mMaterialInstances.size(); ++i)
	{
		mMaterialInstances.push_back(Other.mMaterialInstances[i]);
	}
}

UPtr<IManagedInterface> JMeshObject::Clone() const
{
	return MakeUPtr<JMeshObject>(*this);
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

	for (int32_t i = 0; i < mMaterialInstances.size(); ++i)
	{
		JText materialPath = mMaterialInstances[i]->GetMaterialPath();
		Utils::Serialization::Serialize_Text(materialPath, FileStream);
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

	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetSubMaterialNum(); ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
		auto* matInstance = GetWorld.MaterialInstanceManager->CreateOrLoad(materialPath);
		mMaterialInstances.push_back(matInstance);
	}

	return true;
}

/** 버퍼는 메모리에 올릴 때 한번만 생성되고 공유 */
void JMeshObject::CreateBuffers(ID3D11Device* InDevice, JHash<uint32_t, Buffer::FBufferGeometry>& InBufferList)
{
	// if (mGeometryBuffer.size() > 0)
	// {
	// 	return;
	// }
	//
	// ID3D11Device* device = GetWorld.D3D11API->GetDevice();
	// assert(device);
	//
	// const int32_t meshDataSize = mPrimitiveMeshData.size();
	//
	// mGeometryBuffer.resize(meshDataSize);
	//
	// for (int32_t i = 0; i < meshDataSize; ++i)
	// {
	// 	auto& mesh           = mPrimitiveMeshData[i];
	// 	auto& instanceBuffer = mGeometryBuffer[i];
	// 	auto& subMeshes      = mesh->GetSubMesh();
	//
	// 	instanceBuffer.Resize(subMeshes.empty() ? 1 : subMeshes.size());
	//
	// 	for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
	// 	{
	// 		auto& data = subMeshes.empty()
	// 						 ? mesh->GetVertexData()
	// 						 : subMeshes[j]->GetVertexData();
	//
	// 		// Vertex 버퍼 생성
	// 		Utils::DX::CreateBuffer(device,
	// 								D3D11_BIND_VERTEX_BUFFER,
	// 								reinterpret_cast<void**>(&data->VertexArray.at(0)),
	// 								mVertexSize,
	// 								data->VertexArray.size(),
	// 								instanceBuffer.Buffer_Vertex[j].GetAddressOf());
	//
	// 		// Index 버퍼 생성
	// 		Utils::DX::CreateBuffer(device,
	// 								D3D11_BIND_INDEX_BUFFER,
	// 								reinterpret_cast<void**>(&data->IndexArray.at(0)),
	// 								SIZE_INDEX_BUFFER,
	// 								data->IndexArray.size(),
	// 								instanceBuffer.Buffer_Index[j].GetAddressOf());
	// 	}
	// }
}

void JMeshObject::UpdateBuffer(const FMatrix& InWorldMatrix)
{
	mWorldMatrix = InWorldMatrix;

	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	const int32_t lodCount = mPrimitiveMeshData.size();

	for (int32_t i = 0; i < lodCount; ++i)
	{
		auto&         meshData     = mPrimitiveMeshData[i];
		auto&         subMeshes    = meshData->GetSubMesh();
		const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

		for (int32_t j = 0; j < subMeshCount; ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
			mMaterialInstances[j]->UpdateConstantData(deviceContext,
													  CBuffer::NAME_CONSTANT_BUFFER_MESH,
													  &mMeshConstantBuffer);
		}
	}
}

void JMeshObject::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex)
{
	if (InIndex >= mPrimitiveMeshData[0]->GetSubMaterialNum())
	{
		LOG_CORE_WARN("머티리얼 슬롯 인덱스가 범위를 벗어남.");
		return;
	}

	mMaterialInstances[InIndex] = InMaterialInstance;
}

void JMeshObject::SetMaterialInstance(JMaterialInstance* InMaterialInstance, const JText& InName) {}

JMaterialInstance* JMeshObject::GetMaterialInstance(uint32_t InIndex) const
{
	if (InIndex >= mPrimitiveMeshData[0]->GetSubMaterialNum())
	{
		LOG_CORE_WARN("머티리얼 슬롯 인덱스가 범위를 벗어남.");
		return nullptr;
	}

	return mMaterialInstances[InIndex];
}

JMaterialInstance* JMeshObject::GetMaterialInstance(const JText& InName) const
{
	return nullptr;
}

void JMeshObject::AddInstance()
{
	const int32_t lodCount = mPrimitiveMeshData.size();

	for (int32_t i = 0; i < lodCount; ++i)
	{
		auto&         meshData     = mPrimitiveMeshData[i];
		auto&         subMeshes    = meshData->GetSubMesh();
		const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

		for (int32_t j = 0; j < subMeshCount; ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

			FInstanceData_Mesh data;
			data.WorldMatrix = mWorldMatrix;
			GetWorld.MeshManager->PushCommand(currMesh->GetHash(), data);
		}
	}
}

void JMeshObject::Draw()
{
	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	const int32_t lodCount = mPrimitiveMeshData.size();

	for (int32_t i = 0; i < lodCount; ++i)
	{
		auto&         meshData     = mPrimitiveMeshData[i];
		auto&         subMeshes    = meshData->GetSubMesh();
		const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

		for (int32_t j = 0; j < subMeshCount; ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

			uint32_t offset = 0;

			mVertexSize = currMesh->GetVertexData()->GetVertexSize();

			// Topology 설정

			mMaterialInstances[j]->BindMaterial(deviceContext);

			int32_t indexNum = currMesh->GetVertexData()->IndexArray.size();

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0,
											  1,
											  mGeometryBuffer[j].Buffer_Vertex.GetAddressOf(),
											  &mVertexSize,
											  &offset);
			deviceContext->IASetIndexBuffer(mGeometryBuffer[j].Buffer_Index.Get(), DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}

void JMeshObject::DrawID(uint32_t ID)
{
	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	const int32_t lodCount = mPrimitiveMeshData.size();

	for (int32_t i = 0; i < lodCount; ++i)
	{
		auto&         instanceBuffer = mGeometryBuffer[i];
		auto&         meshData       = mPrimitiveMeshData[i];
		auto&         subMeshes      = meshData->GetSubMesh();
		const int32_t subMeshCount   = subMeshes.empty() ? 1 : subMeshes.size();

		for (int32_t j = 0; j < subMeshCount; ++j)
		{
			uint32_t offset = 0;
			int32_t  indexNum;

			// Topology 설정
			GetWorld.D3D11API->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex.GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index.Get(), DXGI_FORMAT_R32_UINT, 0);

			auto idShader = GetWorld.ShaderManager->IDShader;

			auto cam = GetWorld.CameraManager->GetCurrentMainCam();

			CBuffer::Camera camera;
			camera.CameraPos  = FVector4::ZeroVector;
			camera.View       = XMMatrixTranspose(cam->GetViewMatrix());
			camera.Projection = XMMatrixTranspose(cam->GetProjMatrix());

			idShader->UpdateConstantData(deviceContext,
										 CBuffer::NAME_CONSTANT_BUFFER_CAMERA,
										 &camera);

			FMatrix worldMatrix = XMMatrixTranspose(mWorldMatrix);
			idShader->UpdateConstantData(deviceContext,
										 "WorldConstantBuffer",
										 &worldMatrix);

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

			GetWorld.D3D11API->SetBlendState(EBlendState::Opaque);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}
