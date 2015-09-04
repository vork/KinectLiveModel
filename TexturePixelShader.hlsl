Texture2D shaderTexture;
SamplerState SampleType;

cbuffer MaterialBuffer{
	float4 materialAmbient;
	float4 materialDiffuse;
	float3 materialSpecular;
	float materialPower;
	float3 dirLightDir;
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 view : TEXCOORD1;
};

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float3 reflection;
	float4 specular;

	textureColor = shaderTexture.Sample(SampleType, input.tex);

	lightDir = normalize(-dirLightDir);
	float3 view = normalize(input.view);
	float3 normal = normalize(input.normal);

	color = materialAmbient;
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	lightIntensity = saturate(dot(normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (materialDiffuse * lightIntensity);

		color = saturate(color);
		reflection = normalize(2 * lightIntensity * normal - lightDir);
		specular = pow(saturate(dot(reflection, view)), materialPower);
	}

	color = color *textureColor;

	color = saturate(color + specular);

	return color;
}