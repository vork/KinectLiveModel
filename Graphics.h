#pragma once

#include "D3Device.h"
#include "Camera.h"
#include "Model.h"
#include "TextureShader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

	D3Device* m_D3D;

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

	float rotate;

	Camera* m_Camera;
	Model* m_Model;
	//ColorShaderClass* m_ColorShader;
	TextureShader* m_TextureShader;
};
