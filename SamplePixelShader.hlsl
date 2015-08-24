cbuffer cbPerObject : register(b0)
{
	float4        g_vObjectColor            : packoffset(c0);
};

cbuffer cbPerFrame : register(b1)
{
	float3        g_vLightDir                : packoffset(c0);
	float        g_fAmbient : packoffset(c0.w);
};

Texture2D    g_txDiffuse : register(t0);
SamplerState g_samLinear : register(s0);

struct PixelShaderInput
{
	float3 vNormal        : NORMAL;
	float2 vTexcoord    : TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 vDiffuse = g_txDiffuse.Sample(g_samLinear, Input.vTexcoord);

	float fLighting = saturate(dot(g_vLightDir, Input.vNormal));
	fLighting = max(fLighting, g_fAmbient);

	return vDiffuse * fLighting;
}
