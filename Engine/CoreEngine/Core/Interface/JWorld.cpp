﻿#include "JWorld.h"

#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Audio/MSoundManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Window/Application.h"
#include "GUI/MGUIManager.h"
#include "inifile_cpp/inicpp.h"
#include "Core/Entity/Navigation/NavTest.h"

extern ini::IniFile g_settings;

JWorld::JWorld()
	: GUIManager(),
	  ViewportManager(nullptr),
	  CameraManager(nullptr),
	  ShaderManager(nullptr),
	  TextureManager(nullptr),
	  MaterialManager(nullptr),
	  MeshManager(nullptr),
	  D3D11API(nullptr),
	  ThreadPool(std::thread::hardware_concurrency())
{}

void JWorld::Initialize()
{
	G_Logger.Initialize();			 // Logger

	Application             = Application::s_AppInstance;
	D3D11API                = &XD3DDevice::Get();
	ShaderManager           = &MShaderManager::Get();
	CameraManager           = &MCameraManager::Get();
	ViewportManager         = &MViewportManager::Get();
	TextureManager          = &MTextureManager::Get();
	MaterialManager         = &MMaterialManager::Get();
	MaterialInstanceManager = &MMaterialInstanceManager::Get();
	MeshManager             = &MMeshManager::Get();
	LevelManager            = &MLevelManager::Get();
	SoundManager            = &MSoundManager::Get();
	GUIManager              = &MGUIManager::Get();
	UIManager               = &MUIManager::Get();
	AnimationManager        = &MAnimManager::Get();
	ColliderManager         = &MCollisionManager::Get();

	MaterialManager->SaveEngineMaterials();
	GUIManager->Initialize(D3D11API->GetDevice(), D3D11API->GetImmediateDeviceContext());
	G_DebugBatch.Initialize();		 // Primitive Batch

	OnDebugModeChanged.Bind([](bool bDebugMode){});

	// ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/Tex)"));
	// ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/GameResource)"));
	// ThreadPool.ExecuteTask(&ParseFiles_Recursive, std::filesystem::path(R"(rsc)"));

	const char* levelPath = g_settings["Editor.Level"]["DefaultLevel"].as<const char*>();
	assert(levelPath[0] != '\0');

	LevelManager->SetActiveLevel(LevelManager->Load(levelPath));

	// std::uniform_real_distribution dist(-10000.0f, 10000.0f);
	// for (int i = 0; i < 1000; ++i)
	// {
	// 	std::mt19937 gen(i);
	// 	float x = dist(gen);
	// 	float y = dist(gen);
	//
	// 	auto* newActor = SpawnActor<JStaticMeshActor>("Sample",
	// 								 FVector{x, 0.0f, y},
	// 								 FVector::ZeroVector,
	// 								 nullptr,
	// 								 "Game/Mesh/SM_Couch_01.jasset");
	// }
}

void JWorld::Update(float DeltaTime)
{
	GUIManager->Update(DeltaTime);

	SoundManager->Update(DeltaTime);

	ColliderManager->UpdateCollision();

	LevelManager->Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);
	currentFrame++;
}

void JWorld::Render()
{
	GUIManager->Render();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Alpha);
	ShaderManager->UpdateCamera(GetWorld.CameraManager->GetCurrentMainCam());
	LevelManager->Render();
}

void JWorld::Release()
{
	G_DebugBatch.Release();

	SoundManager->Release();

	GUIManager->Release();

	D3D11API->Release();

	Utils::Fbx::FbxFile::Release();
}

float JWorld::GetDeltaSeconds() const
{
	return Application->GetDeltaSeconds();
}

float JWorld::GetGameTime() const
{
	return Application->GetCurrentTime();
}


void JWorld::SearchFiles_Recursive(const std::filesystem::path& InPath)
{
	if (exists(InPath) && is_directory(InPath))
	{
		for (const auto& entry : std::filesystem::directory_iterator(InPath))
		{
			if (entry.is_directory())
			{
				SearchFiles_Recursive(entry.path());
			}
			else if (entry.is_regular_file())
			{
				JText ext = entry.path().extension().string();
				// 해시 비교

				std::ranges::transform(ext,
									   ext.begin(),
									   [](unsigned char c){ return std::tolower(c); });

				const uint32_t hash = StringHash(ext.c_str());
				const JText    name = entry.path().string();

				switch (hash)
				{
				case HASH_EXT_PNG:
				case HASH_EXT_JPG:
				case HASH_EXT_DDS:
				case HASH_EXT_BMP:
					GetWorld.TextureManager->Load(name);
					break;
				case HASH_EXT_WAV:
				case HASH_EXT_MP3:
					GetWorld.SoundManager->Load(name);
					break;
				}
			}
		}
	}
}

void JWorld::ParseFiles_Recursive(const std::filesystem::path& InPath)
{
	if (exists(InPath) && is_directory(InPath))
	{
		for (const auto& entry : std::filesystem::directory_iterator(InPath))
		{
			if (entry.is_directory())
			{
				ParseFiles_Recursive(entry.path());
			}
			else if (entry.is_regular_file())
			{
				JText ext = entry.path().extension().string();
				// 해시 비교

				std::ranges::transform(ext,
									   ext.begin(),
									   [](unsigned char c){ return std::tolower(c); });

				const uint32_t hash = StringHash(ext.c_str());

				if (hash == Hash_EXT_FBX)
				{
					JText resultFile = entry.path().string();
					if (std::filesystem::exists(resultFile) && std::filesystem::is_regular_file(resultFile))
					{
						// Import Fbx
						Utils::Fbx::FbxFile fbxFile;
						if (!fbxFile.Load(resultFile.c_str()))
						{
							LOG_CORE_INFO("Fbx File Failed: {}", resultFile);
						}
					}
				}
			}
		}
	}
}
