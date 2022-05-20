// dxmedia_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../dxmedia/interface.h"
#include <Windows.h>
#include <iostream>
#include <vector>

int main()
{
	i_dxmedia_factory* pFactory = nullptr;
	const wchar_t* input_media_file
		= L"D:\\editing_test\\8K 奥地利风光 Pictures in Motion UHD 8K_24fps.mp4";
	const wchar_t* output_media_file
		= L"D:\\editing_test\\4K 奥地利风光 Pictures in Motion UHD 4K_24fps.mp4";

	auto dxmedia_dll = LoadLibraryW(L"dxmedia");
	if (dxmedia_dll)
	{
		auto cdf_func_address = GetProcAddress(dxmedia_dll, "create_dxmedia_factory");
		auto cdf_func =
			reinterpret_cast<decltype(&create_dxmedia_factory)>(cdf_func_address);

		cdf_func(&pFactory);
	}

	if (pFactory)
	{
		i_dxmedia_object* pObject = nullptr;
		i_dxmedia_reader* pReader = nullptr;
		i_dxmedia_writer* pWriter = nullptr;

		pFactory->create_dxmedia_object(DXMEDIA_READER, &pObject);
		pReader = static_cast<i_dxmedia_reader*>(pObject);

		pFactory->create_dxmedia_object(DXMEDIA_WRITER, &pObject);
		pWriter = static_cast<i_dxmedia_writer*>(pObject);
		
		int stream_num = 0;
		pReader->open_media(input_media_file, stream_num);

		pWriter->create_media(output_media_file);

		for (int i = 0; i < stream_num; i++)
		{
			dxstream in_stream;
			pReader->get_stream(i, in_stream);

			dxstream out_stream = in_stream;
			out_stream.frame_width = in_stream.frame_width;
			out_stream.frame_height = in_stream.frame_height;
			out_stream.avg_bitrate = in_stream.avg_bitrate;

			int stream_index = 0;
			pWriter->set_stream(out_stream, stream_index);
		}

		delete pWriter;
		delete pReader;
		delete pFactory;
	}
}

//while (true)
//{
//	dxframe frame;
//	unsigned int stream_index = 1;
//	pReader->get_sample(stream_index, -1, frame);
//	if (!frame.data_ptr)
//		break;
//	std::cout
//		<< "read sample(" << frame.frame_time << ") from stream "
//		<< stream_index
//		<< std::endl;
//}