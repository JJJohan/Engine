#include "stdafx.h"

#define RELEASE(x) if (x) { x->Release(); x = NULL; }

namespace Engine
{
	DX11::DX11()
	{
		m_pSwapChain = NULL;
		m_pDevice = NULL;
		m_pDeviceContext = NULL;
		m_pRenderTargetView = NULL;
		m_pDepthStencilBuffer = NULL;
		m_pDepthStencilState = NULL;
		m_pDepthStencilView = NULL;
		m_pRasterState = NULL;
		m_renderScale = 1.0f;
		m_pBackBuffer = NULL;
#if defined(_DEBUG)
		m_pDebug = NULL;
#endif

		SetClearColour(0.0f, 0.0f, 0.25f, 1.0f);
	}

	bool DX11::Initialise(int a_width, int a_height, bool a_vsync,
		HWND a_hwnd, bool a_fullscreen, float a_screenNear, float a_screenFar)
	{
		// Store common display settings
		m_width = a_width;
		m_height = a_height;
		m_vsync = a_vsync;
		m_fullscreen = a_fullscreen;

		// Initiate renderer
		bool device = CreateDevice(a_hwnd);
		bool resources = CreateResources(a_screenNear, a_screenFar);

#if defined(_DEBUG)
		if (device && resources)
		{
			//m_pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
#endif

		return device && resources;
	}

	bool DX11::CreateDevice(HWND a_hwnd)
	{
		HRESULT result;

		// Create a DirectX graphics interface factory.
		IDXGIFactory* factory;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create DXGI Factory.");
			return false;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create graphics adapter.");
			return false;
		}

		// Enumerate the primary adapter output (monitor).
		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create graphics adapter output.");
			return false;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		unsigned int displaymodes;
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displaymodes, NULL);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create display mode list.");
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[displaymodes];
		if (!displayModeList)
		{
			LOG_ERROR("Could not fill display mode list.");
			return false;
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displaymodes, displayModeList);
		if (FAILED(result))
		{
			LOG_ERROR("Could not fill display mode list structures.");
			return false;
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		unsigned int numerator;
		unsigned int denominator;
		for (unsigned int i = 0; i < displaymodes; i++)
		{
			if (displayModeList[i].Width == (unsigned int) m_width)
			{
				if (displayModeList[i].Height == (unsigned int) m_height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Get the adapter (video card) description
		DXGI_ADAPTER_DESC adapterDesc;
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			LOG_ERROR("Could not get graphics device description.");
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		m_vmemory = (int) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		unsigned int stringLength;
		int error = wcstombs_s(&stringLength, m_cardDesc, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			LOG_ERROR("Could not get graphics device name.");
			return false;
		}

		// Release the display mode list.
		delete [] displayModeList;
		displayModeList = 0;

		// Release resources.
		RELEASE(adapterOutput);
		RELEASE(adapter);
		RELEASE(factory);

		// Initialize the swap chain description.
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = (UINT) (m_width * m_renderScale);
		swapChainDesc.BufferDesc.Height = (UINT) (m_height * m_renderScale);

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Set the refresh rate of the back buffer.
		if (m_vsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		swapChainDesc.OutputWindow = a_hwnd;

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Set to full screen or windowed mode.
		swapChainDesc.Windowed = (m_fullscreen) ? false : true;

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		// Create the swap chain, Direct3D device, and Direct3D device context.
#if defined(_DEBUG)
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS, NULL, NULL,
			D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
#else
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL,
			D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
#endif
		if (FAILED(result))
		{
			LOG_ERROR("Could not create D3D11 device, device context and swap chain.");
			return false;
		}

		// Get the pointer to the back buffer.
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &m_pBackBuffer);
		if (FAILED(result))
		{
			LOG_ERROR("Could not get back buffer.");
			return false;
		}

#if defined(_DEBUG)
		result = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (LPVOID*) &m_pDebug);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create debug interface.");
			return false;
		}
#endif

		return true;
	}

	bool DX11::CreateResources(float a_screenNear, float a_screenFar)
	{
		HRESULT result;

		// Create the render target view with the back buffer pointer.
		result = m_pDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create render target.");
			return false;
		}

		RELEASE(m_pBackBuffer);

		// Initialize the description of the depth buffer.
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
		depthBufferDesc.Width = m_width;
		depthBufferDesc.Height = m_height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// Create the texture for the depth buffer using the filled out description.
		result = m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create depth buffer texture.");
			return false;
		}

		// Initialize the description of the stencil state.
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create depth stencil state.");
			return false;
		}

		// Set the depth stencil state.
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

		// Initialise the depth stencil view.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view.
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create depth stencil view.");
			return false;
		}

		// Bind the render target view and depth stencil buffer to the output render pipeline.
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create rasterizer state.");
			return false;
		}

		// Now set the rasterizer state.
		m_pDeviceContext->RSSetState(m_pRasterState);

		// Setup the viewport for rendering.
		D3D11_VIEWPORT viewport;
		viewport.Width = (float) m_width;
		viewport.Height = (float) m_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// Create the viewport.
		m_pDeviceContext->RSSetViewports(1, &viewport);

		// Setup the projection matrix.
		float fieldOfView = (float) XM_PI / 4.0f;
		float screenAspect = (float) m_width / (float) m_height;

		// Create the projection matrix for 3D rendering.
		m_projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, a_screenNear, a_screenFar);

		// Initialize the world matrix to the identity matrix.
		m_worldMatrix = XMMatrixIdentity();

		// Create an orthographic projection matrix for 2D rendering.
		m_orthoMatrix = XMMatrixOrthographicLH((float) m_width, (float) m_height, a_screenNear, a_screenFar);

		return true;
	}

	void DX11::SetClearColour(float a_red, float a_green, float a_blue, float a_alpha)
	{
		m_clearColour[0] = a_red;
		m_clearColour[1] = a_green;
		m_clearColour[2] = a_blue;
		m_clearColour[3] = a_alpha;
	}

	void DX11::BeginScene()
	{
		// Clear the back buffer.
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, m_clearColour);

		// Clear the depth buffer.
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void DX11::Draw()
	{

	}

	void DX11::EndScene()
	{
		// Present the back buffer to the screen since rendering is complete.
		(m_vsync) ? m_pSwapChain->Present(1, 0) : m_pSwapChain->Present(0, 0);
	}

	ID3D11Device* DX11::GetDevice()
	{
		return m_pDevice;
	}

	ID3D11DeviceContext* DX11::GetDeviceContext()
	{
		return m_pDeviceContext;
	}

	XMMATRIX& DX11::GetProjectionMatrix()
	{
		return m_projMatrix;
	}


	XMMATRIX& DX11::GetWorldMatrix()
	{
		return m_worldMatrix;
	}


	XMMATRIX& DX11::GetOrthoMatrix()
	{
		return m_orthoMatrix;
	}

	void DX11::GetVideoCardInfo(char* a_cardName, int& a_memory)
	{
		strcpy_s(a_cardName, 128, m_cardDesc);
		a_memory = m_vmemory;
	}

	void DX11::Release()
	{
		if (m_pSwapChain)
		{
			// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		if (m_pDeviceContext)
			m_pDeviceContext->ClearState();

		RELEASE(m_pRasterState);
		RELEASE(m_pDepthStencilView);
		RELEASE(m_pDepthStencilState);
		RELEASE(m_pDepthStencilBuffer);
		RELEASE(m_pRenderTargetView);
		RELEASE(m_pSwapChain);
		RELEASE(m_pDevice);
		RELEASE(m_pDeviceContext);

		LOG("DX11 released.");

#if defined(_DEBUG)
		if (m_pDebug)
		{
			m_pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}

		RELEASE(m_pDebug);
#endif
	}
}