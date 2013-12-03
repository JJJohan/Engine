#include "stdafx.h"
#include "wtypes.h"
#include <malloc.h>    
#include <stdio.h>
#include <tchar.h>

namespace Engine
{
	void cpuID(unsigned i, unsigned regs[4]) {
#ifdef _WIN32
		__cpuid((int *) regs, (int) i);

#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
			: "a" (i), "c" (0));
		// ECX is set to zero for CPUID function 4
#endif
	}

	int SysInfo::Screen::width = 0;
	int SysInfo::Screen::height = 0;
	int SysInfo::Screen::bpp = 0;
	int SysInfo::Screen::monitors = 0;

	int SysInfo::OS::build = 0;
	int SysInfo::OS::major = 0;
	int SysInfo::OS::minor = 0;

	int SysInfo::Memory::available = 0;
	int SysInfo::Memory::total = 0;
	int SysInfo::Memory::used = 0;

	int SysInfo::CPU::cache_l1 = 0;
	int SysInfo::CPU::cache_l2 = 0;
	int SysInfo::CPU::cache_l3 = 0;
	int SysInfo::CPU::cpus = 0;
	int SysInfo::CPU::cores_logical = 0;
	int SysInfo::CPU::cores_physical = 0;
	bool SysInfo::CPU::hyperthreading = false;
	std::string SysInfo::CPU::name = "";

	int SysInfo::Video::memory = 0;
	std::string SysInfo::Video::name = "";
	int SysInfo::Video::dx_major = 0;
	int SysInfo::Video::dx_minor = 0;

	void SysInfo::Refresh()
	{
		// Get resolution
		RECT desktop;
		GetWindowRect(GetDesktopWindow(), &desktop);
		Screen::width = desktop.right;
		Screen::height = desktop.bottom;

		// OS Info
		OSVERSIONINFO info;
		ZeroMemory(&info, sizeof(OSVERSIONINFO));
		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&info))
		{
			OS::build = info.dwBuildNumber;
			OS::major = info.dwMajorVersion;
			OS::minor = info.dwMinorVersion;
		}

		// Monitor
		Screen::monitors = GetSystemMetrics(SM_CMONITORS);
		HDC dc = GetDC(NULL);
		Screen::bpp = GetDeviceCaps(dc, BITSPIXEL);
		ReleaseDC(NULL, dc);

		// CPU
		CPU::name = String(GetProcessorName()).StripWhitespace().StdString();
		GetCPUInfo();

		// Memory
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		Memory::available = (int)status.ullTotalPhys;
		Memory::used = Memory::available - (int)status.ullAvailPhys;
		ULONGLONG memcount;
		if (GetPhysicallyInstalledSystemMemory(&memcount))
		{
			Memory::total = (int)memcount;
		}

		// Video
		GetVideoInfo();
	}

	void SysInfo::Print()
	{
		LOG_LINE;
		LOG("SYSTEM INFO\n");
		if (Screen::monitors > 1)
			LOG("Primary Resolution: %i x %i x %i - (%i monitor)", Screen::width, Screen::height, Screen::bpp, Screen::monitors);
		else
			LOG("Primary Resolution: %i x %i x %i - (%i monitors)", Screen::width, Screen::height, Screen::bpp, Screen::monitors);
		LOG("OS: Windows %i.%i (Build %i)", OS::major, OS::minor, OS::build);
		if (CPU::hyperthreading)
			LOG("CPU: %s - %i Cores (%i Logical) - HT: Yes", CPU::name.c_str(), CPU::cores_physical, CPU::cores_logical);
		else
			LOG("CPU: %s - %i Cores (%i Logical) - HT: No", CPU::name.c_str(), CPU::cores_physical, CPU::cores_logical);
		LOG("System Memory: %i MB (%i MB Available)", Memory::total/1024, Memory::available/1024/1024);
		LOG("Graphics Card: %s (%i MB) - DirectX: %i.%i", Video::name.c_str(), Video::memory, Video::dx_major, Video::dx_minor);

		LOG_LINE;
	}

	std::string SysInfo::GetProcessorName()
	{
		int CPUInfo[4] = { -1 };
		char CPUBrandString[0x40];
		__cpuid(CPUInfo, 0x80000000);
		unsigned int nExIds = CPUInfo[0];

		memset(CPUBrandString, 0, sizeof(CPUBrandString));

		// Get the information associated with each extended ID.
		for (unsigned int i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string.
			if (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}

		return std::string(CPUBrandString);
	}

	// Helper function to count set bits in the processor mask.
	DWORD SysInfo::CountSetBits(ULONG_PTR bitMask)
	{
		DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
		DWORD bitSetCount = 0;
		ULONG_PTR bitTest = (ULONG_PTR) 1 << LSHIFT;
		DWORD i;

		for (i = 0; i <= LSHIFT; ++i)
		{
			bitSetCount += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}

		return bitSetCount;
	}

	typedef BOOL(WINAPI *LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);

	void SysInfo::GetCPUInfo()
	{
		LPFN_GLPI glpi;
		BOOL done = FALSE;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
		DWORD returnLength = 0;
		DWORD logicalProcessorCount = 0;
		DWORD numaNodeCount = 0;
		DWORD processorCoreCount = 0;
		DWORD processorL1CacheCount = 0;
		DWORD processorL2CacheCount = 0;
		DWORD processorL3CacheCount = 0;
		DWORD processorPackageCount = 0;
		DWORD byteOffset = 0;
		PCACHE_DESCRIPTOR Cache;

		glpi = (LPFN_GLPI) GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (NULL == glpi)
		{
			_tprintf(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
			return;
		}

		while (!done)
		{
			DWORD rc = glpi(buffer, &returnLength);

			if (FALSE == rc)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						free(buffer);

					buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) malloc(
						returnLength);

					if (NULL == buffer)
					{
						_tprintf(TEXT("\nError: Allocation failure\n"));
						return;
					}
				}
				else
				{
					_tprintf(TEXT("\nError %d\n"), GetLastError());
					return;
				}
			}
			else
			{
				done = TRUE;
			}
		}

		ptr = buffer;

		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationNumaNode:
				// Non-NUMA systems report a single record of this type.
				numaNodeCount++;
				break;

			case RelationProcessorCore:
				processorCoreCount++;

				// A hyperthreaded core supplies more than one logical processor.
				logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
				break;

			case RelationCache:
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
				Cache = &ptr->Cache;
				if (Cache->Level == 1)
				{
					processorL1CacheCount++;
				}
				else if (Cache->Level == 2)
				{
					processorL2CacheCount++;
				}
				else if (Cache->Level == 3)
				{
					processorL3CacheCount++;
				}
				break;

			case RelationProcessorPackage:
				// Logical processors share a physical package.
				processorPackageCount++;
				break;

			default:
				_tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		CPU::cache_l1 = processorL1CacheCount;
		CPU::cache_l2 = processorL2CacheCount;
		CPU::cache_l3 = processorL3CacheCount;
		CPU::cpus = processorPackageCount;
		CPU::cores_logical = logicalProcessorCount;
		CPU::cores_physical = processorCoreCount;
		CPU::hyperthreading = (logicalProcessorCount == processorCoreCount) ? true : false;

		free(buffer);
	}

	void SysInfo::GetVideoInfo()
	{
		HRESULT result;

		// Create a DirectX graphics interface factory.
		IDXGIFactory* factory;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
		if (FAILED(result))
		{
			LOG_WARNING("Could not create DXGI Factory.");
			return;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			LOG_WARNING("Could not create graphics adapter.");
			return;
		}

		// Enumerate the primary adapter output (monitor).
		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			LOG_WARNING("Could not create graphics adapter output.");
			return;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		unsigned int displaymodes;
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displaymodes, NULL);
		if (FAILED(result))
		{
			LOG_WARNING("Could not create display mode list.");
			return;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[displaymodes];
		if (!displayModeList)
		{
			LOG_WARNING("Could not fill display mode list.");
			return;
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displaymodes, displayModeList);
		if (FAILED(result))
		{
			LOG_WARNING("Could not fill display mode list structures.");
			return;
		}

		// Get the adapter (video card) description
		DXGI_ADAPTER_DESC adapterDesc;
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			LOG_WARNING("Could not get graphics device description.");
			return;
		}

		// Store the dedicated video card memory in megabytes.
		Video::memory = (int) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		unsigned int stringLength;
		char cardDesc[128];
		int error = wcstombs_s(&stringLength, cardDesc, 128, adapterDesc.Description, 128);
		Video::name = cardDesc;
		if (error != 0)
		{
			LOG_WARNING("Could not get graphics device name.");
		}

		// Feature list to try and load.
		D3D_FEATURE_LEVEL features[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, 
										 D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1};

		// Create temporary device.
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;
		D3D_FEATURE_LEVEL pFeature;
		result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, features, ARRAY_SIZE(features), D3D11_SDK_VERSION, &pDevice, &pFeature, &pContext);
		if (FAILED(result))
		{
			LOG_WARNING("Could not query graphics device.");
		}

		int test = ARRAY_SIZE(features);

		// Get feature level.
		if (pFeature == D3D_FEATURE_LEVEL_11_1) { Video::dx_major = 11; Video::dx_minor = 1; }
		else if (pFeature == D3D_FEATURE_LEVEL_11_0) { Video::dx_major = 11; Video::dx_minor = 0; }
		else if (pFeature == D3D_FEATURE_LEVEL_10_1) { Video::dx_major = 10; Video::dx_minor = 1; }
		else if (pFeature == D3D_FEATURE_LEVEL_10_0) { Video::dx_major = 10; Video::dx_minor = 0; }
		else if (pFeature == D3D_FEATURE_LEVEL_9_3) { Video::dx_major = 9; Video::dx_minor = 3; }
		else if (pFeature == D3D_FEATURE_LEVEL_9_2) { Video::dx_major = 9; Video::dx_minor = 2; }
		else if (pFeature == D3D_FEATURE_LEVEL_9_1) { Video::dx_major = 9; Video::dx_minor = 1; }
		else { Video::dx_major = 0; Video::dx_minor = 0; }

		// Release the display mode list.
		delete [] displayModeList;
		displayModeList = 0;

		// Release resources.
		RELEASE(adapterOutput);
		RELEASE(adapter);
		RELEASE(factory);
		RELEASE(pContext);
		RELEASE(pDevice);
	}
}