#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include "stdafx.h"

using namespace Engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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

	// Clean up.
	app->Release();

	return 0;
}