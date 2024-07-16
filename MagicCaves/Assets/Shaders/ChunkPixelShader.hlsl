struct PixelShaderInput
{
    float3 Color : COLOR;
};

float4 PSMain(PixelShaderInput input) : SV_TARGET
{
    return float4(input.Color, 1.0f);
}