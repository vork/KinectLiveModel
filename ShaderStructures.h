#include <d3d11.h>
#include <directxmath.h>
#include <string>

using namespace DirectX;

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
	XMFLOAT4 materialSpecular;
	float materialPower;

	XMFLOAT3 dirLightDir;

	std::string texture;
};