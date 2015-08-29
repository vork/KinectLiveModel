cbuffer MatrixBuffer
{
	float4x4 matWorldViewProj : WORLDVIEWPROJECTION;
	float4x3 matWorldIT : WORLDINVERSETRANSPOSE;
	float4x4 matWorld : WORLD;
	float3 viewPosition : VIEWPOSITION;
}

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 view : TEXCOORD2;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Calculate the position of the vertex against the worldviewprojection matrix
	output.position = mul(input.position, matWorldViewProj);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	output.normal = mul(matWorldIT, input.normal);

	float3 worldPos = mul(input.position, matWorld).xyz;
	output.view = viewPosition - worldPos;

	return output;
}