#pragma once
#include "JMeshObject.h"
#include "Core/Manager/Manager_Base.h"

class MMeshManager : public Manager_Base<class JMeshObject, MMeshManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;
	void CreateBuffers(JMeshObject* MeshObject);
	
	void GenInstanceBuffer(ID3D11Device* InDevice, uint32_t NameHash, JArray<void*>& InstanceData);
	void GenGeometryBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject);
	void GenBoneBuffer(ID3D11Device* InDevice, JMeshObject* MeshObject);

public:
	Buffer::FBufferGeometry* GetBuffer(uint32_t NameHash)
	{
		if (mBufferList.contains(NameHash))
		{
			return &mBufferList[NameHash];
		}

		return nullptr;
	}

private:
	JHash<uint32_t, Buffer::FBufferGeometry> mBufferList;
	JHash<uint32_t, Buffer::FBufferBone>     mBoneBufferList;

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
