#pragma once
#include "interface.h"
class dxmedia_writer :
	public i_dxmedia_writer
{
public:

	void create_media(const wchar_t* media) override;
	void add_stream(const dxstream& out_stream, int& stream_index) override;
	void write_sample(int stream_index, dxframe& frame) override;
	void begin_writing() override;
	void finalize() override;

private:

	dxmedia m_media;
	std::vector<dxstream> m_stream;
	CComPtr<IMFSinkWriter> m_pWriter;
};
