#pragma once
#include "JShader.h"
#include "Core/Manager/Manager_Base.h"

class SFXAAEffect;

class MShaderManager : public Manager_Base<JShader, MShaderManager>
{
public:
	void Initialize();

public:
	JShader* GetBasicShader();

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MShaderManager>;
	friend class MManagerInterface;

	MShaderManager()  = default;
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;
#pragma endregion
};
