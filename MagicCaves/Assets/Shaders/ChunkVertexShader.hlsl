struct VertexInput
{
    float4 Position : POSITION;
    float2 TexCoords : TEXCOORD;
    float3 Color : COLOR;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};


cbuffer ConstantBuffer : register(b0)
{
    float4x4 ProjectionView;
    float c;
    float padding[47];
}


PixelShaderInput VSMain(VertexInput input)
{
    PixelShaderInput ps = (PixelShaderInput)0;
    ps.Position = mul(input.Position,ProjectionView);
    //ps.Position = input.Position;
    ps.Color = input.Color;
    
	return ps;
}