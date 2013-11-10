#ifndef SYSINFO_H
#define SYSINFO_H

namespace Engine
{
	class SysInfo
	{
	public:
		struct Resolution
		{
			static __declspec(dllexport) int Width();
			static __declspec(dllexport) int Height();
		};

		static __declspec(dllexport) void Fetch();

	private:
		static int m_width;
		static int m_height;
	};
}

#endif // SYSINFO_H