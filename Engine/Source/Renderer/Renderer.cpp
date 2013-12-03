#include "stdafx.h"
#include <mmsystem.h>
#include <thread>

#pragma comment(lib, "Winmm.lib")

namespace Engine
{
	Renderer::Renderer()
	{
		m_pDX11 = NULL;
		m_pDX11Buffer = NULL;
		m_pOGL = NULL;
		m_pWindow = NULL;
		m_fps = 0.0f;
		m_frameDelta = 0.0f;
		m_frames = 0;
		m_frameTime = 0.0f;
		m_lastFrameTime = (float) timeGetTime() / 1000.0f;
	}

	bool Renderer::CreateRenderer(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Device a_renderer)
	{		
		m_renderer = a_renderer;
		m_fullscreen = a_fullscreen;
		m_vsync = a_vsync;

		if (m_renderer == OPENGL)
		{
			m_pOGL = new OGL();
		}

		m_pWindow = new Window(m_pOGL);
#if defined(FORCE_SINGLE_THREADING)
		m_pWindow->OpenWindow(a_width, a_height, a_fullscreen, a_vsync);
#else
		m_tWindow = std::thread(&Window::OpenWindow, m_pWindow, a_width, a_height, a_fullscreen, a_vsync);

		while (!m_pWindow->Created())
		{
			if (APPLICATION && APPLICATION->GetExit())
				return false;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
#endif			

		if (m_renderer == DIRECTX11)
		{
			m_pDX11Buffer = _aligned_malloc(sizeof(DX11), 16);
			m_pDX11 = new (m_pDX11Buffer) DX11();
			if (m_pDX11->Initialise(a_width, a_height, a_vsync, m_pWindow->WindowHandle(), a_fullscreen, 1.0f, 1000.0f))
			{
				LOG(Logger::BLUE, "Initialised DirectX 11 Renderer.");
			}
			else
			{
				return false;
			}
		}

#if !defined(FORCE_SINGLE_THREADING)
		m_tDraw = std::thread(&Renderer::Draw, this);
#endif

		return true;
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

	bool Renderer::Active()
	{
		return (m_pDX11 || m_pOGL);
	}

	void Renderer::Draw()
	{
		if (m_renderer == DIRECTX11)
		{
#if !defined(FORCE_SINGLE_THREADING)
			while (APPLICATION && !APPLICATION->GetExit())
			{
#endif
				APPLICATION->Draw();
				CalculateFPS();
				m_pDX11->BeginScene();
				m_pDX11->Draw();
				m_pDX11->EndScene();

#if !defined(FORCE_SINGLE_THREADING)
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			m_pDX11->Release();
#endif
		}
		else
		{
#if !defined(FORCE_SINGLE_THREADING)
			// Set current thread as the rendering thread.
			m_pOGL->MakeCurrent();

			while (APPLICATION && !APPLICATION->GetExit())
			{
#endif
				APPLICATION->Draw();
				CalculateFPS();
				m_pOGL->BeginScene();
				m_pOGL->EndScene();

#if !defined(FORCE_SINGLE_THREADING)
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			m_pOGL->Release(m_pWindow->WindowHandle());
			m_pOGL = NULL;
#endif
		}
	}

	void Renderer::SetClearColour(float a_red, float a_green, float a_blue, float a_alpha)
	{
		if (m_renderer == DIRECTX11 && m_pDX11)
		{
			m_pDX11->SetClearColour(a_red, a_green, a_blue, a_alpha);
		}
		else if (m_pOGL)
		{
			m_pOGL->SetClearColour(a_red, a_green, a_blue, a_alpha);
		}
	}

	void Renderer::GetVideoCardInfo(char* a_cardName, int& a_memory)
	{
		if (m_renderer == DIRECTX11 && m_pDX11)
		{
			m_pDX11->GetVideoCardInfo(a_cardName, a_memory);
		}
		else if (m_pOGL)
		{
			m_pOGL->GetVideoCardInfo(a_cardName);
			a_memory = -1;
		}
	}

	Renderer::~Renderer()
	{
#if !defined(FORCE_SINGLE_THREADING)
		if (m_tDraw.joinable())
			m_tDraw.join();

		if (m_tWindow.joinable())
			m_tWindow.join();
#else
		if (m_renderer == DIRECTX11)
		{
			m_pDX11->Release();
		}
		else
		{
			m_pOGL->Release(m_pWindow->WindowHandle());
		}
#endif

		if (m_pDX11)
		{
			m_pDX11->~DX11();
			_aligned_free(m_pDX11Buffer);
			m_pDX11 = NULL;
			m_pDX11Buffer = NULL;
		}

		SAFE_DELETE(m_pOGL);
		SAFE_DELETE(m_pWindow);

		LOG("Renderer released.");
	}
}