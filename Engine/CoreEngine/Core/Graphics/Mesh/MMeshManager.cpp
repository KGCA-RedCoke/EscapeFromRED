﻿#include "MMeshManager.h"

#include "JSkeletalMeshObject.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/JShader_Shadow.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"

MMeshManager::MMeshManager()
{}

MMeshManager::~MMeshManager() {}

void MMeshManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								  void*        Entity)
{
	if (JMeshObject* meshObject = static_cast<JMeshObject*>(Entity))
	{
		if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
		{
			const bool bIsSkeletalMesh = meshObject->GetType() == HASH_ASSET_TYPE_SKELETAL_MESH;
			Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(),
											  bIsSkeletalMesh
												  ? dynamic_cast<JSkeletalMeshObject*>(meshObject)
												  : meshObject);

			LOG_CORE_INFO("<Type: {}> Post Initialize: {}", bIsSkeletalMesh ? "SkeletalMesh" : "Mesh", ParsedName);

			CreateBuffers(meshObject);
		}
		else if (!meshObject->mPrimitiveMeshData.empty())
		{
			CreateBuffers(meshObject);
		}

	}
}

void MMeshManager::CreateBuffers(JMeshObject* MeshObject)
{
	auto* device = G_Device.GetDevice();

	GenBuffer(device, MeshObject);
}

void MMeshManager::GenBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject)
{
	// 메시가 여러개인 경우(파싱을 했는데 메시가 여러개로 나뉜)는 거의 LOD일 것임 (언리얼에서 가져올 경우)
	const int32_t lodCount = MeshObject->mPrimitiveMeshData.size();

	MeshObject->mGeometryBuffer.clear();

	const uint32_t assetTypeHash = MeshObject->GetType();
	const bool     bSkeletal     = assetTypeHash == HASH_ASSET_TYPE_SKELETAL_MESH;

	// LOD별로 버퍼를 생성한다.
	for (int32_t i = 0; i < lodCount; ++i)
	{
		const Ptr<JMeshData>&         mesh         = MeshObject->mPrimitiveMeshData[i];	// 현재 LOD메시
		const JArray<Ptr<JMeshData>>& subMeshes    = mesh->GetSubMesh();					// 현재 LOD메시의 서브메시
		const bool                    bHasSubMesh  = !subMeshes.empty();
		const int32_t                 subMeshCount = bHasSubMesh ? subMeshes.size() : 1;

		// 1. 메시 데이터 하나마다 따로 버퍼를 생성한다. (서브메시가 오브젝트에서 떨어져 따로 분리 될 가능성이 있음)
		for (int32_t j = 0; j < subMeshCount; ++j)
		{
			Buffer::FBufferGeometry geometryBuffer;
			Buffer::FBufferInstance instanceBuffer;

			const auto&    currentMesh     = bHasSubMesh ? subMeshes[j] : mesh;
			const auto&    currentMeshData = currentMesh->GetVertexData();
			const uint32_t meshHash        = currentMesh->GetHash();

			if (auto it = mBufferList.find(meshHash); it != mBufferList.end())
			{
				MeshObject->mGeometryBuffer.push_back(geometryBuffer);
				continue;
			}

			// Vertex 버퍼 생성
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_VERTEX_BUFFER,
									reinterpret_cast<void**>(currentMeshData->VertexArray.data()),
									sizeof(Vertex::FVertexInfo_Base),
									currentMeshData->VertexArray.size(),
									geometryBuffer.Buffer_Vertex.GetAddressOf());

			// Instance 버퍼 생성
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_VERTEX_BUFFER,
									nullptr,
									sizeof(FInstanceData_Mesh),
									MAX_INSTANCE,
									instanceBuffer.Buffer_Instance.GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);

			if (bSkeletal)
			{
				Utils::DX::CreateBuffer(InDevice,
										D3D11_BIND_VERTEX_BUFFER,
										nullptr,
										sizeof(FSkeletalMeshInstanceData),
										MAX_INSTANCE,
										instanceBuffer.Buffer_Bone.GetAddressOf(),
										D3D11_USAGE_DYNAMIC,
										D3D11_CPU_ACCESS_WRITE);
			}

			// Index 버퍼 생성
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_INDEX_BUFFER,
									reinterpret_cast<void**>(currentMeshData->IndexArray.data()),
									SIZE_INDEX_BUFFER,
									currentMeshData->IndexArray.size(),
									geometryBuffer.Buffer_Index.GetAddressOf());

			// 이미 존재하면 덮어쓰기
			MeshObject->mGeometryBuffer.push_back(geometryBuffer);
			mBufferList[meshHash].Geometry   = geometryBuffer;
			mBufferList[meshHash].Instance   = instanceBuffer;
			mBufferList[meshHash].IndexCount = currentMeshData->IndexArray.size();
		}
	}
}

void MMeshManager::PushCommand(uint32_t                    NameHash, JMaterialInstance* InMaterialRef,
							   JArray<FInstanceData_Mesh>& InInstanceData)
{
	assert(mBufferList.contains(NameHash));

	mInstanceData_Mesh[NameHash][InMaterialRef].insert(mInstanceData_Mesh[NameHash][InMaterialRef].end(),
													   InInstanceData.begin(),
													   InInstanceData.end());
}

void MMeshManager::PushCommand(uint32_t                   NameHash, JMaterialInstance* InMaterialRef,
							   FInstanceData_Mesh&        InInstanceData,
							   ID3D11ShaderResourceView** InAnimTexture)
{
	assert(mBufferList.contains(NameHash));

	InInstanceData.Transform.ShadowMatrix = InInstanceData.Transform.WorldMatrix * GetWorld.
			DirectionalLightView * GetWorld
			.DirectionalLightProj * FMatrix(0.5f,
											0.0f,
											0.0f,
											0.0f,
											0.0f,
											-0.5f,
											0.0f,
											0.0f,
											0.0f,
											0.0f,
											1.0f,
											0.0f,
											0.5f,
											0.5f,
											0.0f,
											1.0f);

	mInstanceData_Mesh[NameHash][InMaterialRef].emplace_back(InInstanceData);


	if (InAnimTexture)
	{
		mInstanceData_Skeletal[NameHash] = InAnimTexture;
	}
}

void MMeshManager::FlushCommandList_Shadow(ID3D11DeviceContext* InContext)
{

	for (auto& [nameHash, instanceByMaterial] : mInstanceData_Mesh)
	{
		auto it = mBufferList.find(nameHash);
		// 인스턴스 갯수 만큼 버퍼 업데이트
		if (it == mBufferList.end())
		{
			return;
		}

		Buffer::FBufferMesh bufferMesh     = it->second;
		auto&               instanceBuffer = bufferMesh.Instance.Buffer_Instance;

		for (auto& [material, instanceData] : instanceByMaterial)
		{
			GetWorld.ShaderManager->ShadowMapShader->BindShaderPipeline(InContext);

			Utils::DX::UpdateDynamicBuffer(InContext,
										   instanceBuffer.Get(),
										   instanceData.data(),
										   sizeof(FInstanceData_Mesh) * instanceData.size());

			ID3D11Buffer*      buffers[2] = {bufferMesh.Geometry.Buffer_Vertex.Get(), instanceBuffer.Get()};
			constexpr uint32_t stride[2]  = {sizeof(Vertex::FVertexInfo_Base), sizeof(FInstanceData_Mesh)};
			constexpr uint32_t offset[2]  = {0, 0};

			InContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
			InContext->IASetIndexBuffer(bufferMesh.Geometry.Buffer_Index.Get(), DXGI_FORMAT_R32_UINT, 0);
			InContext->DrawIndexedInstanced(bufferMesh.IndexCount, instanceData.size(), 0, 0, 0);
		}
	}

	mInstanceData_Mesh.clear();
	mInstanceData_Skeletal.clear();
}

void MMeshManager::FlushCommandList(ID3D11DeviceContext* InContext)
{
	for (auto& [nameHash, instanceByMaterial] : mInstanceData_Mesh)
	{
		const bool bSkeletal = mBufferList[nameHash].Instance.Buffer_Bone.Get() != nullptr;

		auto it = mBufferList.find(nameHash);
		// 인스턴스 갯수 만큼 버퍼 업데이트
		if (it == mBufferList.end())
		{
			return;
		}

		Buffer::FBufferMesh bufferMesh     = it->second;
		auto&               instanceBuffer = bufferMesh.Instance.Buffer_Instance;

		for (auto& [material, instanceData] : instanceByMaterial)
		{
			material->BindMaterial(InContext);
			Utils::DX::UpdateDynamicBuffer(InContext,
										   instanceBuffer.Get(),
										   instanceData.data(),
										   sizeof(FInstanceData_Mesh) * instanceData.size());

			if (bSkeletal && mInstanceData_Skeletal[nameHash])
			{
				InContext->VSSetShaderResources(SLOT_SKINNING, 1, mInstanceData_Skeletal[nameHash]);
			}

			// InContext->PSSetShaderResources(10, 1, GetWorld.DirectionalLightShadowMap->DepthSRV.GetAddressOf());


			ID3D11Buffer*      buffers[2] = {bufferMesh.Geometry.Buffer_Vertex.Get(), instanceBuffer.Get()};
			constexpr uint32_t stride[2]  = {sizeof(Vertex::FVertexInfo_Base), sizeof(FInstanceData_Mesh)};
			constexpr uint32_t offset[2]  = {0, 0};

			InContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
			InContext->IASetIndexBuffer(bufferMesh.Geometry.Buffer_Index.Get(), DXGI_FORMAT_R32_UINT, 0);
			InContext->DrawIndexedInstanced(bufferMesh.IndexCount, instanceData.size(), 0, 0, 0);
		}
	}

	mInstanceData_Mesh.clear();
	mInstanceData_Skeletal.clear();

}

// void MMeshManager::GenChunkMeshes(JMeshObject*   MeshObject, uint32_t InstanceCount, const FVector& Position,
// 								  const FVector& Scale)
// {
// 	if (!mChunkList.empty())
// 	{
// 		return;
// 	}
// 	mCornMaterial = MeshObject->mMaterialInstances[0];
//
// 	// 메시가 여러개인 경우(파싱을 했는데 메시가 여러개로 나뉜)는 거의 LOD일 것임 (언리얼에서 가져올 경우)
// 	const int32_t lodCount = MeshObject->mPrimitiveMeshData.size();
// 	int32_t       lodIndex = 0;
//
// 	const Ptr<JMeshData>&         mesh         = MeshObject->mPrimitiveMeshData[lodIndex];	// 현재 LOD메시
// 	const JArray<Ptr<JMeshData>>& subMeshes    = mesh->GetSubMesh();					// 현재 LOD메시의 서브메시
// 	const bool                    bHasSubMesh  = !subMeshes.empty();
// 	const int32_t                 subMeshCount = bHasSubMesh ? subMeshes.size() : 1;
// 	mChunkList.resize(subMeshCount);
//
// 	// 1. 메시 데이터 하나마다 따로 버퍼를 생성한다. (서브메시가 오브젝트에서 떨어져 따로 분리 될 가능성이 있음)
// 	for (int32_t j = 0; j < subMeshCount; ++j)
// 	{
// 		Buffer::FBufferGeometry geometryBuffer;
// 		Buffer::FBufferInstance instanceBuffer;
//
// 		const auto&    currentMesh     = bHasSubMesh ? subMeshes[j] : mesh;
// 		const auto&    currentMeshData = currentMesh->GetVertexData();
// 		const uint32_t hash            = currentMesh->GetHash();
//
// 		auto it = mBufferList.find(hash);
// 		if (it != mBufferList.end())
// 		{}
//
//
// 		// Vertex 버퍼 생성
// 		Utils::DX::CreateBuffer(G_Device.GetDevice(),
// 								D3D11_BIND_VERTEX_BUFFER,
// 								reinterpret_cast<void**>(currentMeshData->VertexArray.data()),
// 								sizeof(Vertex::FVertexInfo_Base),
// 								currentMeshData->VertexArray.size(),
// 								geometryBuffer.Buffer_Vertex.GetAddressOf());
//
// 		// Instance 버퍼 생성
// 		Utils::DX::CreateBuffer(G_Device.GetDevice(),
// 								D3D11_BIND_VERTEX_BUFFER,
// 								nullptr,
// 								sizeof(FInstanceData_Mesh),
// 								MAX_INSTANCE,
// 								instanceBuffer.Buffer_Instance.GetAddressOf(),
// 								D3D11_USAGE_DYNAMIC,
// 								D3D11_CPU_ACCESS_WRITE);
//
// 		// Index 버퍼 생성
// 		Utils::DX::CreateBuffer(G_Device.GetDevice(),
// 								D3D11_BIND_INDEX_BUFFER,
// 								reinterpret_cast<void**>(currentMeshData->IndexArray.data()),
// 								SIZE_INDEX_BUFFER,
// 								currentMeshData->IndexArray.size(),
// 								geometryBuffer.Buffer_Index.GetAddressOf());
//
// 		// 이미 존재하면 덮어쓰기
// 		mChunkList[j].Geometry   = geometryBuffer;
// 		mChunkList[j].Instance   = instanceBuffer;
// 		mChunkList[j].IndexCount = currentMeshData->IndexArray.size();
//
// 	}
//
// 	for (int32_t i = 0; i < InstanceCount; ++i)
// 	{
// 		std::uniform_real_distribution dist(-3000.0f, 3000.0f);
// 		std::mt19937                   gen(i);
// 		float                          x = Position.x + dist(gen);
// 		float                          z = Position.z + dist(gen);
//
// 		auto worldLoc = XMMatrixTranslation(x, 0.f, z);
// 		auto worldScl = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
//
// 		auto worldMat      = worldScl * worldLoc;
// 		auto worldInvTrans = XMMatrixTranspose(XMMatrixInverse(nullptr, worldMat));
//
// 		mCornMeshes.emplace_back(
// 								 FInstanceData_Mesh{
// 									 worldMat,
// 									 worldInvTrans
// 								 });
// 	}
// }
//
// void MMeshManager::FlushChunkMeshes(ID3D11DeviceContext* InContext)
// {
// 	mCornMaterial->BindMaterial(InContext);
//
// 	for (auto& corn : mChunkList)
// 	{
// 		auto& instanceBuffer = corn.Instance.Buffer_Instance;
//
// 		Utils::DX::UpdateDynamicBuffer(InContext,
// 									   instanceBuffer.Get(),
// 									   mCornMeshes.data(),
// 									   sizeof(FInstanceData_Mesh) * mCornMeshes.size());
//
// 		ID3D11Buffer*      buffers[2] = {corn.Geometry.Buffer_Vertex.Get(), instanceBuffer.Get()};
// 		constexpr uint32_t stride[2]  = {sizeof(Vertex::FVertexInfo_Base), sizeof(FInstanceData_Mesh)};
// 		constexpr uint32_t offset[2]  = {0, 0};
//
// 		InContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
// 		InContext->IASetIndexBuffer(corn.Geometry.Buffer_Index.Get(), DXGI_FORMAT_R32_UINT, 0);
// 		InContext->DrawIndexedInstanced(corn.IndexCount, mCornMeshes.size(), 0, 0, 0);
// 	}
//
//
// }
