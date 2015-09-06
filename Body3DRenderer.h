#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Kinect.h"
#include "ShaderStructures.h"

class TextureShader;
using namespace DirectX;
using namespace ShaderStructures;

class Body3DRenderer
{
public:
	Body3DRenderer();
	~Body3DRenderer();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const KinectHelper*);
	void Release();
	void Render(ID3D11DeviceContext*, TextureShader*, XMMATRIX*, XMMATRIX*, XMMATRIX*, XMFLOAT3*, XMFLOAT3*); //TODO add a simple color shader class

private:
	const KinectHelper* m_pKinectHelper;
	shared_ptr<RigJoint> m_BoneHierarchy;

	bool InitializeBuffers();
	void RenderBuffers();
	void ProcessBody(int nBodyCount, IBody** ppBodies, ID3D11DeviceContext* context, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* lightColor, XMFLOAT3* lightDir);
	void DrawBone(ID3D11DeviceContext* context, TextureShader* texShader, XMFLOAT3 color, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* lightColor, XMFLOAT3* lightDir);

	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_constantBuffer;

	UINT32 m_indexCount;

};
