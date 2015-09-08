Texture2D shaderTexture;
SamplerState SampleType;

cbuffer ThresholdBuffer
{
	float	luminanceThreshold;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color;

	// Initialize the color to black.
	color = shaderTexture.Sample(SampleType, input.tex);

	// Based on http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
	float luminance = (0.299f*color.r) + (0.587f *color.g) + (0.114*color.b);
	if (luminance < luminanceThreshold){
		discard;
	}
	color.a = 1.0f;
	return color;
}