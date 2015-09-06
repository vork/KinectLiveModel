#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <string>

using namespace DirectX;

namespace ShaderStructures {
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct MaterialType
	{
		XMFLOAT4 materialAmbient;
		XMFLOAT4 materialDiffuse;
		XMFLOAT3 materialSpecular;
		XMFLOAT3 materialEmissive;
		float materialPower;
	};

	struct MatrixType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraType
	{
		XMFLOAT3 cameraPosition;
	};

	struct LightType
	{
		XMFLOAT3 LightPosition;
		XMFLOAT3 LightDiffuseColor;
		XMFLOAT3 LightSpecularColor;
		float LightDistanceSquared;
	};
}