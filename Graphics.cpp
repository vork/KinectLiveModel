#include "Graphics.h"


Graphics::Graphics(const KinectHelper* kinect)
{
	m_device = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_KinectHelper = kinect;

	m_HorizontalBlurShader = 0;
	m_VerticalBlurShader = 0;
	m_RenderTexture = 0;
	m_DownSampleTexure = 0;
	m_HorizontalBlurTexture = 0;
	m_VerticalBlurTexture = 0;
	m_UpSampleTexure = 0;
	m_SmallWindow = 0;
	m_FullScreenWindow = 0;
}




Graphics::~Graphics()
{
}

void Graphics::setPosition(float x, float y, float z)
{
	m_Camera->SetPosition(x, y, z);
}

void Graphics::setRotation(float x, float y, float z)
{
	m_Camera->SetRotation(x, y, z);
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	int downSampleWidth, downSampleHeight;

	downSampleWidth = screenWidth / 2;
	downSampleHeight = screenHeight / 2;

	// Create the Direct3D object.
	m_device = new D3Device;
	if (!m_device)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_device->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, 0.f);
	m_Camera->SetRotation(0.f, 0.f, 0.f);

	// Create the model object.
	m_Model = new Landscape3DRenderer;
	if (!m_Model)
	{
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShader(m_Camera);
	if (!m_TextureShader)
	{
		return false;
	}

	m_BodyRenderer = new Body3DRenderer();
	if (!m_BodyRenderer)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_device->GetDevice(), m_device->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the color shader object.
	result = m_TextureShader->Initialize(m_device->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the body renderer object.
	result = m_BodyRenderer->Initialize(m_device->GetDevice(), m_device->GetDeviceContext(), m_KinectHelper);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the body renderer object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur shader object.
	m_HorizontalBlurShader = new HorizontalBlurShader;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// Initialize the horizontal blur shader object.
	result = m_HorizontalBlurShader->Initialize(m_device->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur shader object.
	m_VerticalBlurShader = new VerticalBlurShader;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// Initialize the vertical blur shader object.
	result = m_VerticalBlurShader->Initialize(m_device->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTexture;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_device->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the down sample render to texture object.
	m_DownSampleTexure = new RenderTexture;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// Initialize the down sample render to texture object.
	result = m_DownSampleTexure->Initialize(m_device->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTexture;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(m_device->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTexture;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(m_device->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTexture;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(m_device->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the small ortho window object.
	m_SmallWindow = new OrthoWindow;
	if (!m_SmallWindow)
	{
		return false;
	}

	// Initialize the small ortho window object.
	result = m_SmallWindow->Initialize(m_device->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	m_FullScreenWindow = new OrthoWindow;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(m_device->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	light.dir = XMFLOAT3(2.5f, 0.0f, 2.5f);
	light.pos = XMFLOAT3(2.5f, 5.0f, 2.5f);
	light.diffcolor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	light.speccolor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	light.lightDistanceSquared = 4;

	return true;
}


void Graphics::Release()
{
	SafeRelease(m_FullScreenWindow);
	SafeRelease(m_SmallWindow);
	SafeRelease(m_UpSampleTexure);
	SafeRelease(m_VerticalBlurTexture);
	SafeRelease(m_HorizontalBlurTexture);
	SafeRelease(m_DownSampleTexure);
	SafeRelease(m_RenderTexture);
	SafeRelease(m_VerticalBlurShader);
	SafeRelease(m_HorizontalBlurShader);
	SafeRelease(m_TextureShader);
	SafeRelease(m_Model);
	SafeRelease(m_BodyRenderer);
	SafeRelease(m_Camera);
	SafeRelease(m_device);

	return;
}


bool Graphics::Frame()
{
	bool result;

	static float rotation = 0.0f;

	/*rotation += (float)XM_PI * 0.001f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}*/

	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool Graphics::Render(float rotation)
{
	bool result;


	// Render scene to a texture
	result = RenderSceneToTexture(rotation);
	if (!result)
	{
		return false;
	}

	// down sample the texture
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// run the horizontal blur
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// run the vertical blur
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// up samle the texture
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	// render the blurred shader to the screen
	result = Render2DTextureScene();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::RenderSceneToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//init the render texture
	m_RenderTexture->SetRenderTarget(m_device->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(m_device->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Create the view, world, projection matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_device->GetProjectionMatrix(projectionMatrix);

	cameraType.cameraPosition = m_Camera->GetPosition();

	lightType.LightSpecularColor = light.speccolor;
	lightType.LightDiffuseColor = light.diffcolor;
	lightType.LightDistanceSquared = light.lightDistanceSquared;
	lightType.LightPosition = light.pos;

	// render the mode
	m_Model->Render(m_device->GetDeviceContext(), m_TextureShader, &worldMatrix, &viewMatrix, &projectionMatrix, &lightType, &cameraType);

	// render the body
	m_BodyRenderer->Render(m_device->GetDeviceContext(), m_TextureShader, &worldMatrix, &viewMatrix, &projectionMatrix, &cameraType, &lightType);

	//TODO no separate renders

	// reset render targert to back buffer
	m_device->SetBackBufferRenderTarget();

	m_device->ResetViewport();

	return true;
}


bool Graphics::DownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	//init the render texture
	m_DownSampleTexure->SetRenderTarget(m_device->GetDeviceContext());
	m_DownSampleTexure->ClearRenderTarget(m_device->GetDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	//Create the view, world, ortho matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 2d rendering -> turn off z buffer
	m_device->TurnZBufferOff();

	//Render the texture
	m_SmallWindow->Render(m_device->GetDeviceContext());
	result = m_TextureShader->Render(m_device->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(), NULL, NULL, NULL);
	if (!result)
	{
		return false;
	}

	//Reset everything to defaults
	m_device->TurnZBufferOn();
	m_device->SetBackBufferRenderTarget();
	m_device->ResetViewport();

	return true;
}


bool Graphics::RenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;

	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	//init the render texture
	m_HorizontalBlurTexture->SetRenderTarget(m_device->GetDeviceContext());
	m_HorizontalBlurTexture->ClearRenderTarget(m_device->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Create the view, world, ortho matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 2d rendering -> turn off z buffer
	m_device->TurnZBufferOff();

	//Render the texture
	m_SmallWindow->Render(m_device->GetDeviceContext());
	result = m_HorizontalBlurShader->Render(m_device->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), screenSizeX);
	if (!result)
	{
		return false;
	}

	//Reset everything to defaults
	m_device->TurnZBufferOn();
	m_device->SetBackBufferRenderTarget();
	m_device->ResetViewport();

	return true;
}


bool Graphics::RenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	//init the render texture
	m_VerticalBlurTexture->SetRenderTarget(m_device->GetDeviceContext());
	m_VerticalBlurTexture->ClearRenderTarget(m_device->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Create the view, world, ortho matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 2d rendering -> turn off z buffer
	m_device->TurnZBufferOff();

	//Render the texture
	m_SmallWindow->Render(m_device->GetDeviceContext());
	result = m_VerticalBlurShader->Render(m_device->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result)
	{
		return false;
	}

	//Reset everything to defaults
	m_device->TurnZBufferOn();
	m_device->SetBackBufferRenderTarget();
	m_device->ResetViewport();

	return true;
}


bool Graphics::UpSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	//init the render texture
	m_UpSampleTexure->SetRenderTarget(m_device->GetDeviceContext());
	m_UpSampleTexure->ClearRenderTarget(m_device->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//Create the view, world, ortho matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 2d rendering -> turn off z buffer
	m_device->TurnZBufferOff();

	//Render the texture
	m_FullScreenWindow->Render(m_device->GetDeviceContext());
	result = m_TextureShader->Render(m_device->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView(), NULL, NULL, NULL);
	if (!result)
	{
		return false;
	}

	//Reset everything to defaults
	m_device->TurnZBufferOn();
	m_device->SetBackBufferRenderTarget();
	m_device->ResetViewport();

	return true;
}


bool Graphics::Render2DTextureScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	//init the scene (background color blue)
	m_device->BeginScene(0.13f, 0.59f, 0.95f, 1.0f);

	//Create the view, world, projection matrix
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetWorldMatrix(worldMatrix);
	m_device->GetOrthoMatrix(orthoMatrix);

	// 2d rendering -> turn off z buffer
	m_device->TurnZBufferOff();

	//Render the texture
	m_FullScreenWindow->Render(m_device->GetDeviceContext());
	result = m_TextureShader->Render(m_device->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_UpSampleTexure->GetShaderResourceView(), NULL, NULL, NULL);
	if (!result)
	{
		return false;
	}

	//Reset everything to defaults and end the scene
	m_device->TurnZBufferOn();
	m_device->EndScene();

	return true;
}
