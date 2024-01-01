#pragma once

namespace ImVulkan
{
	class Layer
	{
	public:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};

		virtual void OnUpdate(double const deltaTime) {};

		virtual void OnDraw() {};
	};
}
