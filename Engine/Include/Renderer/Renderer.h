#ifndef RENDERER_H
#define RENDERER_H

#include <thread>
#include <atomic>

#define RENDERER APPLICATION->GetRenderer()

namespace Engine
{
	class OGL;
	class DX11;
	class Window;

	class Renderer
	{
	public:
		enum Device
		{
			DIRECTX11 = 0,
			OPENGL = 1
		};

		__declspec(dllexport) Renderer();
		__declspec(dllexport) ~Renderer();
		__declspec(dllexport) bool CreateRenderer(int a_width, int a_height, bool a_fullscreen, bool a_vsync, Device a_renderer);
		void Draw();
		void Release();
		__declspec(dllexport) Device GraphicsMode();
		__declspec(dllexport) float FPS();
		__declspec(dllexport) float FrameTime();
		__declspec(dllexport) Window* GetWindow();
		__declspec(dllexport) bool VSync();

		__declspec(dllexport) void SetClearColour(float a_red, float a_green, float a_blue, float a_alpha);
		__declspec(dllexport) void GetVideoCardInfo(char* a_cardName, int& a_memory);

	private:
		inline void CalculateFPS();

		Device m_renderer;
		float m_fps;
		float m_frameDelta;
		float m_frameTime;
		float m_lastFrameTime;
		int m_frames;
		bool m_vsync;
		bool m_fullscreen;
		OGL* m_pOGL;
		DX11* m_pDX11;
		Window* m_pWindow;
		std::thread m_tWindow;
		std::thread m_tDraw;
	};
}

#endif // RENDERER_H