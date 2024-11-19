#include "MMeshManager.h"

#include "JSkeletalMeshObject.h"
#include "Core/Graphics/XD3DDevice.h"

void MMeshManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								  void*        Entity)
{
	if (JMeshObject* meshObject = static_cast<JMeshObject*>(Entity))
	{
		if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
		{
			LOG_CORE_INFO("<Type: Mesh> Post Initialize: {0}", ParsedName);

			JAssetMetaData metaData = Utils::Serialization::GetType(OriginalNameOrPath.c_str());

			if (metaData.AssetType == HASH_ASSET_TYPE_SKELETAL_MESH)
			{
				Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(),
												  dynamic_cast<JSkeletalMeshObject*>(meshObject));
			}
			else
			{
				Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(),
												  meshObject);
			}

			CreateBuffers(meshObject);
		}

	}
}

void MMeshManager::CreateBuffers(JMeshObject* MeshObject)
{
	if (MeshObject)
	{
		auto* device = G_Device.GetDevice();

		GenGeometryBuffer(device, MeshObject);
		uint32_t assetTypeHash = MeshObject->GetType();
		if (assetTypeHash == HASH_ASSET_TYPE_SKELETAL_MESH)
		{
			GenBoneBuffer(device, MeshObject);
		}
	}
}

void MMeshManager::GenGeometryBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject)
{
	// 메시가 여러개인 경우는 거의 LOD일 것임
	const int32_t lodCount = MeshObject->mPrimitiveMeshData.size();

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

			const auto&    currentMesh     = bHasSubMesh ? subMeshes[j] : mesh;
			const auto&    currentMeshData = currentMesh->GetVertexData();
			const uint32_t meshHash        = currentMesh->GetHash();

			// Vertex 버퍼 생성
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_VERTEX_BUFFER,
									reinterpret_cast<void**>(currentMeshData->VertexArray.data()),
									sizeof(Vertex::FVertexInfo_Base)/*MeshObject->mVertexSize*/,
									currentMeshData->VertexArray.size(),
									geometryBuffer.Buffer_Vertex.GetAddressOf());

			// Index 버퍼 생성
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_INDEX_BUFFER,
									reinterpret_cast<void**>(currentMeshData->IndexArray.data()),
									SIZE_INDEX_BUFFER,
									currentMeshData->IndexArray.size(),
									geometryBuffer.Buffer_Index.GetAddressOf());

			// 이미 존재하면 덮어쓰기
			MeshObject->mGeometryBuffer.push_back(geometryBuffer);
			mBufferList.insert_or_assign(meshHash, geometryBuffer);
		}
	}
}

void MMeshManager::GenBoneBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject)
{
	auto& bone         = mBoneBufferList[MeshObject->GetHash()];
	bone.Buffer_Bone   = nullptr;
	bone.Resource_Bone = nullptr;

	// Bone 버퍼 생성
	Utils::DX::CreateBuffer(InDevice,
							D3D11_BIND_SHADER_RESOURCE,
							nullptr,
							sizeof(FMatrix),
							SIZE_MAX_BONE_NUM,
							bone.Buffer_Bone.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);

	D3D11_SHADER_RESOURCE_VIEW_DESC boneSRVDesc;
	ZeroMemory(&boneSRVDesc, sizeof(boneSRVDesc));
	{
		boneSRVDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;	// 4x4 Matrix
		boneSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;		// Buffer
		boneSRVDesc.Buffer.ElementOffset = 0;
		boneSRVDesc.Buffer.ElementWidth  = SIZE_MAX_BONE_NUM * 4;
		CheckResult(InDevice->CreateShaderResourceView(bone.Buffer_Bone.Get(),
													   &boneSRVDesc,
													   bone.Resource_Bone.GetAddressOf()));
	}

	auto* skel = dynamic_cast<JSkeletalMeshObject*>(MeshObject);
	assert(skel);

	skel->mInstanceBuffer_Bone = bone;
	mBoneBufferList.insert_or_assign(MeshObject->GetHash(), bone);
}


void MMeshManager::GenInstanceBuffer(ID3D11Device*  InDevice, uint32_t NameHash,
										JArray<void*>& InstanceData)
{
	assert(mBufferList.contains(NameHash));

	// mBufferList[NameHash].BufferInstance.Buffer_Instance.Reset();
	//
	// JArray<FMatrix> instanceDataMatrix;
	// instanceDataMatrix.reserve(InstanceData.size());
	// for (auto& data : InstanceData)
	// {
	// 	instanceDataMatrix.push_back(XMMatrixTranspose(*static_cast<FMatrix*>(data)));
	// }
	//
	// Utils::DX::CreateBuffer(InDevice,
	// 						D3D11_BIND_VERTEX_BUFFER,
	// 						reinterpret_cast<void**>(instanceDataMatrix.data()),
	// 						sizeof(FMatrix),
	// 						InstanceData.size(),
	// 						mBufferList[NameHash].BufferInstance.Buffer_Instance.GetAddressOf(),
	// 						D3D11_USAGE_DYNAMIC,
	// 						D3D11_CPU_ACCESS_WRITE);
}

MMeshManager::MMeshManager()
{
	// CreateOrLoad(Path_Mesh_Sphere);
}

MMeshManager::~MMeshManager() {}
