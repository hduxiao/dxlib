#pragma once
#include "interface.h"
class dxmedia_writer :
	public i_dxmedia_writer
{
public:

	void create_media(const wchar_t* media) override;
	void set_stream(const dxstream& out_stream, int& stream_index) override;

private:

	dxmedia m_media;
	std::vector<dxstream> m_stream;
	CComPtr<IMFSinkWriter> m_pWriter;
};
