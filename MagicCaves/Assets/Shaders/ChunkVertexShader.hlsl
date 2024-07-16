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
    float4x4 Projection;
    float4x4 View;
    float4x4 Model;
}


PixelShaderInput VSMain(VertexInput input)
{
    PixelShaderInput ps;
    ps.Position = input.Position;
    ps.Color = input.Color;
    
	return ps;
}