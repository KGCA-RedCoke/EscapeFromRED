#pragma once
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Layer/XLayerBase.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Thread/ThreadPool.h"
#include "GUI/MGUIManager.h"

#define IManager MManagerInterface::Get()

class MManagerInterface : public TSingleton<MManagerInterface>
{
public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	MGUIManager      GUIManager;		// GUI
	MViewportManager ViewportManager;	// 뷰포트
	MCameraManager   CameraManager;		// 카메라
	MShaderManager   ShaderManager;		// 셰이더
	MTextureManager  TextureManager;	// 텍스처
	MMaterialManager MaterialManager;	// 머티리얼

	MLayerManager LayerManager;			// 레이어

	Thread::ThreadPool ThreadPool;		// 스레드


#pragma region Singleton Boilerplate

public:
	friend class TSingleton<MManagerInterface>;

	MManagerInterface();

public:
	MManagerInterface(const MManagerInterface&)            = delete;
	MManagerInterface& operator=(const MManagerInterface&) = delete;

#pragma endregion
};
