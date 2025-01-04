#pragma once
#include "JMeshObject.h"
#include "Core/Manager/Manager_Base.h"

#define MAX_INSTANCE 5000

class MMeshManager : public Manager_Base<class JMeshObject, MMeshManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;
	void CreateBuffers(JMeshObject* MeshObject);

	void GenBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject);

	void PushCommand(uint32_t NameHash, JMaterialInstance* InMaterialRef, JArray<FInstanceData_Mesh>& InInstanceData);
	void PushCommand(uint32_t NameHash, JMaterialInstance* InMaterialRef, FInstanceData_Mesh& InInstanceData,
					 ID3D11ShaderResourceView** InAnimTexture = nullptr);

	void FlushCommandList_Shadow(ID3D11DeviceContext* InContext);

	void FlushCommandList(ID3D11DeviceContext* InContext);
	//
	// void GenChunkMeshes(JMeshObject* MeshObject, uint32_t InstanceCount, const FVector& Position, const FVector& Scale);
	// void FlushChunkMeshes(ID3D11DeviceContext* InContext);

private:
	// -------------- Single Mesh --------------
	JHash<uint32_t, Buffer::FBufferMesh>                                   mBufferList;
	JHash<uint32_t, JHash<JMaterialInstance*, JArray<FInstanceData_Mesh>>> mInstanceData_Mesh;
	JHash<uint32_t, ID3D11ShaderResourceView**>                            mInstanceData_Skeletal;


	// -------------- Chunk Mesh --------------
	JHash<uint32_t, Buffer::FBufferMesh> mChunkList;
	JArray<FInstanceData_Mesh>           mCornMeshes;
	JMaterialInstance*                   mCornMaterial = nullptr;


#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMeshManager>;
	friend class JWorld;

	MMeshManager();
	~MMeshManager();

public:
	MMeshManager(const MMeshManager&)            = delete;
	MMeshManager& operator=(const MMeshManager&) = delete;

#pragma endregion
};
