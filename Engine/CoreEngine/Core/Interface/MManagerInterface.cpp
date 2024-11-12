#include "MManagerInterface.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"


MManagerInterface::MManagerInterface()
	: GUIManager(),
	  ViewportManager(nullptr),
	  CameraManager(nullptr),
	  ShaderManager(nullptr),
	  TextureManager(nullptr),
	  MaterialManager(nullptr),
	  MeshManager(nullptr),
	  RenderManager(nullptr),
	  LayerManager(),
	  ThreadPool(std::thread::hardware_concurrency())
{}

void MManagerInterface::Initialize()
{
	G_Logger.Initialize();			 // Logger

	RenderManager = &XD3DDevice::Get();

	ShaderManager = &MShaderManager::Get();

	CameraManager = &MCameraManager::Get();

	ViewportManager = &MViewportManager::Get();

	TextureManager = &MTextureManager::Get();

	MeshManager = &MMeshManager::Get();

	MaterialManager = &MMaterialManager::Get();
	MaterialManager->SaveEngineMaterials();

	MaterialInstanceManager = &MMaterialInstanceManager::Get();

	GUIManager = &MGUIManager::Get();
	GUIManager->Initialize(RenderManager->GetDevice(), RenderManager->GetImmediateDeviceContext());

	World = &GetWorld();

	G_DebugBatch.Initialize();		 // Primitive Batch

	ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/Tex)"));
	ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/TT)"));
	// ThreadPool.ExecuteTask(&ParseFiles_Recursive, std::filesystem::path(R"(rsc/GameResource/Horror_Farm)"));
}

void MManagerInterface::Update(float DeltaTime)
{
	GUIManager->Update(DeltaTime);

	World->Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);
}

void MManagerInterface::Render()
{
	GUIManager->Render();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::TrueBlack);
	ShaderManager->UpdateCamera(IManager.CameraManager->GetCurrentMainCam());

	G_DebugBatch.PreRender();

	G_DebugBatch.Render();

	G_DebugBatch.PostRender();

}

void MManagerInterface::Release()
{
	G_DebugBatch.Release();

	GUIManager->Release();

	RenderManager->Release();

	Utils::Fbx::FbxFile::Release();
}


void MManagerInterface::SearchFiles_Recursive(const std::filesystem::path& InPath)
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
					IManager.TextureManager->CreateOrLoad(entry.path().string());
				}
				else if (hash == HASH_EXT_HLSL)
				{
					IManager.ShaderManager->CreateOrLoad(entry.path().string());
				}
				else if (hash == Hash_EXT_JASSET)
				{
					IManager.MeshManager->CreateOrLoad(entry.path().string());
				}
			}
		}
	}
}

void MManagerInterface::ParseFiles_Recursive(const std::filesystem::path& InPath)
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
