#pragma once
class i_dxmedia_reader
{
public:
	virtual void open_media(const wchar_t* media) = 0;
};