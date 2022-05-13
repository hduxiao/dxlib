#pragma once
class i_dxmedia_reader
{
public:
	virtual ~i_dxmedia_reader() {}
	virtual void open_media(const wchar_t* media, unsigned int& stream_num) = 0;
	virtual void get_stream_info(unsigned int stream_index, dxstream& stream_info) = 0;
	virtual void get_sample(unsigned int stream_index, long long frame_time, dxframe& frame) = 0;
};
