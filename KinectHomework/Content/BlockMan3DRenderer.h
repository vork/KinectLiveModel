#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <DirectXMath.h>
#include "BoneJoint.h"
#include "Grid.h"
#include "Axis.h"

using namespace WindowsPreview::Kinect;
using namespace Platform::Collections;
using namespace DirectX;
using namespace std;
using namespace Windows::System;

namespace KinectHomework
{
	// This sample renderer instantiates a basic rendering pipeline.
	class BlockMan3DRenderer
	{
	public:
		BlockMan3DRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking(float positionX, float positionY, VirtualKeyModifiers mod);
		void TrackingUpdate(float positionX, float positionY, VirtualKeyModifiers mod);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);
		void DrawBone(ID3D11DeviceContext2 *context, XMFLOAT3 color);
		void DrawGrid(ID3D11DeviceContext2 *context);

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		MatrixBuffer m_constantBufferData;
		uint32	m_indexCount;

		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		float m_yaw;
		float m_pitch;
		float m_roll;
		float m_zoom;

		KinectSensor^ m_sensor;
		BodyFrameReader^ m_reader;
		Vector<Body^>^ m_bodies;

		ID3D11RasterizerState * m_rasterState;

		XMFLOAT4 m_eye;
		shared_ptr<BoneJoint> m_boneHierarchy;

		unique_ptr<Grid> m_grid;
	};
}

