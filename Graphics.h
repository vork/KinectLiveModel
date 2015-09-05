#pragma once

#include "D3Device.h"
#include "Camera.h"
#include "Model.h"
#include "TextureShader.h"
#include "Body3DRenderer.h"
#include "Kinect.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics(const KinectHelper* kinectHelper);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	void setPosition(float, float, float);
	void setRotation(float, float, float);

private:
	bool Render(float);

	D3Device* m_device;

	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}
		XMFLOAT3 dir;
		XMFLOAT3 pos;
		XMFLOAT3 color;
	};

	Light light;

	Camera* m_Camera;
	Model* m_Model;
	Body3DRenderer* m_BodyRenderer;
	const KinectHelper* m_KinectHelper;
	TextureShader* m_TextureShader;
};
