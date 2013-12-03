#include "stdafx.h"
#include <iostream>

namespace Engine
{
	OGL::OGL()
	{
		m_pDeviceContext = NULL;
		m_pRenderingContext = NULL;

		SetClearColour(0.0f, 0.0f, 0.25f, 1.0f);
	}

	bool OGL::InitialiseExtensions(HWND a_hwnd)
	{
		HDC deviceContext;
		PIXELFORMATDESCRIPTOR pixelFormat;
		int error;
		HGLRC renderContext;
		bool result;

		// Get the device context for this window.
		deviceContext = GetDC(a_hwnd);
		if (!deviceContext)
		{
			LOG_ERROR("Extensions - Could not get device context.");
			return false;
		}

		// Set a temporary default pixel format.
		error = SetPixelFormat(deviceContext, 1, &pixelFormat);
		if (error != 1)
		{
			LOG_ERROR("Extensions - Could not set pixel format.");
			return false;
		}

		// Create a temporary rendering context.
		renderContext = wglCreateContext(deviceContext);
		if (!renderContext)
		{
			LOG_ERROR("Extensions - Could not create rendering context.");
			return false;
		}

		// Set the temporary rendering context as the current rendering context for this window.
		error = wglMakeCurrent(deviceContext, renderContext);
		if (error != 1)
		{
			LOG_ERROR("Extensions - Could not set current rendering context.");
			return false;
		}

		// Initialise the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
		result = LoadExtensionList();
		if (!result)
		{
			LOG_ERROR("Extensions - Could not load extension list.");
			return false;
		}

		// Release the temporary rendering context now that the extensions have been loaded.
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(renderContext);
		renderContext = NULL;

		// Release the device context for this window.
		ReleaseDC(a_hwnd, deviceContext);
		deviceContext = 0;

		return true;
	}

	bool OGL::MakeCurrent()
	{
		int result = wglMakeCurrent(m_pDeviceContext, m_pRenderingContext);
		if (result != 1)
		{
			LOG_ERROR("Could not set rendering context.");
			return false;
		}

		return true;
	}

	bool OGL::InitialiseOpenGL(int a_width, int a_height, bool a_vsync, HWND a_hwnd, float a_screenNear, float a_screenFar)
	{
		int attributeListInt[19];
		int pixelFormat[1];
		unsigned int formatCount;
		int result;
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
		int attributeList[5];
		float fieldOfView, screenAspect;
		char *vendorString, *rendererString;

		// Get the device context for this window.
		m_pDeviceContext = GetDC(a_hwnd);
		if (!m_pDeviceContext)
		{
			LOG_ERROR("Could not get device context.");
			return false;
		}

		// Support for OpenGL rendering.
		attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
		attributeListInt[1] = TRUE;

		// Support for rendering to a window.
		attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
		attributeListInt[3] = TRUE;

		// Support for hardware acceleration.
		attributeListInt[4] = WGL_ACCELERATION_ARB;
		attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

		// Support for 24bit color.
		attributeListInt[6] = WGL_COLOR_BITS_ARB;
		attributeListInt[7] = 24;

		// Support for 24 bit depth buffer.
		attributeListInt[8] = WGL_DEPTH_BITS_ARB;
		attributeListInt[9] = 24;

		// Support for double buffer.
		attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
		attributeListInt[11] = TRUE;

		// Support for swapping front and back buffer.
		attributeListInt[12] = WGL_SWAP_METHOD_ARB;
		attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

		// Support for the RGBA pixel type.
		attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
		attributeListInt[15] = WGL_TYPE_RGBA_ARB;

		// Support for a 8 bit stencil buffer.
		attributeListInt[16] = WGL_STENCIL_BITS_ARB;
		attributeListInt[17] = 8;

		// Null terminate the attribute list.
		attributeListInt[18] = 0;

		// Query for a pixel format that fits the attributes we want.
		result = wglChoosePixelFormatARB(m_pDeviceContext, attributeListInt, NULL, 1, pixelFormat, &formatCount);
		if (result != 1)
		{
			LOG_ERROR("Could not choose pixel format.");
			return false;
		}

		// If the video card/display can handle our desired pixel format then we set it as the current one.
		result = SetPixelFormat(m_pDeviceContext, pixelFormat[0], &pixelFormatDescriptor);
		if (result != 1)
		{
			LOG_ERROR("Could not set pixel format.");
			return false;
		}

		// Set the 4.0 version of OpenGL in the attribute list.
		attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attributeList[1] = 4;
		attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attributeList[3] = 0;

		// Null terminate the attribute list.
		attributeList[4] = 0;

		// Create a OpenGL 4.0 rendering context.
		m_pRenderingContext = wglCreateContextAttribsARB(m_pDeviceContext, 0, attributeList);
		if (m_pRenderingContext == NULL)
		{
			LOG_ERROR("Could not create rendering context.");
			return false;
		}

		// Set the rendering context to active.
		result = wglMakeCurrent(m_pDeviceContext, m_pRenderingContext);
		if (result != 1)
		{
			LOG_ERROR("Could not set rendering context.");
			return false;
		}

		// Set the depth buffer to be entirely cleared to 1.0 values.
		glClearDepth(1.0f);

		// Enable depth testing.
		glEnable(GL_DEPTH_TEST);

		// Set the polygon winding to front facing for the left handed system.
		glFrontFace(GL_CW);

		// Enable back face culling.
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Initialize the world/model matrix to the identity matrix.
		m_worldMatrix = Matrix::Identity();

		// Set the field of view and screen aspect ratio.
		fieldOfView = 3.14159265358979323846f / 4.0f;
		screenAspect = (float) a_width / (float) a_height;

		// Build the perspective projection matrix.
		m_projMatrix = BuildPerspectiveFovLHMatrix(fieldOfView, screenAspect, a_screenNear, a_screenFar);

		// Get the name of the video card.
		vendorString = (char*) glGetString(GL_VENDOR);
		rendererString = (char*) glGetString(GL_RENDERER);

		// Store the video card name in a class member variable so it can be retrieved later.
		strcpy_s(m_cardDesc, vendorString);
		strcat_s(m_cardDesc, " - ");
		strcat_s(m_cardDesc, rendererString);

		// Turn on or off the vertical sync depending on the input bool value.
		if (a_vsync)
		{
			result = wglSwapIntervalEXT(1);
		}
		else
		{
			result = wglSwapIntervalEXT(0);
		}

		// Check if vsync was set correctly.
		if (result != 1)
		{
			LOG_ERROR("Could not set vsync.");
			return false;
		}

		// Remove current render context for render thread.
		wglMakeCurrent(0, 0);

		return true;
	}

	void OGL::Release(HWND a_hwnd)
	{
		// Release the rendering context.
		if (m_pRenderingContext)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_pRenderingContext);
			m_pRenderingContext = NULL;
		}

		// Release the device context.
		if (m_pDeviceContext)
		{
			ReleaseDC(a_hwnd, m_pDeviceContext);
			m_pDeviceContext = NULL;
		}

		LOG("OGL released.");
	}

	bool OGL::LoadExtensionList()
	{
		// Load the OpenGL extensions that this application will be using.
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
		if (!wglChoosePixelFormatARB)
			return false;

		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		if (!wglCreateContextAttribsARB)
			return false;

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
		if (!wglSwapIntervalEXT)
			return false;

		glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
		if (!glAttachShader)
			return false;

		glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
		if (!glBindBuffer)
			return false;

		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) wglGetProcAddress("glBindVertexArray");
		if (!glBindVertexArray)
			return false;

		glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
		if (!glBufferData)
			return false;

		glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
		if (!glCompileShader)
			return false;

		glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
		if (!glCreateProgram)
			return false;

		glCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
		if (!glCreateShader)
			return false;

		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
		if (!glDeleteBuffers)
			return false;

		glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
		if (!glDeleteProgram)
			return false;

		glDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
		if (!glDeleteShader)
			return false;

		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) wglGetProcAddress("glDeleteVertexArrays");
		if (!glDeleteVertexArrays)
			return false;

		glDetachShader = (PFNGLDETACHSHADERPROC) wglGetProcAddress("glDetachShader");
		if (!glDetachShader)
			return false;

		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
		if (!glEnableVertexAttribArray)
			return false;

		glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
		if (!glGenBuffers)
			return false;

		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) wglGetProcAddress("glGenVertexArrays");
		if (!glGenVertexArrays)
			return false;

		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress("glGetAttribLocation");
		if (!glGetAttribLocation)
			return false;

		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
		if (!glGetProgramInfoLog)
			return false;

		glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
		if (!glGetProgramiv)
			return false;

		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
		if (!glGetShaderInfoLog)
			return false;

		glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
		if (!glGetShaderiv)
			return false;

		glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
		if (!glLinkProgram)
			return false;

		glShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
		if (!glShaderSource)
			return false;

		glUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
		if (!glUseProgram)
			return false;

		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
		if (!glVertexAttribPointer)
			return false;

		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) wglGetProcAddress("glBindAttribLocation");
		if (!glBindAttribLocation)
			return false;

		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
		if (!glGetUniformLocation)
			return false;

		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) wglGetProcAddress("glUniformMatrix4fv");
		if (!glUniformMatrix4fv)
			return false;

		glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
		if (!glActiveTexture)
			return false;

		glUniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
		if (!glUniform1i)
			return false;

		glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
		if (!glGenerateMipmap)
			return false;

		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
		if (!glDisableVertexAttribArray)
			return false;

		glUniform3fv = (PFNGLUNIFORM3FVPROC) wglGetProcAddress("glUniform3fv");
		if (!glUniform3fv)
			return false;

		glUniform4fv = (PFNGLUNIFORM4FVPROC) wglGetProcAddress("glUniform4fv");
		if (!glUniform4fv)
			return false;

		return true;
	}

	Matrix OGL::GetWorldMatrix()
	{
		return m_worldMatrix;
	}

	Matrix OGL::GetProjectionMatrix()
	{
		return m_projMatrix;
	}

	void OGL::GetVideoCardInfo(char* a_cardName)
	{
		strcpy_s(a_cardName, 128, m_cardDesc);
	}

	Matrix OGL::BuildPerspectiveFovLHMatrix(float a_fieldOfView, float a_screenAspect, float a_screenNear, float a_screenDepth)
	{
		return Matrix(
			1.0f / (a_screenAspect * tan(a_fieldOfView * 0.5f)),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f / tan(a_fieldOfView * 0.5f),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			a_screenDepth / (a_screenDepth - a_screenNear),
			1.0f,
			0.0f,
			0.0f,
			(-a_screenNear * a_screenDepth) / (a_screenDepth - a_screenNear),
			0.0f);
	}

	Matrix OGL::MatrixRotationY(float a_angle)
	{
		float cs = cosf(a_angle);
		float sn = sinf(a_angle);

		return Matrix(
			cs,
			0.0f,
			-sn,
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,
			sn,
			0.0f,
			cs,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f);
	}

	void OGL::SetClearColour(float a_red, float a_green, float a_blue, float a_alpha)
	{
		m_clearColour[0] = a_red;
		m_clearColour[1] = a_green;
		m_clearColour[2] = a_blue;
		m_clearColour[3] = a_alpha;
	}

	void OGL::BeginScene()
	{
		// Set the color to clear the screen to.
		glClearColor(m_clearColour[0], m_clearColour[1], m_clearColour[2], m_clearColour[3]);

		// Clear the screen and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void OGL::EndScene()
	{
		// Present the back buffer to the screen since rendering is complete.
		SwapBuffers(m_pDeviceContext);
	}
}