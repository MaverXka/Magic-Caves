#pragma once

#include "../Base/RenderPass.h"
#include "../World/Chunks/ChunkData.h"

struct ChunkConstantBuffer;

class RenderPass_Chunks : public RenderPass
{


public:

	RenderPass_Chunks(class ChunkRendering* NewChunkRenderer);
	~RenderPass_Chunks();


	virtual void RenderThread_Prepare() override;

	virtual RenderPassType GetRenderPassType() override { return RenderPassType::Dynamic; };

	UINT8* Data = nullptr;

protected:

	struct ChunkConstantBuffer constantBuffer;

	void UpdateConstantBuffer(int OffsetX, int OffsetY);

	class ChunkRendering* ChunkRenderer = nullptr;

};

