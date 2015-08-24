cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix        g_mWorldViewProjection    : packoffset(c0);
	matrix        g_mWorld					: packoffset(c4);
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 vPosition    : POSITION;
	float3 vNormal        : NORMAL;
	float2 vTexcoord    : TEXCOORD0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float3 vNormal        : NORMAL;
	float2 vTexcoord    : TEXCOORD0;
	float4 vPosition    : SV_POSITION;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    // Transform the vertex position into projected space.
	Output.vPosition = mul(Input.vPosition, g_mWorldViewProjection);
	Output.vNormal = mul(Input.vNormal, (float3x3)g_mWorld);
	Output.vTexcoord = Input.vTexcoord;

    return output;
}
