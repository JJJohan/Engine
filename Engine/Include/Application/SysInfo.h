#ifndef SYSINFO_H
#define SYSINFO_H

#include <string>

namespace Engine
{
	class __declspec(dllexport) SysInfo
	{
	public:
		static struct Screen
		{
			static int width;
			static int height;
			static int bpp;
			static int monitors;
		};

		static struct CPU
		{
			static int cpus;
			static int cores_logical;
			static int cores_physical;
			static bool hyperthreading;
			static std::string name;
			static int cache_l1;
			static int cache_l2;
			static int cache_l3;
		};

		static struct OS
		{
			static int build;
			static int major;
			static int minor;
		};

		static struct Memory
		{
			static int total;
			static int available;
			static int used;
		};

		static struct Video
		{
			static int memory;
			static std::string name;
			static int dx_major;
			static int dx_minor;
		};

		static void Refresh();
		static void Print();

	private:
		SysInfo(SysInfo const&);
		void operator=(SysInfo const&);

		static std::string GetProcessorName();
		static DWORD CountSetBits(ULONG_PTR bitMask);
		static void GetCPUInfo();
		static void GetVideoInfo();
	};
}

#endif // SYSINFO_H