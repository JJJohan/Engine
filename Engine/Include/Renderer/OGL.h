#ifndef OGL_H
#define OGL_H

#include "stdafx.h"

namespace Engine
{
	class OGL
	{
	public:
		OGL();

		bool InitialiseExtensions(HWND a_hwnd);
		bool InitialiseOpenGL(int a_width, int a_height, bool a_vsync, HWND a_hwnd, float a_screenNear, float a_screenFar);
		void Release(HWND a_hwnd);
		bool MakeCurrent();

		void BeginScene();
		void EndScene();

		Matrix GetWorldMatrix();
		Matrix GetProjectionMatrix();

		void SetClearColour(float a_red, float a_green, float a_blue, float a_alpha);
		void GetVideoCardInfo(char* a_cardName);

		Matrix BuildPerspectiveFovLHMatrix(float a_fieldOfView, float a_screenAspect, float a_screenNear, float a_screenFar);
		Matrix MatrixRotationY(float a_angle);

		PFNGLATTACHSHADERPROC glAttachShader;
		PFNGLBINDBUFFERPROC glBindBuffer;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		PFNGLBUFFERDATAPROC glBufferData;
		PFNGLCOMPILESHADERPROC glCompileShader;
		PFNGLCREATEPROGRAMPROC glCreateProgram;
		PFNGLCREATESHADERPROC glCreateShader;
		PFNGLDELETEBUFFERSPROC glDeleteBuffers;
		PFNGLDELETEPROGRAMPROC glDeleteProgram;
		PFNGLDELETESHADERPROC glDeleteShader;
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		PFNGLDETACHSHADERPROC glDetachShader;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		PFNGLGENBUFFERSPROC glGenBuffers;
		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		PFNGLGETPROGRAMIVPROC glGetProgramiv;
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
		PFNGLGETSHADERIVPROC glGetShaderiv;
		PFNGLLINKPROGRAMPROC glLinkProgram;
		PFNGLSHADERSOURCEPROC glShaderSource;
		PFNGLUSEPROGRAMPROC glUseProgram;
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
		PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
		PFNGLACTIVETEXTUREPROC glActiveTexture;
		PFNGLUNIFORM1IPROC glUniform1i;
		PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
		PFNGLUNIFORM3FVPROC glUniform3fv;
		PFNGLUNIFORM4FVPROC glUniform4fv;

	private:
		bool LoadExtensionList();

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

		HDC m_pDeviceContext;
		HGLRC m_pRenderingContext;
		Matrix m_worldMatrix;
		Matrix m_projMatrix;
		char m_cardDesc[128];
		float m_clearColour[4];
	};
}

#endif // OGL_H