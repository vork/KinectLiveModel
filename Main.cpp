#include "Main.h"

#include <strsafe.h>

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Main application;
	application.Run(hInstance, nShowCmd);
}

Main::Main() :
m_hWnd(NULL),
m_nStartTime(0),
m_nLastCounter(0),
m_nFramesSinceUpdate(0),
m_fFreq(0),
m_nNextStatusTime(0LL),
m_pD3Device(NULL),
m_pD3Context(NULL)
{

}


Main::~Main()
{
}

LRESULT CALLBACK Main::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

}
LRESULT CALLBACK Main::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

}

int Main::Run(HINSTANCE hInstance, int nCmdShow) {

}

HRESULT Main::EnsureDirect3DResources() {

}

void Main::DiscardDirect3DResources() {

}

void Main::Update() {

}

bool Main::Render() {

}