#include "JWorld.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Actor/MActorManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Utils/SpaceDivision/SpaceDivision.h"
#include "Core/Window/Application.h"
#include "GUI/MGUIManager.h"
#include "inifile_cpp/inicpp.h"

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
	Application = Application::s_AppInstance;

	G_Logger.Initialize();			 // Logger

	D3D11API = &XD3DDevice::Get();

	ShaderManager = &MShaderManager::Get();

	CameraManager = &MCameraManager::Get();

	ViewportManager = &MViewportManager::Get();

	TextureManager = &MTextureManager::Get();

	MaterialManager = &MMaterialManager::Get();
	MaterialManager->SaveEngineMaterials();

	MaterialInstanceManager = &MMaterialInstanceManager::Get();

	MeshManager = &MMeshManager::Get();

	ActorManager = &MActorManager::Get();

	LevelManager = &MLevelManager::Get();

	GUIManager = &MGUIManager::Get();
	GUIManager->Initialize(D3D11API->GetDevice(), D3D11API->GetImmediateDeviceContext());

	G_DebugBatch.Initialize();		 // Primitive Batch

	Oc::JTree tree;
	tree.Initialize({{0, 0, 0}, {1000, 1000, 1000}}, 5);

	ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/Tex)"));
	// ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/TT)"));
	// ThreadPool.ExecuteTask(&ParseFiles_Recursive, std::filesystem::path(R"(rsc/GameResource/Horror_Farm)"));

	const char* levelPath = g_settings["Editor.Level"]["DefaultLevel"].as<const char*>();
	assert(levelPath[0] != '\0');

	LevelManager->SetActiveLevel(LevelManager->CreateOrLoad(levelPath));
}

void JWorld::Update(float DeltaTime)
{
	GUIManager->Update(DeltaTime);

	LevelManager->Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);
}

void JWorld::Render()
{
	GUIManager->Render();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::TrueBlack);
	ShaderManager->UpdateCamera(GetWorld.CameraManager->GetCurrentMainCam());

	LevelManager->Render();

	G_DebugBatch.Draw();
}

void JWorld::Release()
{
	G_DebugBatch.Release();

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

JActor* JWorld::SpawnActor(const JText&   InName,
						   const FVector& InLocation,
						   const FVector& InRotation,
						   JActor*        InOwner,
						   JLevel*        InLevel)
{
	JActor* newActor = LevelManager->GetActiveLevel()->CreateActor<JActor>(InName);
	newActor->SetWorldLocation(InLocation);
	newActor->SetWorldRotation(InRotation);
	newActor->SetOwnerActor(InOwner);

	return nullptr;
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

				if (hash == HASH_EXT_PNG || hash == HASH_EXT_JPG || hash == HASH_EXT_DDS || hash == HASH_EXT_BMP)
				{
					GetWorld.TextureManager->CreateOrLoad(entry.path().string());
				}
				else if (hash == HASH_EXT_HLSL)
				{
					GetWorld.ShaderManager->CreateOrLoad(entry.path().string());
				}
				else if (hash == Hash_EXT_JASSET)
				{
					GetWorld.MeshManager->CreateOrLoad(entry.path().string());
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
