#ifndef DX11_H
#define DX11_H

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace Engine
{
	class DX11
	{
	public:
		DX11();

		bool Initialise(int a_width, int a_height, bool a_vsync,
			HWND a_hwnd, bool a_fullscreen,	float a_screenNear, float a_screenFar);
		void Release();

		void BeginScene();
		void Draw();
		void EndScene();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		XMMATRIX& GetProjectionMatrix();
		XMMATRIX& GetWorldMatrix();
		XMMATRIX& GetOrthoMatrix();

		void SetClearColour(float a_red, float a_green, float a_blue, float a_alpha);
		void GetVideoCardInfo(char* a_cardName, int& a_memory);

	private:
		HRESULT SetupMRTs();
		void RenderTextures();

		bool CreateDevice(HWND a_hwnd);
		bool CreateResources(float a_screenNear, float a_screenFar);

		bool m_fullscreen;
		bool m_vsync;
		int m_vmemory;
		int m_width;
		int m_height;
		char m_cardDesc[128];
		float m_clearColour[4];
		float m_renderScale;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilState* m_pDepthStencilState;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11RasterizerState* m_pRasterState;
		ID3D11Texture2D* m_pBackBuffer;

#if defined(_DEBUG)
		ID3D11Debug* m_pDebug;
#endif
		XMMATRIX m_projMatrix;
		XMMATRIX m_worldMatrix;
		XMMATRIX m_orthoMatrix;

		// The Multiple Render Targets
		ID3D11Texture2D*                    m_mrtTex;// Environment map
		ID3D11RenderTargetView*             m_mrtRTV;// Render target view for the mrts
		ID3D11ShaderResourceView*           m_mrtSRV;// Shader resource view for the mrts
		ID3D10EffectShaderResourceVariable* m_mrtTextureVariable = NULL; // for sending in the mrts
		ID3D11Texture2D*                    m_mrtMapDepth;// Depth
		ID3D11DepthStencilView*             m_mrtDSV;// Depth stencil view
	};
}

#endif // DX11_H