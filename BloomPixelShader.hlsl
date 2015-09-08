Texture2D regularTexture;
Texture2D glowTexture;
SamplerState SampleType;

cbuffer GlowBuffer
{
	float glowStrength;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 glowColor;
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = regularTexture.Sample(SampleType, input.tex);

	// Sample the glow texture.
	glowColor = glowTexture.Sample(SampleType, input.tex);

	// Add the texture color to the glow color multiplied by the glow stength.
	color = saturate(textureColor + (glowColor * glowStrength));

	return color;
}