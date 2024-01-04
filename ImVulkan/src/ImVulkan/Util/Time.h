#pragma once

#include <stdint.h>

namespace ImVulkan
{
	class Time
	{
	public:
		static void Init();

		static double GetTime();

		static double GetTimeMS();
	};
}
