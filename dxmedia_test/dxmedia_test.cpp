// dxmedia_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../dxmedia/interface.h"
#include <Windows.h>

int main()
{
    i_dxmedia_factory* pFactory = nullptr;
    i_dxmedia_reader* pReader = nullptr;

	auto dxmedia_dll = LoadLibraryW(L"dxmedia");
	if (dxmedia_dll)
	{
		auto cdf_func_address = GetProcAddress(dxmedia_dll, "create_dxmedia_factory");
		auto cdf_func =
			reinterpret_cast<decltype(&create_dxmedia_factory)>(cdf_func_address);
		if (!cdf_func) {
			return 0;
		}

		cdf_func(&pFactory);
	}

	if (pFactory)
	{
		void* pObject = nullptr;
		pFactory->create_dxmedia_object(DXMEDIA_READER, &pObject);
		pReader = reinterpret_cast<i_dxmedia_reader*>(pObject);
		UINT32 streamNum = 0;
		pReader->open_media(L"D:\\editing_test\\Ultra_Video_Group\\Bosphorus_1920x1080_30fps_420_8bit_AVC_MP4.mp4", streamNum);
		dxstream streamInfo;
		pReader->get_stream_info(0, streamInfo);
		dxframe frame;
		pReader->get_sample(0, -1, frame);
		delete pFactory;
	}
}
