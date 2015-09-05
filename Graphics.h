#pragma once

#include "D3Device.h"
#include "Camera.h"
#include "Model.h"
#include "TextureShader.h"
#include "Body3DRenderer.h"
#include "Kinect.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "RenderTexture.h"
#include "OrthoWindow.h"

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

	bool RenderSceneToTexture(float);
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();

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

	HorizontalBlurShader* m_HorizontalBlurShader;
	VerticalBlurShader* m_VerticalBlurShader;

	RenderTexture *m_RenderTexture, *m_DownSampleTexure, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpSampleTexure;

	OrthoWindow *m_SmallWindow, *m_FullScreenWindow;
};
