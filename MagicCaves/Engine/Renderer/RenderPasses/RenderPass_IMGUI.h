#pragma once

#include "../Base/RenderPass.h"

class RenderPass_IMGUI : public RenderPass
{
public:

	RenderPass_IMGUI();

	virtual void RenderThread_Prepare() override;


};

