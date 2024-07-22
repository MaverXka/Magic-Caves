#pragma once

#include "../Base/RenderPass.h"

class RenderPass_Chunks : public RenderPass
{


public:

	RenderPass_Chunks(class ChunkRendering* NewChunkRenderer);


	virtual void RenderThread_Prepare() override;

	virtual RenderPassType GetRenderPassType() override { return RenderPassType::Dynamic; };

	UINT8* Data = nullptr;

protected:

	class ChunkRendering* ChunkRenderer = nullptr;

};

