#pragma once
#include "JShader.h"
#include "Core/Manager/Manager_Base.h"

class JShader_Basic;

class MShaderManager : public Manager_Base<JShader, MShaderManager>
{
public:
	void UpdateCamera(const Ptr<class JCamera>& InCameraObj) const;

private:
	void Initialize_Initialize();

public:
	Ptr<JShader> BasicShader;
	Ptr<JShader> IDShader;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MShaderManager>;
	friend class MManagerInterface;

	MShaderManager();
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;
#pragma endregion
};
