#ifndef CONSOLE_H
#define CONSOLE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "String2.h"

#define LOG Logger::Instance().Log
#define LOG_ERROR Logger::Instance().LogError
#define LOG_WARNING Logger::Instance().LogWarning
#define MESSAGE Logger::Instance().Message

namespace Engine
{
	class Logger
	{
	public:
		static __declspec(dllexport) Logger& Instance();

		enum Colours
		{
			WHITE = 0x0F,
			RED = 0x0C,
			YELLOW = 0x0E,
			GREEN = 0x0A,
			BLUE = 0x0B,
			PINK = 0x0D
		};

		__declspec(dllexport) void InitConsole();
		__declspec(dllexport) void ShowConsole();
		__declspec(dllexport) void HideConsole();
		__declspec(dllexport) void Log(std::string a_fmt, ...);
		__declspec(dllexport) void Log(Colours a_colour, std::string a_fmt, ...);
		__declspec(dllexport) void LogWarning(std::string a_fmt, ...);
		__declspec(dllexport) void LogError(std::string a_fmt, ...);
		__declspec(dllexport) void LogError(bool a_getLast, std::string a_fmt, ...);
		__declspec(dllexport) void Message(std::string a_title, std::string a_fmt, ...);
		__declspec(dllexport) void Message(std::string a_fmt, ...);

	private:
		__declspec(dllexport) Logger();
		__declspec(dllexport) Logger(Logger const&);
		__declspec(dllexport) void operator=(Logger const&);

		bool m_console;
		std::string m_last;
		int m_repeats;
		int m_conHandle;
	};
}

#endif // CONSOLE_H