#pragma once
#include "interface.h"
class dxmedia_reader :
	public i_dxmedia_reader
{
public:

	void open_media(const wchar_t* media, dxmedia& media_info) override;
	void get_stream(int stream_index, dxstream& stream_info) override;
	void read_sample(const int stream_index, int& actual_index, dxframe& frame) override;
	void set_position(long long position) override;

private:

	void get_mediainfo();
	void set_mediatype();

	dxmedia m_media;
	std::vector<dxstream> m_stream;
	CComPtr<IMFSourceReader> m_pReader;
};
