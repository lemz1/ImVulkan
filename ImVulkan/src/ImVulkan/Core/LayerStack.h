#pragma once

#include "ImVulkan/Core/Layer.h"

#include <vector>

namespace ImVulkan
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void RemoveLayer(Layer* layer);

		void OnUpdate(const double deltaTime);

		const std::vector<Layer*>& GetLayers() const { return m_Layers; }
	private:
		std::vector<Layer*> m_Layers;
	};
}
