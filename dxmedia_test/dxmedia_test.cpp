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
		auto create_dxmedia_func_address = GetProcAddress(dxmedia_dll, "create_dxmedia_reader");
		auto create_dxmedia_func = 
			reinterpret_cast<decltype(&create_dxmedia_reader)>(create_dxmedia_func_address);
		if (!create_dxmedia_func) {
			return 0;
		}
		create_dxmedia_func(&pReader);
	}
}
