#include "stdafx.h"

namespace Engine
{
	Window::Window(OGL* a_pOGL)
	{
		m_hwnd = NULL;
		m_hInstance = NULL;
		m_pOGL = a_pOGL;
		m_created = false;
	}

	void Window::OpenWindow(int a_width, int a_height, bool a_fullscreen, bool a_vsync)
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;
		m_fullscreen = a_fullscreen;

		// Get the instance of this application.
		m_hInstance = GetModuleHandle(NULL);

		// Give the application a name
		std::string title = Core::Instance().GetTitle();
		std::wstring appName = std::wstring(title.begin(), title.end());

		// Setup the windows class with default settings.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = appName.c_str();
		wc.cbSize = sizeof(WNDCLASSEX);

		// Register the window class.
		if (!RegisterClassEx(&wc))
		{
			LOG_ERROR("Window could not be registered!");
			return;
		}

		if (m_pOGL)
		{
			// Create a temporary window for the OpenGL extension setup.
			m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName.c_str(), appName.c_str(), WS_POPUP,
				0, 0, 640, 480, NULL, NULL, m_hInstance, NULL);
			if (m_hwnd == NULL)
				LOG_ERROR("Temporary window could not be created.");

			// Don't show the window.
			ShowWindow(m_hwnd, SW_HIDE);

			// Initialise OpenGL extensions.
			if (!m_pOGL->InitialiseExtensions(m_hwnd))
				LOG_ERROR("Could not initialise OpenGL extensions.");

			// Release the temporary window now that the extensions have been initialized.
			DestroyWindow(m_hwnd);
			m_hwnd = NULL;
		}

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (a_fullscreen)
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long) a_width;
			dmScreenSettings.dmPelsHeight = (unsigned long) a_height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen.
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Set the position of the window to the top left corner.
			posX = posY = 0;
		}
		else
		{
			// Place the window in the middle of the screen.
			posX = (GetSystemMetrics(SM_CXSCREEN) - a_width) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - a_height) / 2;
		}

		// Create the window with the screen settings and get the handle to it.
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName.c_str(), appName.c_str(),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
			posX, posY, a_width, a_height, NULL, NULL, m_hInstance, NULL);

		if (!m_hwnd)
		{
			LOG_ERROR("Window could not be created!");
			return;
		}

		if (m_pOGL)
		{
			if (!m_pOGL->InitialiseOpenGL(a_width, a_height, a_vsync, m_hwnd, 1.0f, 1000.0f))
				LOG_ERROR("Could not initialise OpenGL.");
			
			LOG("Initialised OpenGL Renderer.", Logger::BLUE);
		}

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		// Hide the mouse cursor.
		//ShowCursor(false);

		m_created = true;
		LOG("Window created.");

		// Process message pump.
		Update();
	}
	
	void Window::SetTitle(std::string a_title)
	{
		std::wstring title = std::wstring(a_title.begin(), a_title.end());
		SetWindowText(m_hwnd, title.c_str());
	}

	std::string Window::GetTitle()
	{
		LPWSTR buff = new WCHAR[128];
		GetWindowText(m_hwnd, buff, 128);
		std::wstring wtitle = buff;
		std::string title = std::string(wtitle.begin(), wtitle.end());
		return title;
	}

	bool Window::Created()
	{
		return m_created;
	}

	HINSTANCE Window::InstanceHandle()
	{
		return m_hInstance;
	}

	HWND Window::WindowHandle()
	{
		return m_hwnd;
	}

	Window::~Window()
	{
		// Remove the window.
		if (m_fullscreen)
			ChangeDisplaySettings(NULL, 0);

		DestroyWindow(m_hwnd);
		m_hwnd = NULL;

		// Remove the application instance.
		std::string title = Core::Instance().GetTitle();
		std::wstring appName = std::wstring(title.begin(), title.end());
		UnregisterClass(appName.c_str(), m_hInstance);
		m_hInstance = NULL;
	}

	void Window::Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (!Core::Instance().Exit())
		{
			// Handle the windows messages.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT)
			{
				Core::Instance().SetExit();
			}
		
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	LRESULT CALLBACK Window::WndProc(HWND a_hwnd, UINT a_umessage, WPARAM a_wparam, LPARAM a_lparam)
	{
		 return Input::Instance().MessageHandler(a_hwnd, a_umessage, a_wparam, a_lparam);
	}
}