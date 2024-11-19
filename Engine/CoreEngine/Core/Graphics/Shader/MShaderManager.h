#pragma once
#include "JShader.h"
#include "Core/Manager/Manager_Base.h"

class JShader_Basic;

class MShaderManager : public Manager_Base<JShader, MShaderManager>
{
public:
	void UpdateCamera(class JCamera* InCameraObj) const;

private:
	void Initialize_Initialize();

public:
	JShader* BasicShader;
	JShader* IDShader;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MShaderManager>;
	friend class JWorld;

	MShaderManager();
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;
#pragma endregion
};
