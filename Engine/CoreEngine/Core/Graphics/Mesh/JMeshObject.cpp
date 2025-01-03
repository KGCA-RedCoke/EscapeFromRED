#include "JMeshObject.h"

#include "MMeshManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Interface/JWorld.h"

JMeshObject::JMeshObject(const JText& InAssetPath, const JArray<Ptr<JMeshData>>& InData)
	: mName(ParseFile(InAssetPath))
{
	mPath = InAssetPath;
	mName = ParseFile(mPath);

	mPrimitiveMeshData = InData;
}

JMeshObject::JMeshObject(const JMeshObject& Other)
	: mName(Other.mName),
	  mGeometryBuffer(Other.mGeometryBuffer),
	  mVertexSize(Other.mVertexSize),
	  mBoundingBox(Other.mBoundingBox)
{
	mPath              = Other.mPath;
	mPrimitiveMeshData = Other.mPrimitiveMeshData;
	mInstanceData      = Other.mInstanceData;

	mMaterialInstances.resize(Other.mMaterialInstances.size());
	for (int32_t i = 0; i < Other.mMaterialInstances.size(); ++i)
	{
		JMeshObject::SetMaterialInstance(Other.mMaterialInstances[i], i);
	}
}

JMeshObject::~JMeshObject()
{
	mPrimitiveMeshData.clear();
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

	// Bounding Box
	Utils::Serialization::Serialize_Primitive(&mBoundingBox, sizeof(mBoundingBox), FileStream);

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

	mMaterialInstances.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	mInstanceData.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetSubMaterialNum(); ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
		auto* matInstance = GetWorld.MaterialInstanceManager->Load(materialPath);

		SetMaterialInstance(matInstance, i);
	}

	// Bounding Box
	Utils::Serialization::DeSerialize_Primitive(&mBoundingBox, sizeof(mBoundingBox), InFileStream);

	return true;
}

void JMeshObject::UpdateInstance_Transform(const FMatrix& InWorldMatrix)
{
	for (int32_t i = 0; i < mInstanceData.size(); ++i)
	{
		mInstanceData[i].Transform.WorldMatrix           = InWorldMatrix;
		mInstanceData[i].Transform.WorldInverseTranspose = InWorldMatrix.Invert().Transpose();
	}
}

void JMeshObject::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex)
{
	if (InIndex > mMaterialInstances.size())
	{
		LOG_CORE_WARN("머티리얼 슬롯 인덱스가 범위를 벗어남.");
		return;
	}
	mMaterialInstances[InIndex] = InMaterialInstance;

	// // 이전에 바인딩된 Delegate 해제
	// if (mDelegateIDs.contains(InIndex))
	// {
	// 	InMaterialInstance->OnMaterialInstanceParamChanged.UnBind(mDelegateIDs[InIndex]);
	// 	mDelegateIDs.erase(InIndex);
	// }

	// const size_t delegateID = InMaterialInstance->OnMaterialInstanceParamChanged.Bind([InMaterialInstance, InIndex, this]{
	// 	if (InMaterialInstance)
	// 	{
	// 		const int32_t size = InMaterialInstance->GetMaterialSize();
	//
	// 		if (size > 0)
	// 		{
	// 			memcpy_s(&mInstanceData[InIndex].MaterialData,
	// 					 InMaterialInstance->GetMaterialSize(),
	// 					 InMaterialInstance->GetMaterialData(),
	// 					 size);
	//
	// 		}
	// 	}
	// });

	// ID 저장
	// mDelegateIDs[InIndex] = delegateID;

	// const int32_t size = InMaterialInstance->GetMaterialSize();

	// if (size > 0)
	// {
	// 	memcpy_s(&mInstanceData[InIndex].MaterialData,
	// 			 InMaterialInstance->GetMaterialSize(),
	// 			 InMaterialInstance->GetMaterialData(),
	// 			 size);
	// }
}

int32_t JMeshObject::GetMaterialCount() const
{
	return mMaterialInstances.size();
}

JMaterialInstance* JMeshObject::GetMaterialInstance(uint32_t InIndex) const
{
	if (InIndex >= mMaterialInstances.size())
	{
		LOG_CORE_WARN("머티리얼 슬롯 인덱스가 범위를 벗어남.");
		return nullptr;
	}

	return mMaterialInstances[InIndex];
}

void JMeshObject::AddInstance(float InCameraDistance)
{
	if (bChunkMesh)
	{
		MMeshManager::Get().GenChunkMeshes(this, 1000, FVector::ZeroVector, FVector::OneVector);
		MMeshManager::Get().FlushChunkMeshes(G_Device.GetImmediateDeviceContext());
		return;
	}

	const int32_t lodCount = mPrimitiveMeshData.size();
	int32_t       lod      = static_cast<int32_t>(InCameraDistance / 3000.f);
	lod                    = FMath::Clamp<int32_t>(lod, 0, lodCount - 1);

	auto&         meshData     = mPrimitiveMeshData[lod];
	auto&         subMeshes    = meshData->GetSubMesh();
	const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
		GetWorld.MeshManager->PushCommand(currMesh->GetHash(), mMaterialInstances[j], mInstanceData[j]);
	}
}

void JMeshObject::Draw()
{
	auto&         meshData     = mPrimitiveMeshData[0];
	auto&         subMeshes    = meshData->GetSubMesh();
	const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		mInstanceData[j].Transform.WorldMatrix           = FMatrix::Identity;
		mInstanceData[j].Transform.WorldInverseTranspose = FMatrix::Identity;
		auto& currMesh                                   = subMeshes.empty() ? meshData : subMeshes[j];
		GetWorld.MeshManager->PushCommand(currMesh->GetHash(), mMaterialInstances[j], mInstanceData[j]);
	}

	GetWorld.MeshManager->FlushCommandList(G_Device.GetImmediateDeviceContext());
}

void JMeshObject::DrawID(uint32_t ID)
{
	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);


	auto&         meshData     = mPrimitiveMeshData[0];
	auto&         subMeshes    = meshData->GetSubMesh();
	const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		uint32_t offset = 0;
		int32_t  indexNum;
		auto&    instanceBuffer = mGeometryBuffer[j];

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

		FMatrix worldMatrix = XMMatrixTranspose(mInstanceData[j].Transform.WorldMatrix);
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
