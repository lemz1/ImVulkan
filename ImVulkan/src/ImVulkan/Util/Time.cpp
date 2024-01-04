#include "imvkpch.h"
#ifdef IMVK_HEADLESS
#include "ImVulkan/Util/Time.h"

#include <chrono>

namespace ImVulkan
{
	static std::chrono::time_point<std::chrono::high_resolution_clock> s_StartTime;

	void Time::Init()
	{
		s_StartTime = std::chrono::high_resolution_clock::now();
	}

	double Time::GetTime()
	{
		return Time::GetTimeMS () * .001;
	}

	double Time::GetTimeMS()
	{
		return static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now() - s_StartTime
			).count() * .001
		);
	}
}
#endif
