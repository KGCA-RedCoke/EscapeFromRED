#pragma once
#include "Core/Window/Application.h"
#include "Core/Window/Window.h"

// ---------------------------- Resolution -------------------------
constexpr uint32_t HD_X  = 1280;
constexpr uint32_t HD_Y  = 720;
constexpr uint32_t FHD_X = 1920;
constexpr uint32_t FHD_Y = 1080;
constexpr uint32_t QHD_X = 2560;
constexpr uint32_t QHD_Y = 1440;

// ---------------------------- Name -------------------------
constexpr wchar_t Name_Engine_Version[] = L"J.01";

#if defined(_DEBUG)
	#define RUN(Engine_V, Resolution_X, Resolution_Y)\
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);\
	FBasicWindowData g_initialWinData{Resolution_X, Resolution_Y, false, false};\
	Application App(Engine_V, g_initialWinData);\
	App.Run();
#else
	#define RUN(Engine_V, Resolution_X, Resolution_Y)\
	FBasicWindowData g_initialWinData{Resolution_X, Resolution_Y, false, false};\
	Application App(Engine_V, g_initialWinData);\
	App.Run();
#endif
