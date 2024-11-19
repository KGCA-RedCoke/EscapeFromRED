#pragma once
#include "JMeshObject.h"
#include "Core/Manager/Manager_Base.h"

#define MAX_INSTANCE 1000

class MMeshManager : public Manager_Base<class JMeshObject, MMeshManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;
	void CreateBuffers(JMeshObject* MeshObject);

	void GenInstanceBuffer(ID3D11Device* InDevice, uint32_t NameHash, JArray<void*>& InstanceData);
	void GenGeometryBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject);
	void GenBoneBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject);

	void UpdateInstanceBuffer(uint32_t NameHash, int32_t NumInstances);


	void PushCommand(uint32_t NameHash, const FMatrix& InWorldMatrix);
	void FlushCommandList(ID3D11DeviceContext* InContext);

private:
	JHash<uint32_t, Buffer::FBufferMesh> mBufferList;

	JHash<uint32_t, JMaterialInstance*>    mMaterialInstance;
	JHash<uint32_t, JArray<FInstanceData>> mInstanceData;

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
