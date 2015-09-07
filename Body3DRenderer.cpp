#include "Body3DRenderer.h"
#include "System.h"



Body3DRenderer::Body3DRenderer()
{
}


Body3DRenderer::~Body3DRenderer()
{
}

bool Body3DRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const KinectHelper* helper)
{
	m_pKinectHelper = helper;

	m_BoneHierarchy = helper->CreateHierarchy();

	Assimp::Importer Importer;
	const aiScene* pScene = NULL;
	const aiMesh* pMesh = NULL;
	const aiMaterial* pMat = NULL;

	std::string filename = "Cube.dae"; //"World.dae";

	//Possible flags:  aiProcess_TransformUVCoords | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph 
	pScene = Importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_SplitLargeMeshes |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType |
		aiProcess_PreTransformVertices
		);

	if (!pScene)
	{
		printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
		return false;
	}

	pMesh = pScene->mMeshes[0]; //There is only 1 mesh

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_indexCount = 0;

	// Set the number of indices in the index array.
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		if (pMesh->mFaces[i].mNumIndices == 3) //only allow triangles
		{
			m_indexCount = m_indexCount + 3;
		}
		else
		{
			printf("Error parsing Faces. Try to Re-Export model from 3d package!");
			return false;
		}
	}

	int vertexCount = pMesh->mNumVertices;

	VertexType* Vertices;
	int* Indices;

	Vertices = new ShaderStructures::VertexType[vertexCount];
	if (!Vertices)
	{
		return false;
	}


	// Create the index array.
	Indices = new int[m_indexCount];
	if (!Indices)
	{
		return false;
	}

	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		XMFLOAT3 pos(pMesh->mVertices[i].x , pMesh->mVertices[i].y, pMesh->mVertices[i].z);
		Vertices[i].position = pos;
		if (pMesh->GetNumUVChannels() > 0) {
			XMFLOAT2 texcoord(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
			Vertices[i].texture = texcoord;
		}
		XMFLOAT3 normals(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
		Vertices[i].normal = normals;
	}

	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		Indices[i * 3 + 0] = pMesh->mFaces[i].mIndices[0];
		Indices[i * 3 + 1] = pMesh->mFaces[i].mIndices[1];
		Indices[i * 3 + 2] = pMesh->mFaces[i].mIndices[2];
	}
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = Indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Body3DRenderer::Release()
{
	
}

void Body3DRenderer::DrawBone(ID3D11DeviceContext* context, TextureShader* texShader, XMFLOAT3 color, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, CameraType* camera, LightType* light)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);

	offset = 0;

	// Activate the vertex buffer
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Activate the index buffer
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Topology are triangles
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Fill in the data for the constant buffer in the shader
	MaterialType materialType;
	materialType.materialAmbient = XMFLOAT4(color.x, color.y, color.z, 1.f);
	materialType.materialDiffuse = XMFLOAT4(color.x, color.y, color.z, 1.f);
	materialType.materialPower = 0.f;
	materialType.materialSpecular = XMFLOAT3(color.x, color.y, color.z);
	materialType.materialEmissive = XMFLOAT3(0.f, 0.f, 0.f);

	bool result = texShader->Render(context, m_indexCount, *world, *view, *projection, NULL, &materialType, camera, light);
	if (!result)
	{
	}
}

void Body3DRenderer::Render(ID3D11DeviceContext* deviceContext, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, CameraType* camera, LightType* light)
{
	//Safety first. Is everything loaded correctly?
	if (!m_pKinectHelper->m_p_body_frame_reader())
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;

	//Get the last frame from the kinect
	HRESULT hr = m_pKinectHelper->m_p_body_frame_reader()->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			//Refresh the bodies array with the bodies detected in the frame
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			//Process the body
			ProcessBody(BODY_COUNT, ppBodies, deviceContext, texShader, world, view, projection, camera, light);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}

	SafeRelease(pBodyFrame);
}

void Body3DRenderer::ProcessBody(int nBodyCount, IBody** ppBodies, ID3D11DeviceContext* context, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, CameraType* camera, LightType* light)
{
	HRESULT hr;
	if (m_pKinectHelper->m_p_coordinate_mapper()) //Safety first !
	{
		for (int i = 0; i < nBodyCount; ++i) //Go through all the bodies
		{
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked) //Check if the body is tracked
				{
					// Setup the length of a bone, and init the transformation matrices
					const float boneLen = 4.0f;
					XMVECTOR transformed = { 0.0f, 0.0f, 0.0f, 0.0f };
					XMVECTOR res = { 0.0f, 0.0f, 0.0f, 0.0f };

					//Call the traverse function and pass in a function
					KinectHelper::TraverseHierarchy(m_BoneHierarchy,
						[&pBody, &res, this, &transformed, boneLen, &context, &texShader,
						&world, &view, &projection, &camera, &light](shared_ptr<BoneJoint>& t)
					{
						// Get the orientation of the joint
						JointOrientation joint_orientation[JointType_Count];
						pBody->GetJointOrientations(JointType_Count, joint_orientation);

						// Check joint orientation for the joint
						for (int i = 0; i < JointType_Count; i++)
						{
							if (joint_orientation[i].JointType == t->JointType())
							{
								t->m_orientation = joint_orientation[i];
								break;
							}
						}

						JointOrientation orientation = t->m_orientation;

						auto vec4 = XMFLOAT4(t->m_orientation.Orientation.x,
							t->m_orientation.Orientation.y,
							t->m_orientation.Orientation.z,
							t->m_orientation.Orientation.w);

						auto parent = t->Parent();
						// Some leaf orientations are zero. In that case we need to switch to the parent orientation
						if (XMVector4Equal(XMLoadFloat4(&vec4), XMVectorZero()) && parent != nullptr)
						{
							orientation = parent->m_orientation;
						}

						// Depending on the location we need to get either ...
						auto orientationVec4 = XMFLOAT4(orientation.Orientation.x, orientation.Orientation.y, orientation.Orientation.z, orientation.Orientation.w);
						auto rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&orientationVec4));
						// ...  the parent transformation (non root node) (affine transformation) ...
						if (parent != nullptr)
						{
							transformed = parent->m_transformed;
						}
						// ... or the absolute position for the rotation matrix (Root node) 
						else
						{
							Joint joints[JointType_Count];
							pBody->GetJoints(JointType_Count, joints);

							for (int i = 0; i < JointType_Count; i++)
							{
								if (joints[i].JointType == t->JointType())
								{
									// So transform to the absolute position
									auto pos = joints[i].Position;
									auto v3 = XMFLOAT3(MULT * pos.X, MULT * pos.Y, MULT * pos.Z);
									transformed = XMLoadFloat3(&v3);
									break;
									// This will transform all bones in the hierachy
								}
							}
						}

						// Create the transforma matrix and then store it in the world matrix //TODO is this correct
						auto translatedOrigin = XMMatrixTranslationFromVector(transformed);
						m_matrixBuffer.world = XMMatrixTranspose(translatedOrigin);

						auto scaleMat = XMMatrixScaling(1.0f, t->BoneLength(), 1.0f);
						auto mat = scaleMat * rotMatrix * translatedOrigin;
						m_matrixBuffer.world = XMMatrixTranspose(mat);

						auto boneVec3 = XMFLOAT3(0.0f, boneLen, 0.0f);
						t->m_transformed = XMVector3TransformCoord(XMLoadFloat3(&boneVec3), mat);

						if (parent != nullptr)
						{
							DrawBone(context, texShader, t->getColor(), world, view, projection, camera, light);
						}
					});
				}
			}
		}
	}
}