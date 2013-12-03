#include "stdafx.h"
#include "wtypes.h"
#include <mmsystem.h>
#include <fstream>
#include <sstream>

using namespace Engine;

Application* Application::m_pApplication = NULL;

Application::Application()
{
	m_pApplication = this;

	m_exit = false;
	m_title = "Application";
	m_pRenderer = NULL;
	m_timeCurr = 0.0f;
	m_timeLast = (float)timeGetTime() / 1000.0f;

	SysInfo::Refresh();
}

Application* Application::Instance()
{
	return m_pApplication;
}

void Application::Release()
{
	if (!m_exit)
	{
		Shutdown();
		m_exit = true;
		SAFE_DELETE(m_pRenderer);
	}
}

void Application::Shutdown()
{

}

bool Application::AutoCreate()
{
	int width = 0, height = 0;
	Renderer::Device renderer = Renderer::Device::DIRECTX11;
	bool fullscreen = false;
	bool vsync = false;
	bool configValid = false;

	/*std::ifstream file(a_configPath);
	if (file.is_open())
	{
		String string;
		while (getline(file, string.StdString()))
		{
			std::vector<std::string> vec = string.Split('=');
			if (vec.size() > 1)
			{
				String left = vec[0];
				String right = vec[1];
				left = left.RemoveChar(' ');
				right = right.RemoveChar(' ');
				if (_tolower(left.CString()) == "width")
				{
					width = right.ToInt();
				}
				else if (_tolower(left.CString()) == "height")
				{
					height = right.ToInt();
				}
				else if (_tolower(left.CString()) == "fullscreen")
				{
					if (right.ToInt() > 0)
						fullscreen = true;
				}
				else if (_tolower(left.CString()) == "renderer")
				{
					renderer = (Renderer::Device)right.ToInt();
				}
				else if (_tolower(left.CString()) == "vsync")
				{
					if (right.ToInt() > 0)
						vsync = true;
				}
			}
		}

		if (width > 0 && height > 0 && renderer > -1 && renderer < 2)
			configValid = true;
			
		file.close();
	}*/

	if (!configValid)
	{
		return Application::Create(1024, 768, false, false, Renderer::Device::DIRECTX11);
	}
	else
	{
		return Application::Create(width, height, fullscreen, vsync, renderer);
	}	

	return false;
}

bool Application::Create(bool a_fullscreen, Renderer::Device a_renderer)
{
	return Application::Create(SysInfo::Screen::width, SysInfo::Screen::height, a_fullscreen, false, a_renderer);
}

bool Application::Create(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Renderer::Device a_renderer)
{
	LOG("Creating engine.");

	if (!m_pRenderer)
	{
		m_pRenderer = new Renderer();
		if (m_pRenderer->CreateRenderer(a_width, a_height, a_fullscreen, a_vsync, a_renderer))
		{
			return true;
		}
		else
		{
			m_exit = true;
			SAFE_DELETE(m_pRenderer);
			return false;
		}
	}
			
	LOG_ERROR(false, "Renderer already exists.");
	return false;
}

Renderer* Application::GetRenderer()
{
	return m_pRenderer;
}

void Application::SetTitle(std::string a_string)
{
	m_title = a_string;
}

std::string Application::GetTitle()
{
	if (!m_pRenderer || !m_pRenderer->GetWindow() || !m_pRenderer->GetWindow()->WindowHandle())
		return "Application";

	return m_pRenderer->GetWindow()->GetTitle();
}

void Application::UpdateInternal()
{
	// Draw
	if (m_pRenderer && m_pRenderer->GetWindow())
	{
		std::stringstream ss;
		ss << m_pRenderer->FPS() << " FPS";
		m_pRenderer->GetWindow()->SetTitle(ss.str());

#if defined(FORCE_SINGLE_THREADING)
		m_pRenderer->Draw();
		m_pRenderer->GetWindow()->Update();
#endif

		if (m_pRenderer->GetWindow()->Shutdown())
			Release();		
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// Delta Time
	m_timeCurr = (float) timeGetTime() / 1000.0f;
	float delta = m_timeCurr - m_timeLast;
	m_timeLast = m_timeCurr;

	Update(delta);
}

bool Application::GetExit()
{
	return m_exit;
}