#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include "Core.h"

using namespace Engine;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// Init logging system.
	Logger::Instance().InitConsole();

	// Create renderer.
	Core::Instance().Create(1024, 768, false, false, Renderer::Device::DIRECTX11);

	// Grab a page.
	//WEB_RESULT result = WEB_ASYNC.PostGet("http://www.renscreations.com/quthiking/dynamic/registerprocess.php", "username=guest&password=password&email=blah@blah.com&dob=1993-07-11&optin=1&about=hi there!&name=Anonymous");
	//WEB_RESULT result = WEB_ASYNC.PostGet("http://www.renscreations.com/quthiking/dynamic/loginprocess.php", "username=jjjohan&password=12123");

	// Update.
	while (!Core::Instance().Exit())
	{
		Core::Instance().Update();
	}

	return 0;
}