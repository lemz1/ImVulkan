#pragma once

#include "ImVulkan/Core/Layer.h"

#include <vector>

namespace ImVulkan
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		void Destroy();

		void PushLayer(Layer* layer);
		void RemoveLayer(Layer* layer);

		void OnUpdate(const double deltaTime);

		void OnDraw();

		void OnDrawImGui();

		void OnEvent(Event& event);

		const std::vector<Layer*>& GetLayers() const { return m_Layers; }
	private:
		std::vector<Layer*> m_Layers;
	};
}
