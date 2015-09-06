Texture2D shaderTexture;
SamplerState SampleType;

cbuffer MaterialBuffer{
	float4 materialAmbient;
	float4 materialDiffuse;
	float3 materialSpecular;
	float3 materialEmissive;
	float materialPower;
}

cbuffer LightBuffer{
	float3 LightPosition;
	float3 LightDiffuseColor;
	float3 LightSpecularColor;
	float LightDistanceSquared;
};

cbuffer CameraBuffer
{
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
	float3 lightDir = normalize(input.worldPos - LightPosition);
	float diffuseLighting = saturate(dot(input.normal, -lightDir));

	diffuseLighting *= (LightDistanceSquared / dot(LightPosition - input.worldPos, LightPosition - input.worldPos));

	float3 h = normalize(normalize(cameraPosition - input.worldPos) - lightDir); //half vector
	float specLighting = pow(saturate(dot(h, input.normal)), materialPower);
	float4 texel = shaderTexture.Sample(SampleType, input.tex);

	return float4(saturate(materialAmbient + //ambient
		(texel.xyz * materialDiffuse * LightDiffuseColor * diffuseLighting * 0.6) + // lightning diffuse vector as intensity
		(materialSpecular * LightSpecularColor * specLighting * 0.5) //specular vector as intensity
		), texel.w);
}