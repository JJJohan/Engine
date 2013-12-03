#include "stdafx.h"
#include "wtypes.h"

namespace Engine
{
	int SysInfo::m_width = 0;
	int SysInfo::m_height = 0;

	void SysInfo::Fetch()
	{
		// Get resolution
		RECT desktop;
		GetWindowRect(GetDesktopWindow(), &desktop);
		m_width = desktop.right;
		m_height = desktop.bottom;
	}

	int SysInfo::Resolution::Width()
	{
		return m_width;
	}

	int SysInfo::Resolution::Height()
	{
		return m_height;
	}
}