#pragma once

#include <Visus/Core/Macros.h>

struct ImGuiContext;

namespace Motus3D {

	void VISUS_API InitUI(void* windowHandle);
	void VISUS_API ShutdownUI(void* windowHandle);
	void VISUS_API RenderUI();
	void VISUS_API BeginFrameUI();
	void VISUS_API EndFrameUI();

	VISUS_API ImGuiContext* GetImGuiContext();


}