#include "CommonHeader.h"
#include <windows.h>
#include "App.h"
#include "ExceptionHandling.h"
#include <thread>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int returnCode = 0;
	try
	{
		gfx::App app(gfx::ScreenWidth, gfx::ScreenHeight, "DX11Raytracer", std::thread::hardware_concurrency() - 1u);
		return app.Run();
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		returnCode = -1;
	}
	catch (...)
	{
		MessageBox(nullptr, "Unknown exception occurred", "Unknown Exception", MB_OK | MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		returnCode = -1;
	}

	return returnCode;
}