#include "Body3DRenderer.h"
#include "System.h"


Body3DRenderer::Body3DRenderer()
{
}


Body3DRenderer::~Body3DRenderer()
{
}

bool Body3DRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, KinectHelper* helper)
{
	// Load mesh vertices. Each vertex has a position and a color.
	static const VertexPositionColor cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 4.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 4.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 4.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 4.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			0, 2, 1, // -x
			1, 2, 3,

			4, 5, 6, // +x
			5, 7, 6,

			0, 1, 5, // -y
			0, 5, 4,

			2, 6, 7, // +y
			2, 7, 3,

			0, 4, 6, // -z
			0, 6, 2,

			1, 3, 7, // +z
			1, 7, 5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
			)
			);
	});
	return true;
}

void Body3DRenderer::Shutdown()
{
	
}

void Body3DRenderer::DrawBone(ID3D11DeviceContext* context, XMFLOAT3 color)
{
	m_constantBufferData.color.x = color.x;
	m_constantBufferData.color.y = color.y;
	m_constantBufferData.color.z = color.z;

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(m_constantBuffer, 0, NULL, &m_constantBufferData, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(0, 1, &m_constantBuffer);

	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
}

void Body3DRenderer::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* lightColor, XMFLOAT3* lightDir)
{
	if (!m_pKinectHelper->m_p_body_frame_reader())
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;

	bool updated = false;
	HRESULT hr = m_pKinectHelper->m_p_body_frame_reader()->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			ProcessBody(BODY_COUNT, ppBodies, deviceContext);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
		updated = true;
	}

	SafeRelease(pBodyFrame);
}

void Body3DRenderer::ProcessBody(int nBodyCount, IBody** ppBodies, ID3D11DeviceContext* context)
{
	HRESULT hr;
	if (m_pKinectHelper->m_p_coordinate_mapper()) //Safety first !
	{
		for (int i = 0; i < nBodyCount; ++i)
		{
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					const float boneLen = 3.5f;
					XMVECTOR transformed = { 0.0f, 0.0f, 0.0f, 0.0f };
					XMVECTOR res = { 0.0f, 0.0f, 0.0f, 0.0f };

					KinectHelper::TraverseBoneHierarchy(m_BoneHierarchy,
						[&pBody, &res, this, &transformed, boneLen, &context](shared_ptr<RigJoint>& t)
					{
						JointOrientation joint_orientation[JointType_Count];
						pBody->GetJointOrientations(JointType_Count, joint_orientation);

						// Check joint orientation for the joint
						for (int i = 0; i < JointType_Count; i++)
						{
							if (joint_orientation[i].JointType == t->JointType())
							{
								t->_orientation = joint_orientation[i];
								break;
							}
						}

						// if orientation is zero use the parent orientation
						JointOrientation orientation = t->_orientation;

						auto v4 = XMFLOAT4(t->_orientation.Orientation.x,
							t->_orientation.Orientation.y,
							t->_orientation.Orientation.z,
							t->_orientation.Orientation.w);

						auto parent = t->Parent();
						if (XMVector4Equal(XMLoadFloat4(&v4), XMVectorZero()) && parent != nullptr)
						{
							orientation = parent->_orientation;
						}

						//Create a rotation Matrix
						//For the root start with a transform (absolute position of the whole body)
						//For the other joints start with the parent transforms
						auto f4 = XMFLOAT4(orientation.Orientation.x, orientation.Orientation.y, orientation.Orientation.z, orientation.Orientation.w);
						auto rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&f4));
						if (parent != nullptr)
						{
							transformed = parent->_transformed;
						}
						else
						{
							Joint joints[JointType_Count];
							pBody->GetJoints(JointType_Count, joints);

							for (int i = 0; i < JointType_Count; i++)
							{
								if (joints[i].JointType == t->JointType())
								{
									auto pos = joints[i].Position;
									auto v3 = XMFLOAT3(MULT * pos.X, MULT * pos.Y, MULT * pos.Z);
									transformed = XMLoadFloat3(&v3);
									break;
								}
							}
						}

						//Convert vector into transform matrix and store in model matrix
						auto translatedOrigin = XMMatrixTranslationFromVector(transformed);
						//XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translatedOrigin)); //TODO store data

						auto translated = XMMatrixTranslation(0.0f, boneLen, 0.0f);
						auto scaleMat = XMMatrixScaling(1.0f, t->BoneLength(), 1.0f);
						auto mat = scaleMat * rotMatrix * translatedOrigin;
						//XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(mat)); //TODO store data

						auto f3 = XMFLOAT3(0.0f, boneLen, 0.0f);
						t->_transformed = XMVector3TransformCoord(XMLoadFloat3(&f3), mat);

						if (parent != nullptr)
						{
							// TODO draw the bone
							//DrawBone(context, t->getColour());
						}
					});
				}
			}
		}
	}
}