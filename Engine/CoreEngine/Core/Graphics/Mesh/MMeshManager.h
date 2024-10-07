#pragma once
#include "JMeshObject.h"
#include "Core/Manager/Manager_Base.h"

class MMeshManager : public Manager_Base<class JMeshObject, MMeshManager>
{
public:
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
