#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "Camera.h"
#include "ShaderStructures.h"

using namespace DirectX;
using namespace std;

class TextureShader
{
public:
	TextureShader(Camera*);
	~TextureShader();

	bool Initialize(ID3D11Device*, HWND);
	void Release();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, ShaderStructures::MaterialType*, ShaderStructures::CameraType*, ShaderStructures::LightType*);

private:
	Camera* m_pCameraClass;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;

	ID3D11SamplerState* m_sampleState;

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, ShaderStructures::MaterialType*, ShaderStructures::CameraType*, ShaderStructures::LightType*);
	void RenderShader(ID3D11DeviceContext*, int);
};

