Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
};

float4 HorizontalBlurPixelShader(PixelInputType input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3, weight4;
	float normalization;
	float4 color;

	//Average the color of the pixel in center. Add weights to add more effect to the closest neighbor
	weight0 = 1.0f;
	weight1 = 0.9f;
	weight2 = 0.55f;
	weight3 = 0.18f;
	weight4 = 0.1f;

	normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	color += shaderTexture.Sample(SampleType, input.texCoord1) * weight4;
	color += shaderTexture.Sample(SampleType, input.texCoord2) * weight3;
	color += shaderTexture.Sample(SampleType, input.texCoord3) * weight2;
	color += shaderTexture.Sample(SampleType, input.texCoord4) * weight1;
	color += shaderTexture.Sample(SampleType, input.texCoord5) * weight0;
	color += shaderTexture.Sample(SampleType, input.texCoord6) * weight1;
	color += shaderTexture.Sample(SampleType, input.texCoord7) * weight2;
	color += shaderTexture.Sample(SampleType, input.texCoord8) * weight3;
	color += shaderTexture.Sample(SampleType, input.texCoord9) * weight4;
	color.a = 1.0f;

	return color;
}