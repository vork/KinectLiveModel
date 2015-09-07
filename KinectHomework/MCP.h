#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\BlockMan3DRenderer.h"
#include "Content\FpsTextRenderer.h"

namespace KinectHomework
{
	// Master Control Programm. Maybe less evil.
	class MCP : public DX::IDeviceNotify
	{
	public:
		MCP(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~MCP();
		void CreateWindowSizeDependentResources();
		void StartTracking(float positionX, float positionY, Windows::System::VirtualKeyModifiers mod) 
		{ 
			m_blockManRenderer->StartTracking(positionX, positionY, mod);
		}
		void TrackingUpdate(float positionX, float positionY, Windows::System::VirtualKeyModifiers mod) 
		{ 
			m_pointerLocationX = positionX; m_pointerLocationY = positionY; 
			_keyModifiers = mod;
		}
		void StopTracking() { m_blockManRenderer->StopTracking(); }
		bool IsTracking() { return m_blockManRenderer->IsTracking(); }
		void StartRenderLoop();
		void StopRenderLoop();
		Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		void ProcessInput();
		void Update();
		bool Render();

		// Cached pointer for the device resource
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		//Content Renderer
		std::unique_ptr<BlockMan3DRenderer> m_blockManRenderer;
		std::unique_ptr<FpsTextRenderer> m_fpsTextRenderer;

		Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
		Concurrency::critical_section m_criticalSection;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		// Track current input pointer position.
		float m_pointerLocationX;
		float m_pointerLocationY;
		Windows::System::VirtualKeyModifiers _keyModifiers;
	};
}