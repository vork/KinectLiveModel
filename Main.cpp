#include "System.h"
#include <iostream>

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
	System* system;
	bool result;

	std::cout << "Starting" << std::endl;

	// Create the system object.
	system = new System;
	if (!system)
	{
		return 0;
	}

	// Initialize and run the system object.

	std::cout << "about to initialize" << std::endl;
	result = system->Initialize();
	if (result)
	{
		system->Run();
	}

	// Shutdown and release the system object.
	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}