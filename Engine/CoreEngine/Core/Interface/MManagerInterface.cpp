#include "MManagerInterface.h"
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

	MaterialInstanceManager = &MMaterialInstanceManager::Get();

	GUIManager = &MGUIManager::Get();
	GUIManager->Initialize(RenderManager->GetDevice(), RenderManager->GetImmediateDeviceContext());

	World = &GetWorld();

	G_DebugBatch.Initialize();		 // Primitive Batch

	ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(rsc/Engine/Tex)"));
	// ThreadPool.ExecuteTask(&SearchFiles_Recursive, std::filesystem::path(R"(Game/Mesh)"));
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

	auto deviceContext = IManager.RenderManager->GetImmediateDeviceContext();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager->SetRenderTarget("Editor Viewport", FLinearColor::Studio);
	ShaderManager->UpdateCamera(IManager.CameraManager->GetCurrentMainCam());

	World->Draw();

	//
	deviceContext->OMSetBlendState(IManager.RenderManager->GetDXTKCommonStates()->AlphaBlend(),
								   nullptr,
								   0xFFFFFFFF);

	deviceContext->RSSetState(IManager.RenderManager->GetRasterizerState());

	G_DebugBatch.PreRender();

	G_DebugBatch.Render();

	// Draw X Axis (Infinity Line)
	G_DebugBatch.DrawRay(
						 {0.f, 0.f, 0.f, 0.f},
						 {100.f, 0.f, 0.f, 0.f},
						 false,
						 {1.f, 0.f, 0.f, 1.f}
						);

	// Draw Z Axis
	G_DebugBatch.DrawRay(
						 {0.f, 0.f, 0.f, 0.f},
						 {0.f, 0.f, 100.f, 0.f},
						 false,
						 {0.f, 0.f, 1.f, 1.f}
						);

	// Draw Y Axis
	G_DebugBatch.DrawRay(
						 {0.f, 0.f, 0.f, 0.f},
						 {0.f, 100.f, 0.f, 0.f},
						 false,
						 {0.f, 1.f, 0.f, 1.f}
						);

	G_DebugBatch.PostRender();

	LayerManager.Render();
}

void MManagerInterface::Release()
{
	G_DebugBatch.Release();

	GUIManager->Release();

	IManager.RenderManager->Release();

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
				// 해시 비교
				const uint32_t hash = StringHash(entry.path().extension().string().c_str());

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
