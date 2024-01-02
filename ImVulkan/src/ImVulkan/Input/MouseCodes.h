#pragma once

#include <stdint.h>

namespace ImVulkan
{
	using MouseCode = uint16_t;

	enum class MouseButton : MouseCode
	{
		//from glfw3.h
		Button1           = 0,
		Button2           = 1,
		Button3           = 2,
		Button4           = 3,
		Button5           = 4,
		Button6           = 5,
		Button7           = 6,
		Button8           = 7,

		ButtonLast        = Button8,
		ButtonLeft        = Button1,
		ButtonRight       = Button2,
		ButtonMiddle      = Button3,
	};
}
