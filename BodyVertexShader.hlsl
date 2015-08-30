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
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 view : TEXCOORD1;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}