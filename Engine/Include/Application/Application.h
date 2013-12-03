#ifndef APPLICATION_H
#define APPLICATION_H

#include <atomic>
#include <thread>
#include "stdafx.h"

#define APPLICATION Application::Instance()

namespace Engine
{
	class Application
	{
	public:
		__declspec(dllexport) static Application* Instance();

		__declspec(dllexport) bool Create(std::string a_configPath);
		__declspec(dllexport) bool Create(bool a_fullscreen, Renderer::Device a_renderer);
		__declspec(dllexport) bool Create(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Renderer::Device a_renderer);

		__declspec(dllexport) void SetTitle(std::string a_title);
		__declspec(dllexport) std::string GetTitle();
		__declspec(dllexport) Renderer* GetRenderer();

		__declspec(dllexport) bool GetExit();
		__declspec(dllexport) void UpdateInternal();
		__declspec(dllexport) void Release();

		__declspec(dllexport) virtual void Config() = 0;
		__declspec(dllexport) virtual void Update(const float a_delta) = 0;
		__declspec(dllexport) virtual void Input() = 0;
		__declspec(dllexport) virtual void Draw() = 0;
		__declspec(dllexport) virtual void Shutdown();

	protected:
		static Application* m_pApplication;
		__declspec(dllexport) Application();

	private:	
		Application(Application const&);
		void operator=(Application const&);

		float m_timeCurr;
		float m_timeLast;
		bool m_exit;
		String m_title;
		Renderer* m_pRenderer;
	};
}

#endif // APPLICATION_H