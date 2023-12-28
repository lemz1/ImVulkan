#pragma once
#include "ImVulkan/Core/Core.h"

#include <chrono>

namespace ImVulkan
{
	class Timer
	{
	public:
		Timer()
		{
			Reset();
		}

		void Reset()
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

		float ElapsedSeconds()
		{
			return ElapsedMilliseconds() * .001f;
		}

		float ElapsedMilliseconds()
		{
			return ElapsedMicroseconds() * .001f;
		}

		float ElapsedMicroseconds()
		{
			return ElapsedNanoseconds() * .001f;
		}

		float ElapsedNanoseconds()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
		}
;	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};

	class ScopedTimer : public Timer
	{
	public:
		ScopedTimer(const char* name)
			: m_Name(name)
		{}

		~ScopedTimer()
		{
			IMVK_INFO("[TIMER] " << m_Name << ": " << m_Timer.ElapsedMicroseconds() << "us" << "(" << m_Timer.ElapsedMilliseconds() << "ms)");
		}
	private:
		const char* m_Name;
		Timer m_Timer;
	};
}
