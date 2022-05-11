#pragma once
class i_dxmedia_reader
{
public:
	virtual ~i_dxmedia_reader() {}
	virtual void open_media(const wchar_t* media) = 0;
};
