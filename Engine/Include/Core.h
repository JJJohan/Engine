#ifndef ENGINE_H
#define ENGINE_H

#include <atomic>
#include <thread>
#include "stdafx.h"
#include "String2.h"
#include "Renderer/Renderer.h"

#define CORE Core::Instance().

namespace Engine
{
	class Core
	{
	public:
		static __declspec(dllexport) Core& Instance();

		__declspec(dllexport) void Create(std::string a_configPath);
		__declspec(dllexport) void Create(bool a_fullscreen, Renderer::Device a_renderer);
		__declspec(dllexport) void Create(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Renderer::Device a_renderer);
		__declspec(dllexport) bool Exit();
		void SetExit();

		__declspec(dllexport) void SetTitle(std::string a_title);
		__declspec(dllexport) std::string GetTitle();
		__declspec(dllexport) Renderer* GetRenderer();

		__declspec(dllexport) void Update();
		__declspec(dllexport) float DeltaTime();

	private:
		Core();
		~Core();
		Core(Core const&);
		void operator=(Core const&);

		inline void CalculateDelta();

		float m_time;
		float m_lastTime;
		bool m_exit;
		String m_title;
		float m_dt;
		Renderer* m_pRenderer;
	};
}

#endif // ENGINE_H