#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace Engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	///_CrtSetBreakAlloc(446);
#endif

	// Init logging system.
	Logger::Instance().InitConsole();

	// Fetch the application instance.
	Application* app = Application::Instance();
	if (!app)
	{
		LOG_ERROR("Engine was not instantiated.");
		return 1;
	}

	// Initialise the application.
	app->Config();

	// Run the main loop.
	while (!app->GetExit())
		app->UpdateInternal();

	// Ensure cleanup.
	app->Release();

#if defined(_DEBUG)
	//_CrtDumpMemoryLeaks();
#endif

	return 0;
}