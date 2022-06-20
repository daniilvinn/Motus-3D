#pragma once

#include <Core/Macros.h>
#include <Core/Layer.h>

namespace Motus3D
{
	// Represents stack of layers, which will be updated and rendered sequentially
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void AddLayer(Layer* layer);
		// TODO: AddOverlay()

		std::vector<Layer*>::iterator begin() { return m_LayerStack.begin(); }
		std::vector<Layer*>::iterator end() { return m_LayerStack.end(); }

	private:

		std::vector<Layer*> m_LayerStack;

	};
}

