#include "System.h"

System::System()
{
	m_Input = 0;
	m_Graphics = 0;
}

System::~System()
{
}


bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	camPos = XMFLOAT3(0.f, 0.f, 0.f);
	camRot = XMFLOAT3(0.f, 0.f, 0.f);

	screenWidth = 0;
	screenHeight = 0;

	// Init the windows api
	InitializeWindows(screenWidth, screenHeight);

	//input is used to handle the user input
	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}

	m_Input->Initialize();

	//kinect helper creates and maintains the kinect specific objects
	m_kinectHelper = new KinectHelper;
	if (!m_kinectHelper)
	{
		return false;
	}
	result = m_kinectHelper->Initialize();
	if (!result)
	{
		return false;
	}

	//Create the graphics object. It will handle all graphics related tasks
	m_Graphics = new Graphics(m_kinectHelper);
	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}


	return true;
}


void System::Shutdown()
{
	SafeRelease(m_Graphics);
	SafeRelease(m_Input);
	SafeRelease(m_kinectHelper);

	ShutdownWindows();

	return;
}


void System::Run()
{
	MSG msg;
	bool done, result;


	// Init the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message
	done = false;
	while (!done)
	{
		// handle messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// check if there is a quit message and quit the application
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}


bool System::Frame()
{
	bool result;


	// Check if user wants to exit the application
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	float posSpeed = XM_PI * 0.01f;
	float rotSpeed = XM_PI * 0.1f;
	//Positional
	if (m_Input->IsKeyDown(VK_UP)) //Move forward
	{
		camPos.z += posSpeed;
	}

	if (m_Input->IsKeyDown(VK_DOWN)) //Move backward
	{
		camPos.z -= posSpeed;
	}


	if (m_Input->IsKeyDown(VK_LEFT)) //Move left
	{
		camPos.x -= posSpeed;
	}


	if (m_Input->IsKeyDown(VK_RIGHT)) //Move right
	{
		camPos.x += posSpeed;
	}

	if (m_Input->IsKeyDown(VK_PRIOR)) //Move up
	{
		camPos.y += posSpeed;
	}


	if (m_Input->IsKeyDown(VK_NEXT)) //Move down
	{
		camPos.y -= posSpeed;
	}

	//Rotation
	if (m_Input->IsKeyDown(0x57)) //W Rotate forwards
	{
		camRot.x += rotSpeed;
	}

	if (m_Input->IsKeyDown(0x53)) //S Rotate backwards
	{
		camRot.x -= rotSpeed;
	}

	if (m_Input->IsKeyDown(0x41)) //A Rotate left
	{
		camRot.y -= rotSpeed;
	}

	if (m_Input->IsKeyDown(0x44)) //D Rotate right
	{
		camRot.y += rotSpeed;
	}

	if (m_Input->IsKeyDown(0x51)) //Q Roll left
	{
		camRot.z += rotSpeed;
	}

	if (m_Input->IsKeyDown(0x45)) //E Roll right
	{
		camRot.z -= rotSpeed;
	}

	m_Graphics->setPosition(camPos.x, camPos.y, camPos.z);
	m_Graphics->setRotation(camRot.x, camRot.y, camRot.z);

	// Start rendering the graphics
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key was pressed
	case WM_KEYDOWN:
		{
			// Send key to the input object so it can handle the key press
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key was released
	case WM_KEYUP:
		{
			// Send key to the input object so it can handle the key release
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Send other messages to default handler
	default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	ApplicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"CG_Hausarbeit";

	// Setup window class with defaults
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); //TODO Y U NO WORK?!
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //TODO Y U NO WORK?!
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup dependes on whether the application runs in fullscreen mode or not
	if (FULL_SCREEN)
	{
		//For full screen set the size to the maximum size
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Set the settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Position should be top left corner
		posX = posY = 0;
	}
	else
	{
		// For windowed mode use 800x600
		screenWidth = 800;
		screenHeight = 600;

		// And center the window on the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create window and get the handle
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
	                                       WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	                                       posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Set the focus on the window and bring it to the top
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(true);

	return;
}


void System::ShutdownWindows()
{
	ShowCursor(true);

	// If leaving full screen restore display settings
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Clean up window and application instance
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	//Only keep a lookout for destroy and close messages
	switch (umessage)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
