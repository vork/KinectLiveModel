Texture2D shaderTexture;
SamplerState SampleType;

cbuffer MaterialBuffer{
	float3 materialEmissive;
	float3 materialAmbient;
	float4 materialDiffuse;
	float3 materialSpecular;
	float materialPower;
	float3 dirLightDir;
	float3 dirLightColor;
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

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	float3 light = normalize(-dirLightDir);
	float3 view = normalize(input.view);
	float3 normal = normalize(input.normal);

	float3 halfway = normalize(light + view);

	// Calculate the emissive lighting
	float3 emissive = materialEmissive;
	// Calculate the ambient reflection
	float3 ambient = materialAmbient;
	// Calculate the diffuse reflection
	float3 diffuse = saturate(dot(normal, light)) * materialDiffuse.rgb;
	// Calculate the specular reflection
	float3 specular = pow(saturate(dot(normal, halfway)), materialPower) * materialSpecular;

	float3 color = (saturate(ambient + diffuse) + specular) * dirLightColor + emissive; // * textureColor + specular) * dirLightColor + emissive; //
	// Calculate the transparency
	float alpha = materialDiffuse.a;// *textureColor.a;
	// Return the pixel's color
	return float4(color, alpha);
}