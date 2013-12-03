#ifndef INPUT_H
#define INPUT_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Engine
{
	class Input
	{
	public:
		static __declspec(dllexport) Input& Instance();

		LRESULT CALLBACK MessageHandler(HWND a_hwnd, UINT a_umsg, WPARAM a_wparam, LPARAM a_lparam);

	private:
		__declspec(dllexport) Input();
		__declspec(dllexport) ~Input();
		__declspec(dllexport) Input(Input const&);
		__declspec(dllexport) void operator=(Input const&);
	};
}

#endif // INPUT_H