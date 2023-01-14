#pragma once

#include <Core/Layer.h>

namespace Motus3D
{
	
	class UILayer : public Layer {
	public:
		void OnEvent(Event& e) override;
		void OnUpdate() override;
		void OnAttach() override;
		void OnDetach() override;

		void BeginUI();
		void EndUI();

	private:

	};

}