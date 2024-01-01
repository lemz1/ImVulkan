#pragma once

namespace ImVulkan
{
	class Layer
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;

		virtual void OnUpdate(double const deltaTime) = 0;
	};
}
