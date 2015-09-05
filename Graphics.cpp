#include "Graphics.h"


Graphics::Graphics()
{
	m_device = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}


Graphics::Graphics(const Graphics& other)
{
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
	m_Model = new Model;
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

	light.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	light.pos = XMFLOAT3(0.f, 0.f, 0.f);
	light.color = XMFLOAT3(1.0f, 1.0f, 1.0f);

	return true;
}


void Graphics::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_device)
	{
		m_device->Shutdown();
		delete m_device;
		m_device = 0;
	}

	return;
}


bool Graphics::Frame()
{
	bool result;

	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	/*rotation += (float)XM_PI * 0.001f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}*/

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool Graphics::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_device->BeginScene(0.13f, 0.59f, 0.95f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_device->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_device->GetProjectionMatrix(projectionMatrix);

	XMMATRIX updatedWorld = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(rotation));

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_device->GetDeviceContext(), m_TextureShader, &updatedWorld, &viewMatrix, &projectionMatrix, &light.color, &light.dir);

	// Present the rendered scene to the screen.
	m_device->EndScene();

	return true;
}
