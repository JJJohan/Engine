#include "stdafx.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sstream>
#include <iostream>
#include <fstream>

namespace Engine
{
	Logger& Logger::Instance()
	{
		static Logger instance;
		return instance;
	}

	Logger::Logger()
	{
		m_console = false;
		m_last = "";
		m_repeats = 0;
		m_conHandle = NULL;
	}

	void Logger::InitConsole()
	{
#ifdef _DEBUG
		if (m_console)
			return;

		// maximum mumber of lines the output console should have
		static const WORD MAX_CONSOLE_LINES = 500;

		long lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE *fp;

		// allocate a console for this app
		AllocConsole();
		
		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),	&coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;

		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),	coninfo.dwSize);

		// redirect unbuffered STDOUT to the console
		lStdHandle = (long) GetStdHandle(STD_OUTPUT_HANDLE);
		m_conHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(m_conHandle, "w");
		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);

		// redirect unbuffered STDIN to the console
		lStdHandle = (long) GetStdHandle(STD_INPUT_HANDLE);
		m_conHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(m_conHandle, "r");
		*stdin = *fp;
		setvbuf(stdin, NULL, _IONBF, 0);

		// redirect unbuffered STDERR to the console
		lStdHandle = (long) GetStdHandle(STD_ERROR_HANDLE);
		m_conHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(m_conHandle, "w");
		*stderr = *fp;
		setvbuf(stderr, NULL, _IONBF, 0);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
		m_console = true; 
#endif
	}

	void Logger::HideConsole()
	{
		if (!m_console)
			LogWarning("Console not initialised.");
		else
			ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	void Logger::ShowConsole()
	{
		if (!m_console)
			LogWarning("Console not initialised.");
		else
			ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	void Logger::Log(std::string a_fmt, ...) 
	{ 
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		Log(Colours::WHITE, std::string(buff));
		SAFE_DELETE(buff);
		va_end(args);
	}

	void Logger::Log(Colours a_colour, std::string a_fmt, ...)
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		std::string fmt = buff;
		SAFE_DELETE(buff);
		va_end(args);

		// Print message to console
		if (m_console)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a_colour);
			if (m_last == fmt)
			{
				++m_repeats;

				// Get last line
				CONSOLE_SCREEN_BUFFER_INFO csbi;		
				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
				COORD result = csbi.dwCursorPosition;
				if (result.Y > 0) result.Y -= 1;
				result.X = 0;

				// Print repeated text
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), result);
				std::cout << fmt;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Colours::PINK);
				std::cout << " [x" << m_repeats << "]" << std::endl;
			}
			else
			{
				// Print text
				m_last = fmt;
				m_repeats = 0;
				std::cout << fmt << std::endl;
			}
		}

#if defined(_DEBUG)
		fmt.append("\n");
		std::wstring wbuff = std::wstring(fmt.begin(), fmt.end());
		OutputDebugString(wbuff.c_str());
#endif
	}

	void Logger::LogWarning(std::string a_fmt, ...)
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		std::string fmt = buff;
		SAFE_DELETE(buff);
		va_end(args);

		// Log a warning
		Log(Colours::YELLOW, "[WARNING] " + fmt);
	}

	void Logger::LogError(std::string a_fmt, ...) 
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		LogError(false, std::string(buff));
		SAFE_DELETE(buff);
		va_end(args);
	}

	void Logger::LogError(bool a_getLast, std::string a_fmt, ...)
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		std::string fmt = buff;
		SAFE_DELETE(buff);
		va_end(args);

		// Log an error and message user.
		LPTSTR errorText = 0;
		std::stringstream ss;
		ss << fmt;
		if (a_getLast)
		{
			// Translate error code to string.
			if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, (LPTSTR) &errorText,
				0, NULL))
			{
				std::wstring wtext = errorText;
				std::string text = std::string(wtext.begin(), wtext.end());
				std::stringstream ss;
				ss << "\nError " << GetLastError() << ": " << text;
				SetLastError(0);
			}
		}	

		Log(Colours::RED, "[ERROR] " + ss.str());
		Message("Error", ss.str());
			
#if _DEBUG
		//__debugbreak();
#endif
	}

	void Logger::Message(std::string a_fmt, ...)
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		Message("", std::string(buff));
		SAFE_DELETE(buff);
		va_end(args);
	}

	void Logger::Message(std::string a_title, std::string a_fmt, ...)
	{
		va_list args;
		va_start(args, a_fmt);
		int length = (_vscprintf(a_fmt.c_str(), args) * sizeof(char)) + 1;
		char* buff = new char[length];
		vsprintf_s(buff, length, a_fmt.c_str(), args);
		va_end(args);
		std::string fmt = buff;
		SAFE_DELETE(buff);

		// Message user	
		if (APPLICATION)
		{
			if (a_title.empty())
				a_title = APPLICATION->GetTitle();
			else
				a_title = APPLICATION->GetTitle() + " - " + a_title;
		}
		else
		{
			if (a_title.empty())
				a_title = "Message";
		}

		std::wstring title = std::wstring(a_title.begin(), a_title.end());
		std::wstring text = std::wstring(fmt.begin(), fmt.end());
		MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
	}
}