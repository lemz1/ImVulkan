#pragma once

#include "ImVulkan/Event/Event.h"

namespace ImVulkan
{
	class Layer
	{
	public:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		virtual void OnUpdate(double const deltaTime) {}

		virtual void OnDraw() {}

		virtual void OnDrawImGui() {}

		virtual void OnEvent(Event& event) {}
	};
}
