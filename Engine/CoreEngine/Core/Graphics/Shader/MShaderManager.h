#pragma once
#include "JShader.h"
#include "Core/Manager/Manager_Base.h"

class MShaderManager : public Manager_Base<JShader, MShaderManager>
{
public:
	void Initialize();
	
private:
	friend class TSingleton<MShaderManager>;
	friend class MManagerInterface;

	MShaderManager();
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;

};
