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
			layer->OnCreate();
		}
	}

	void LayerStack::RemoveLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			layer->OnDestroy();
		}
	}

	void LayerStack::OnUpdate(const double deltaTime)
	{
		for (auto layer : m_Layers)
		{
			layer->OnUpdate(deltaTime);
		}
	}

	void LayerStack::OnDraw()
	{
		for (auto layer : m_Layers)
		{
			layer->OnDraw();
		}
	}

	void LayerStack::OnDrawImGui()
	{
		for (auto layer : m_Layers)
		{
			layer->OnDrawImGui();
		}
	}

	void LayerStack::OnEvent(Event& event)
	{
		for (auto layer : m_Layers)
		{
			layer->OnEvent(event);
		}
	}
}
