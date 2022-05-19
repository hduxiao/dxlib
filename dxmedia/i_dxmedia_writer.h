#pragma once
class i_dxmedia_writer
{
public:
	virtual ~i_dxmedia_writer() {}
	virtual void create_media(const wchar_t* media) = 0;
	virtual void set_stream(const dxstream& in_stream,
		const dxstream& out_stream, int& stream_index) = 0;
};
