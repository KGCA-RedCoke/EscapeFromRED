#include "MManagerInterface.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"


MManagerInterface::MManagerInterface()
	: GUIManager(),
	  ViewportManager(),
	  CameraManager(),
	  ShaderManager(),
	  TextureManager(),
	  MaterialManager(),
	  LayerManager(),
	  ThreadPool(std::thread::hardware_concurrency())
{}

void MManagerInterface::Initialize()
{
	G_Logger.Initialize();			 // Logger

	DeviceRSC.Initialize();			 // Graphic Device, Context...

	G_DebugBatch.Initialize();		 // Primitive Batch
	//
	CameraManager.Initialize();
	//
	GUIManager.Initialize();
	//
	ViewportManager.Initialize();
	//
	ShaderManager.Initialize();
	//
	TextureManager.Initialize();
	//
	MaterialManager.Initialize();
}

void MManagerInterface::Update(float DeltaTime)
{
	GUIManager.Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);
}

void MManagerInterface::Render()
{
	GUIManager.Render();

	auto deviceContext = DeviceRSC.GetImmediateDeviceContext();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager.SetRenderTarget("Editor Viewport", FLinearColor::Blender_Grid_Gray);
	//
	deviceContext->OMSetBlendState(DeviceRSC.GetDXTKCommonStates()->AlphaBlend(),
								   nullptr,
								   0xFFFFFFFF);

	deviceContext->RSSetState(DeviceRSC.GetRasterizerState());

	G_DebugBatch.PreRender();

	G_DebugBatch.Render();

	G_DebugBatch.DrawGrid(
						  {100.f, 0.f, 0.f, 0.f},
						  {0.f, 0.f, 100.f, 0.f},
						  {0.f, 0.f, 0.f, 0.f},
						  100.f,
						  100.f,
						  {0.33f, 0.33f, 0.33f, 1.f}
						 );

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

	GUIManager.Release();

	DeviceRSC.Release();
}
