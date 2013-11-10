#include "stdafx.h"
#include <mmsystem.h>
#include <thread>

#pragma comment(lib, "Winmm.lib")

namespace Engine
{
	Renderer::Renderer(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Device a_renderer)
	{
		m_pDX11 = NULL;
		m_pOGL = NULL;
		m_renderer = a_renderer;
		m_fullscreen = a_fullscreen;
		m_vsync = a_vsync;

		m_fps = 0.0f;
		m_frameDelta = 0.0f;
		m_frames = 0;
		m_frameTime = 0.0f;
		m_lastFrameTime = (float)timeGetTime() / 1000.0f;

		if (m_renderer == OPENGL)
		{
			m_pOGL = new OGL();
		}

		m_pWindow = new Window(m_pOGL);
		m_tWindow = std::thread(&Window::OpenWindow, m_pWindow, a_width, a_height, a_fullscreen, a_vsync);

		while (!m_pWindow->Created())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_renderer == DIRECTX11)
		{
			//void* buffer = _aligned_malloc(sizeof(DX11), 16);
			m_pDX11 = new DX11();
			m_pDX11->Initialise(a_width, a_height, a_vsync, m_pWindow->WindowHandle(), a_fullscreen, 1.0f, 1000.0f);
			LOG("Initialised DirectX 11 Renderer.", Logger::BLUE);
		}

		m_tDraw = std::thread(&Renderer::Draw, this);
	}

	Renderer::Device Renderer::GraphicsMode()
	{
		return m_renderer;
	}

	bool Renderer::VSync()
	{
		return m_vsync;
	}

	Window* Renderer::GetWindow()
	{
		return m_pWindow;
	}

	float Renderer::FrameTime()
	{
		return m_frameDelta;
	}

	float Renderer::FPS()
	{
		return m_fps;
	}

	inline void Renderer::CalculateFPS()
	{
		float m_frameTime = (float)timeGetTime() / 1000.0f;
		m_frames++;
		m_frameDelta = m_frameTime - m_lastFrameTime;

		if (m_frameDelta >= 1.0f)
		{
			// Do calculate FPS
			m_fps = m_frames / (m_frameDelta);

			m_frames = 0;
			m_lastFrameTime = m_frameTime;
		}
	}

	void Renderer::Draw()
	{
		if (m_renderer == DIRECTX11)
		{
			while (!Core::Instance().Exit())
			{
				CalculateFPS();
				m_pDX11->BeginScene();
				m_pDX11->EndScene();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			m_pDX11->Shutdown();
		}
		else
		{
			// Set current thread as the rendering thread.
			m_pOGL->MakeCurrent();

			while (!Core::Instance().Exit())
			{
				CalculateFPS();
				m_pOGL->BeginScene();
				m_pOGL->EndScene();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	void Renderer::CloseWindow()
	{
		Sleep(500);

		m_tWindow.join();
		SAFE_DELETE(m_pWindow);
	}

	Renderer::~Renderer()
	{
		m_tDraw.join();
		m_tWindow.join();

		SAFE_DELETE(m_pDX11);
		SAFE_DELETE(m_pOGL);
		SAFE_DELETE(m_pWindow);
	}
}