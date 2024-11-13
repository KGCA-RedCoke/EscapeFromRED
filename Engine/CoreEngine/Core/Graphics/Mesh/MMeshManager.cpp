#include "MMeshManager.h"

#include "Core/Graphics/XD3DDevice.h"

void MMeshManager::PostInitialize(const JText& ParsedName, const uint32_t NameHash, void* Entity)
{
	LOG_CORE_INFO("CALLED: Mesh Post Initialize: {0}", ParsedName);

	if (mMeshList.contains(NameHash))
	{
		LOG_CORE_ERROR("이미 존재하는 메시가 중복 호출됨: {0}", ParsedName);
		return;
	}

	assert(mManagedList.contains(NameHash));

	if (JMeshObject* meshObject = mManagedList[NameHash].get())
	{
		CreateBuffers(NameHash, meshObject);
	}
}

void MMeshManager::CreateBuffers(const uint32_t NameHash, const JMeshObject* MeshObject)
{
	ID3D11Device* device = Renderer.GetDevice();
	assert(device);

	// 키 - 값 이 없으면 하나 만들어주고 이미 있다면 내부 배열 초기화
	mMeshList[NameHash].clear();

	JArray<Buffer::FCombinedBuffer>& combinedBuffers = mMeshList[NameHash];

	const int32_t meshDataSize = MeshObject->mPrimitiveMeshData.size();

	combinedBuffers.resize(meshDataSize);
	for (int32_t i = 0; i < meshDataSize; ++i)	// LOD가 있으면 뻑날 것임 (LOD는 아직 구현 안함)
	{
		auto& mesh      = MeshObject->mPrimitiveMeshData[i];
		auto& subMeshes = mesh->GetSubMesh();

		combinedBuffers[i].BufferGeometry.Resize(subMeshes.empty() ? 1 : subMeshes.size());

		for (int32_t j = 0; j < meshDataSize; ++j)
		{
			auto& data = subMeshes.empty()
							 ? mesh->GetVertexData()
							 : subMeshes[j]->GetVertexData();

			

			// Vertex 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_VERTEX_BUFFER,
									reinterpret_cast<void**>(&data->VertexArray.at(0)),
									MeshObject->mVertexSize,
									data->VertexArray.size(),
									combinedBuffers[j].BufferGeometry.Buffer_Vertex[j].GetAddressOf());

			// Index 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_INDEX_BUFFER,
									reinterpret_cast<void**>(&data->IndexArray.at(0)),
									SIZE_INDEX_BUFFER,
									data->IndexArray.size(),
									combinedBuffers[j].BufferGeometry.Buffer_Index[j].GetAddressOf());
		}
	}

}

MMeshManager::MMeshManager() {}
MMeshManager::~MMeshManager() {}
