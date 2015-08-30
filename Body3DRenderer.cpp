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
	return true;
}

void Body3DRenderer::Shutdown()
{
	
}

void Body3DRenderer::DrawBone(ID3D11DeviceContext*, XMFLOAT3)
{
	
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