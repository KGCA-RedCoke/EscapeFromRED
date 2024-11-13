#pragma once
#include "JMeshObject.h"
#include "Core/Manager/Manager_Base.h"

class MMeshManager : public Manager_Base<class JMeshObject, MMeshManager>
{
private:
	void PostInitialize(const JText& ParsedName, const uint32_t NameHash, void* Entity) override;

	void CreateBuffers(const uint32_t NameHash, const JMeshObject* MeshObject);

private:
	/** 앞으로 버퍼는 매니저에서 관리한다. (공유 자원) */
	JHash<uint32_t, JArray<Buffer::FCombinedBuffer>> mMeshList;
	friend class JMeshObject;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMeshManager>;
	friend class MManagerInterface;

	MMeshManager();
	~MMeshManager();

public:
	MMeshManager(const MMeshManager&)            = delete;
	MMeshManager& operator=(const MMeshManager&) = delete;

#pragma endregion
};
