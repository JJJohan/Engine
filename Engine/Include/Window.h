#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Engine
{
	class OGL;

	class Window
	{
	public:
		Window(OGL* a_pOGL = NULL);
		void OpenWindow(int a_width, int a_height, bool a_fullscreen, bool a_vsync);
		~Window();

		bool Created();
		void SetTitle(std::string a_title);
		std::string GetTitle();

		HWND WindowHandle();
		HINSTANCE InstanceHandle();

	private:
		void Update();
		static LRESULT CALLBACK WndProc(HWND a_hwnd, UINT a_umessage, WPARAM a_wparam, LPARAM a_lparam);

		bool m_created;
		bool m_fullscreen;
		HWND m_hwnd;
		HINSTANCE m_hInstance;
		OGL* m_pOGL;
	};
}

#endif // WINDOW_H