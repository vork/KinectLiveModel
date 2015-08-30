#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Kinect.h"

class Body3DRenderer
{
public:
	Body3DRenderer();
	~Body3DRenderer();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, KinectHelper*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX*, XMFLOAT3*, XMFLOAT3*); //TODO add a simple color shader class

private:
	KinectHelper* m_pKinectHelper;
	shared_ptr<RigJoint> m_BoneHierarchy;

	bool InitializeBuffers();
	void RenderBuffers();
	void ProcessBody(int nBodyCount, IBody** ppBodies, ID3D11DeviceContext*);
	void DrawBone(ID3D11DeviceContext*, XMFLOAT3);

};
