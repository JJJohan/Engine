#include "stdafx.h"
#include "Logger.h"

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

		SetClearColour(0.0f, 0.0f, 0.25f, 1.0f);
	}

	bool DX11::Initialise(int a_width, int a_height, bool a_vsync,
		HWND a_hwnd, bool a_fullscreen, float a_screenNear, float a_screenFar)
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		unsigned int numModes, i, numerator, denominator, stringLength;
		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		int error;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		//D3D_FEATURE_LEVEL featureLevel;
		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;
		float fieldOfView, screenAspect;

		// Store the vsync setting.
		m_vsync = a_vsync;

		// Create a DirectX graphics interface factory.
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create DXGI Factory.");
			return false;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create graphics adapter.");
			return false;
		}

		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create graphics adapter output.");
			return false;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create display mode list.");
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			LOG_ERROR("Could not fill display mode list.");
			return false;
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			LOG_ERROR("Could not fill display mode list structures.");
			return false;
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int) a_width)
			{
				if (displayModeList[i].Height == (unsigned int) a_height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Get the adapter (video card) description.
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			LOG_ERROR("Could not get graphics device description.");
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		m_vmemory = (int) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		error = wcstombs_s(&stringLength, m_cardDesc, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			LOG_ERROR("Could not get graphics device name.");
			return false;
		}

		// Release the display mode list.
		delete [] displayModeList;
		displayModeList = 0;

		// Release the adapter output.
		adapterOutput->Release();
		adapterOutput = 0;

		// Release the adapter.
		adapter->Release();
		adapter = 0;

		// Release the factory.
		factory->Release();
		factory = 0;

		// Initialize the swap chain description.
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = a_width;
		swapChainDesc.BufferDesc.Height = a_height;

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
		if (a_fullscreen)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		// Create the swap chain, Direct3D device, and Direct3D device context.
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL,
			D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create D3D11 device, device context and swap chain.");
			return false;
		}

		// Get the pointer to the back buffer.
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBufferPtr);
		if (FAILED(result))
		{
			LOG_ERROR("Could not get back buffer.");
			return false;
		}

		// Create the render target view with the back buffer pointer.
		result = m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView);
		if (FAILED(result))
		{
			LOG_ERROR("Could not create render target.");
			return false;
		}

		// Release pointer to the back buffer as we no longer need it.
		backBufferPtr->Release();
		backBufferPtr = 0;

		// Initialize the description of the depth buffer.
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
		depthBufferDesc.Width = a_width;
		depthBufferDesc.Height = a_height;
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
		viewport.Width = (float) a_width;
		viewport.Height = (float) a_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// Create the viewport.
		m_pDeviceContext->RSSetViewports(1, &viewport);

		// Setup the projection matrix.
		fieldOfView = (float) XM_PI / 4.0f;
		screenAspect = (float) a_width / (float) a_height;

		// Create the projection matrix for 3D rendering.
		m_projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, a_screenNear, a_screenFar);

		// Initialize the world matrix to the identity matrix.
		m_worldMatrix = XMMatrixIdentity();

		// Create an orthographic projection matrix for 2D rendering.
		m_orthoMatrix = XMMatrixOrthographicLH((float) a_width, (float) a_height, a_screenNear, a_screenFar);

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


	void DX11::EndScene()
	{
		// Present the back buffer to the screen since rendering is complete.
		if (m_vsync)
		{
			// Lock to screen refresh rate.
			m_pSwapChain->Present(1, 0);
		}
		else
		{
			// Present as fast as possible.
			m_pSwapChain->Present(0, 0);
		}
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

	void DX11::Shutdown()
	{
		if (m_pSwapChain)
		{
			// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		if (m_pRasterState)
		{
			m_pRasterState->Release();
			m_pRasterState = NULL;
		}

		if (m_pDepthStencilView)
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = NULL;
		}

		if (m_pDepthStencilState)
		{
			m_pDepthStencilState->Release();
			m_pDepthStencilState = NULL;
		}

		if (m_pDepthStencilBuffer)
		{
			m_pDepthStencilBuffer->Release();
			m_pDepthStencilBuffer = NULL;
		}

		if (m_pRenderTargetView)
		{
			m_pRenderTargetView->Release();
			m_pRenderTargetView = NULL;
		}

		if (m_pDeviceContext)
		{
			m_pDeviceContext->Release();
			m_pDeviceContext = NULL;
		}

		if (m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = NULL;
		}

		if (m_pSwapChain)
		{
			m_pSwapChain->Release();
			m_pSwapChain = NULL;
		}
	}

	DX11::~DX11()
	{
		
	}
}