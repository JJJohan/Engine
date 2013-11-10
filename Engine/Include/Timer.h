#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

namespace Engine
{
	class Timer
	{
		LARGE_INTEGER lFreq, lStart;

	public:
		Timer()
		{
			QueryPerformanceFrequency(&lFreq);
		}

		inline void Start()
		{
			QueryPerformanceCounter(&lStart);
		}

		inline double Stop()
		{
			// Return duration in seconds...
			LARGE_INTEGER lEnd;
			QueryPerformanceCounter(&lEnd);
			return (double(lEnd.QuadPart - lStart.QuadPart) / lFreq.QuadPart);
		}
	};
}

#endif // TIMER_H