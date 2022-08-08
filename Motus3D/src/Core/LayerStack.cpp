#include "LayerStack.h"

namespace Motus3D
{
	
	LayerStack::LayerStack()
	{
		     
	}
	
	LayerStack::~LayerStack()
	{
		//for (auto& layer : m_LayerStack)
		//{
		//	layer->OnDetach();
		//	delete layer;
		//}
	}
	
	void LayerStack::AddLayer(Layer* layer)
	{
		m_LayerStack.push_back(layer);
		layer->OnAttach();
	}

}
