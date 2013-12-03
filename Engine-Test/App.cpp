#include "App.h"

using namespace Engine;

App appInstance;

App::App()
{}

void App::Config()
{
	LOG("Running Config");

	SysInfo::Print();

	// Create renderer.
	//if (Config::Instance().Loaded())
	if (1)
	{

		if (!AutoCreate())
		{
			LOG_ERROR("Engine could not be initialised.");
		}
	}
	else
	{
		if (!Create(1024, 768, false, false, Renderer::Device::DIRECTX11))
		{
			LOG_ERROR("Engine could not be initialised.");
		}
	}
}

void App::Update(const float a_delta)
{
	
}

void App::Draw()
{
	
}

void App::Input()
{

}

void App::Shutdown()
{
	LOG("Exiting...");
}