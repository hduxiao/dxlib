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
		pReader->open_media(L"D:\\editing_test\\american.assassin.2017.720p.bluray.x264-geckos.mkv", streamNum);
		dxstream streamInfo;
		pReader->get_stream_info(0, streamInfo);

		while (true)
		{
			dxframe frame;
			pReader->get_sample(1, -1, frame);
			if (!frame.data_ptr)
				break;
		}
		
		delete pFactory;
	}
}
