Texture2D shaderTexture;
SamplerState SampleType;

cbuffer MaterialBuffer {
	float4 materialAmbient;
	float4 materialDiffuse;
	float3 materialSpecular;
	float materialPower;
};

cbuffer LightBuffer {
	float3 LightPosition;
	float3 LightColor;
};

cbuffer CameraBuffer {
	float3 cameraPosition;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : TEXCOORD2;
};

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float3 lightDir = normalize(LightPosition - input.worldPos);
	float distance = length(lightDir);
	lightDir = lightDir / distance;
	float LightDistanceSquared = distance * distance;

	float diffuseLighting = saturate(dot(input.normal, lightDir));

	diffuseLighting *= (LightDistanceSquared / dot(LightPosition - input.worldPos, LightPosition - input.worldPos));

	float3 h = normalize(normalize(cameraPosition - input.worldPos) + lightDir); //half vector
	float specLighting = pow(saturate(dot(h, input.normal)), materialPower);
	float4 texel = shaderTexture.Sample(SampleType, input.tex);

	return float4(saturate(materialAmbient + //ambient
		(texel.xyz * materialDiffuse * LightColor * diffuseLighting * 0.6) +
		(materialSpecular * LightColor * specLighting * 0.5)
		), texel.w);
}