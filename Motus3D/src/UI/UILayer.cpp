#include "UILayer.h"

#include <Visus.h>

namespace Motus3D {

	void UILayer::OnUpdate()
	{
		
	}

	void UILayer::OnEvent(Event& e)
	{
		
	}

	void UILayer::OnAttach()
	{
		InitUI(Renderer::GetConfiguration().windowHandle);
	}

	void UILayer::OnDetach()
	{
		ShutdownUI(Renderer::GetConfiguration().windowHandle);
	}

	void UILayer::BeginUI()
	{
		
	}

	void UILayer::EndUI()
	{

	}

}