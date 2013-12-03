#include "stdafx.h"
#include <windows.h>

namespace Engine
{
	Input& Input::Instance()
	{
		static Input instance;
		return instance;
	}

	Input::Input()
	{

	}

	Input::~Input()
	{

	}

	LRESULT CALLBACK Input::MessageHandler(HWND a_hwnd, UINT a_umsg, WPARAM a_wparam, LPARAM a_lparam)
	{
		switch (a_umsg)
		{
			// Check if the window is being closed.
		case WM_CLOSE:
		{
						 ::DestroyWindow(a_hwnd);
						 return 0;
		}

		case WM_DESTROY:
		{
						   if (RENDERER && RENDERER->GetWindow())
						   {
							   if (RENDERER->GetWindow()->Created())
							   {
								   PostQuitMessage(0);
							   }
						   }
							 
						   return 0;
		}

			// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
							// If a key is pressed send it to the input object so it can record that state.
							//m_Input->KeyDown((unsigned int) wparam);
							return 0;
		}

			// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
							// If a key is released then send it to the input object so it can unset the state for that key.
							//m_Input->KeyUp((unsigned int) wparam);
							return 0;
		}

			// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
				   return DefWindowProc(a_hwnd, a_umsg, a_wparam, a_lparam);
		}
		}
	}
}