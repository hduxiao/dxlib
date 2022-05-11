// dxmedia_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../dxmedia/interface.h"
#include <Windows.h>

int main()
{
    i_dxmedia_reader* pReader = nullptr;

	auto dxmedia_dll = LoadLibraryW(L"dxmedia");
	if (dxmedia_dll)
	{
		auto startup_func_address = GetProcAddress(dxmedia_dll, "startup_mediafoundation");
		auto startup_func =
			reinterpret_cast<decltype(&startup_mediafoundation)>(startup_func_address);
		if (!startup_func) {
			return 0;
		}
		startup_func();

		auto create_dxmedia_func_address = GetProcAddress(dxmedia_dll, "create_dxmedia_reader");
		auto create_dxmedia_func = 
			reinterpret_cast<decltype(&create_dxmedia_reader)>(create_dxmedia_func_address);
		if (!create_dxmedia_func) {
			return 0;
		}
		create_dxmedia_func(&pReader);
		pReader->open_media(L"D:\\editing_test\\Ultra_Video_Group\\Bosphorus_1920x1080_30fps_420_8bit_AVC_MP4.mp4");

		auto shutdown_func_address = GetProcAddress(dxmedia_dll, "shutdown_mediafoundation");
		auto shutdown_func =
			reinterpret_cast<decltype(&startup_mediafoundation)>(shutdown_func_address);
		if (!shutdown_func) {
			return 0;
		}
		shutdown_func();
	}
}
