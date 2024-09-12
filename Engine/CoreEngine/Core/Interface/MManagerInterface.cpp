#include "common_pch.h"
#include "MManagerInterface.h"

#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"


MManagerInterface::MManagerInterface()
	: TextureManager(),
	  ShaderManager(),
	  GUIManager(),
	  ViewportManager(),
	  CameraManager(),
	  LayerManager(),
	  ThreadPool(std::thread::hardware_concurrency())
{}

void MManagerInterface::Initialize()
{
	G_Logger.Initialize();			 // Logger

	G_Context.Initialize();			 // Graphic Device, Context...

	G_DebugBatch.Initialize();		 // Primitive Batch

	CameraManager.Initialize();

	GUIManager.Initialize();

	ViewportManager.Initialize();

	ShaderManager.Initialize();
}

void MManagerInterface::Update(float DeltaTime)
{
	GUIManager.Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);

	G_Context.Update(DeltaTime);
}

void MManagerInterface::Render()
{
	GUIManager.Render();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager.SetRenderTarget("Editor Viewport", FLinearColor::EbonyClay);

	G_DebugBatch.PreRender();

	G_DebugBatch.Render();

	G_DebugBatch.DrawGrid(
						  {100.f, 0.f, 0.f, 0.f},
						  {0.f, 0.f, 100.f, 0.f},
						  {0.f, 0.f, 0.f, 0.f},
						  10.f,
						  10.f,
						  Colors::Gray
						 );

	G_DebugBatch.PostRender();

	LayerManager.Render();
}

void MManagerInterface::Release()
{
	G_DebugBatch.Release();

	GUIManager.Release();

	G_Context.Release();
}
