#include "imvkpch.h"
#include "ImVulkan/Core/LayerStack.h"

namespace ImVulkan
{
	LayerStack::~LayerStack()
	{
		for (auto layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		if (std::find(m_Layers.begin(), m_Layers.end(), layer) == m_Layers.end())
		{
			m_Layers.push_back(layer);
		}
	}

	void LayerStack::RemoveLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}

	void LayerStack::OnUpdate(const double deltaTime)
	{
		for (auto layer : m_Layers)
		{
			layer->OnUpdate(deltaTime);
		}
	}
}
