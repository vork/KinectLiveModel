#include "pch.h"
#include "BlockMan3DRenderer.h"
#include "Common\DirectXHelper.h"
#include "BoneHierachy.h"

using namespace KinectHomework;
using namespace Windows::Foundation;

static float lastPosX;
static float lastPosY;
static float lastY;

BlockMan3DRenderer::BlockMan3DRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources),
	m_zoom(30.0f) //Init variables
{
	m_grid = make_unique<Grid>();

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	//Setup the kinect
	m_sensor = KinectSensor::GetDefault();
	m_reader = m_sensor->BodyFrameSource->OpenReader();
	m_bodies = ref new Vector<Body^>(m_sensor->BodyFrameSource->BodyCount);
	m_sensor->Open();

	//Create the Hierachy
	m_boneHierarchy = BoneHierachy::CreateHierarchy();
}

// Initializes view parameters when the window size changes.
void BlockMan3DRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	auto camMatrix = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
	
	XMVECTORF32 alongZ = { 0.0f, 0.0f, m_zoom };

	auto eye = XMVector3TransformCoord(alongZ, camMatrix);
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame
// Unused in this example
void BlockMan3DRenderer::Update(DX::StepTimer const& timer)
{
}

// Rotates the block man
void BlockMan3DRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

// Start the mouse pointer tracking
void BlockMan3DRenderer::StartTracking(float positionX, float positionY, VirtualKeyModifiers mod)
{
	m_tracking = true;
	lastPosY = positionY;
	lastPosX = positionX;
}

// When tracking the block man can be rotated with the mouse pointer by dragging
void BlockMan3DRenderer::TrackingUpdate(float positionX, float positionY, VirtualKeyModifiers mod)
{
	if (m_tracking)
	{
		if ((int)(mod & VirtualKeyModifiers::Control) != 0)
		{
			m_zoom += (positionY - lastPosY) / 10;
		}
		else
		{
			m_pitch += (positionY - lastPosY) / 100;
			m_yaw += (positionX - lastPosX) / 100;
		}

		lastPosY = positionY;
		lastPosX = positionX;

		CreateWindowSizeDependentResources();
	}
}

// Stop the mouse pointer tracking
void BlockMan3DRenderer::StopTracking()
{
	m_tracking = false;
}

// Render a frame
void BlockMan3DRenderer::Render()
{
	// Asynchronous loading. Only draw if loading is completed.
	if (!m_loadingComplete)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	//Draws the Grid
	DrawGrid(context);

	//Get the last frame from the kinect
	auto bodyFrame = m_reader->AcquireLatestFrame();
	if (bodyFrame != nullptr)
	{
		bodyFrame->GetAndRefreshBodyData(m_bodies); //Refresh the bodies array with the bodies detected in the frame
	}

	context->RSSetState(m_rasterState);

	// Loop through the body array
	for (auto body : m_bodies)
	{
		if ((Body^)body == nullptr || !body->IsTracked) //Check if the body is legit and tracked
			continue;

		// Setup the length of a bone, and init the transformation matrices
		const float boneLength = 4.0f;
		XMVECTOR transformed = { 0.0f, 0.0f, 0.0f, 0.0f };
		XMVECTOR res = { 0.0f, 0.0f, 0.0f, 0.0f };

		//Call the traverse function and pass in a function
		BoneHierachy::TraverseHierarchy(m_boneHierarchy,
			[&body, &res, this, &transformed, boneLength, &context](shared_ptr<BoneJoint>& t)
			{
                // Get the orientation of the joint
				t->m_orientation = body->JointOrientations->Lookup(t->JointType());

				JointOrientation orientation = t->m_orientation;

				auto vec4 = XMFLOAT4(t->m_orientation.Orientation.X, 
					t->m_orientation.Orientation.Y, 
					t->m_orientation.Orientation.Z, 
					t->m_orientation.Orientation.W);

                auto parent = t->Parent();
				// Some leaf orientations are zero. In that case we need to switch to the parent orientation
                if (XMVector4Equal(XMLoadFloat4(&vec4), XMVectorZero()) && parent != nullptr)
				{
                    orientation = parent->m_orientation;
				}
				
				// Depending on the location we need to get either ...
				
				auto orientationVec4 = XMFLOAT4(orientation.Orientation.X, orientation.Orientation.Y, orientation.Orientation.Z, orientation.Orientation.W);
				auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&orientationVec4));
				// ...  the parent transformation (non root node) (affine transformation) ...
				if (parent != nullptr)
				{
					transformed = parent->m_transformed;
				}
				// ... or the absolute position for the rotation matrix (Root node) 
				else
				{
					// So transform to the absolute position
					auto pos = body->Joints->Lookup(t->JointType()).Position;
					auto posVec3 = XMFLOAT3(MULT * pos.X, MULT * pos.Y, MULT * pos.Z);
					transformed = XMLoadFloat3(&posVec3);
					// This will transform all bones in the hierachy
				}

				// Create the transforma matrix and then store it in the model matrix
				auto translatedOrigin = XMMatrixTranslationFromVector(transformed);
				XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translatedOrigin));

				auto translated = XMMatrixTranslation(0.0f, boneLength, 0.0f);
				auto scaleMat = XMMatrixScaling(1.0f, t->BoneLength(), 1.0f);
				auto mat = scaleMat * rotationMatrix * translatedOrigin;
				XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(mat));
				
				auto boneVec3 = XMFLOAT3(0.0f, boneLength, 0.0f);
				t->m_transformed = XMVector3TransformCoord(XMLoadFloat3(&boneVec3), mat);

				if (parent != nullptr)
				{
					// draw the bone. Finally.
					DrawBone(context, t->getColor());
				}
			});
	}
}

// Draw the grid
void BlockMan3DRenderer::DrawGrid(ID3D11DeviceContext2 *context)
{
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixIdentity()));

	//Gray
    m_constantBufferData.color.x = 0.3f;
    m_constantBufferData.color.y = 0.3f;
    m_constantBufferData.color.z = 0.3f;

	// Prepare the constant buffer
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

	m_grid->RenderBuffers(context);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach vertex shader..
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// .. send constant shader to the graphic card
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Attach the pixel shader
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// And draw the grid
	context->DrawIndexed(m_grid->IndexCount(), 0, 0);
}

// Draw a single bone
void BlockMan3DRenderer::DrawBone(ID3D11DeviceContext2 *context, XMFLOAT3 color)
{
	//Set the color to the specified one in the bone hierachy
	m_constantBufferData.color.x = color.x;
	m_constantBufferData.color.y = color.y;
	m_constantBufferData.color.z = color.z;

	// Prepare the constant buffer
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

	// A Vertix is a instance of the VertexPositionColor struct. So set the stride and offset accordingly
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	//Init Index Buffer and set the Topology to triangles
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	// Attach vertex shader..
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// .. send constant shader to the graphic card
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Attach pixel shader..
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the bone
	context->DrawIndexed(m_indexCount, 0, 0);
}

void BlockMan3DRenderer::CreateDeviceDependentResources()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK; //TODO is this the correct cull? Cull the weak
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = false;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterDesc, &m_rasterState);

	// Load Shader asynchronously
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// Wait the shader files are loaded. Then create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		//Create the shader or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		// Set the Vertex Description
		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create the input layout or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// Wait till the pixel shader file is loaded and create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		//Create the shader or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		//Set the constant buffer description
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MatrixBuffer) , D3D11_BIND_CONSTANT_BUFFER);

		//Create the buffer or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	//Wait till both shaders are loaded and move on to create a block for the block man
	auto createBlockTask = (createPSTask && createVSTask).then([this] () {

		//Create a block which has 1 unit in width and depth and 4 units height.
		//The block is horizontally centered in the coordinate system but vertically the block starts from 0.
		static const VertexPositionColor blockVertices[] = 
		{
			{XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, 4.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, 4.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(0.5f, 0.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(0.5f, 0.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(0.5f, 4.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.5f, 4.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		};

		//Setup the vertex buffer data
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = blockVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		
		//Setup the vertex buffer description
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(blockVertices), D3D11_BIND_VERTEX_BUFFER);

		//And create the buffer or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		// We always draw triangles. So the indices are grouped in trios.
		static const unsigned short blockIndices [] =
		{
			0,2,1, // -x side
			1,2,3,

			4,5,6, // +x side
			5,7,6,

			0,1,5, // -y side
			0,5,4,

			2,6,7, // +y side
			2,7,3,

			0,4,6, // -z side
			0,6,2,

			1,3,7, // +z side
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(blockIndices);

		//Setup the index buffer data
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = blockIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		//And create the buffer or fail early
		//Setup the index buffer description
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(blockIndices), D3D11_BIND_INDEX_BUFFER);

		//And create the buffer or fail early
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);
	});

	//Wait till the block vertex/index buffer are created and move on to the grid
	auto createGridTask = (createBlockTask).then([this]() {
		m_grid->Initialise(m_deviceResources->GetD3DDevice());
		//Finally everything is loaded up and we can move on to rendering.
		m_loadingComplete = true;
	});
}

void BlockMan3DRenderer::ReleaseDeviceDependentResources()
{
	//Cleanup
	m_loadingComplete = false;
	m_grid->ShutdownBuffers();
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}