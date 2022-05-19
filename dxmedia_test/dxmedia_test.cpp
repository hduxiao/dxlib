// dxmedia_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../dxmedia/interface.h"
#include <Windows.h>
#include <iostream>

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
		const wchar_t* media_file
			= L"D:\\editing_test\\8K 奥地利风光 Pictures in Motion UHD 8K_24fps.mp4";
		pReader->open_media(media_file, streamNum);
		dxstream streamInfo;
		pReader->get_stream_info(0, streamInfo);

		while (true)
		{
			dxframe frame;
			unsigned int stream_index = 1;
			pReader->get_sample(stream_index, -1, frame);
			if (!frame.data_ptr)
				break;
			std::cout
				<< "read sample(" << frame.frame_time << ") from stream "
				<< stream_index
				<< std::endl;
		}

		delete pFactory;
	}
}
