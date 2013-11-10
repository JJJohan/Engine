#include "stdafx.h"
#include "wtypes.h"
#include <mmsystem.h>
#include <fstream>
#include <sstream>

namespace Engine
{
	Core& Core::Instance()
	{
		static Core instance;
		return instance;
	}

	Core::Core()
	{
		m_exit = false;
		m_title = "Application";
		m_dt = 0.0f;
		m_pRenderer = NULL;
		m_time = 0.0f;
		m_lastTime = (float)timeGetTime() / 1000.0f;
	}

	Core::~Core()
	{
		m_exit = true;
		SAFE_DELETE(m_pRenderer);
	}

	void Core::SetExit()
	{
		m_exit = true;
	}

	float Core::DeltaTime()
	{
		return m_dt;
	}

	void Core::Create(std::string a_configPath)
	{
		int width = 0, height = 0;
		Renderer::Device renderer = Renderer::Device::DIRECTX11;
		bool fullscreen = false;
		bool vsync = false;
		bool configValid = false;

		std::ifstream file(a_configPath);
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
		}

		if (!configValid)
		{
			Core::Create(1024, 768, false, false, Renderer::Device::DIRECTX11);
		}
		else
		{
			Core::Create(width, height, fullscreen, vsync, renderer);
		}	
	}

	void Core::Create(bool a_fullscreen, Renderer::Device a_renderer)
	{
		Core::Create(SysInfo::Resolution::Width(), SysInfo::Resolution::Height(), a_fullscreen, false, a_renderer);
	}

	void Core::Create(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Renderer::Device a_renderer)
	{
		LOG("Creating engine.");

		if (!m_pRenderer)
			m_pRenderer = new Renderer(a_width, a_height, a_fullscreen, a_vsync, a_renderer);
		else
			LOG_ERROR(false, "Renderer already exists.");
	}

	Renderer* Core::GetRenderer()
	{
		return m_pRenderer;
	}

	void Core::SetTitle(std::string a_string)
	{
		m_title = a_string;
	}

	std::string Core::GetTitle()
	{
		if (!m_pRenderer || !m_pRenderer->GetWindow())
			return "Application";

		return m_pRenderer->GetWindow()->GetTitle();
	}

	void Core::Update()
	{
		if (m_pRenderer && m_pRenderer->GetWindow())
		{
			std::stringstream ss;
			ss << m_pRenderer->FPS() << " FPS";
			m_pRenderer->GetWindow()->SetTitle(ss.str());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	bool Core::Exit()
	{
		return m_exit;
	}
}