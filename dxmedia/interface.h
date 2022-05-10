#pragma once

class i_dxmedia_reader
{
	virtual void read_media() = 0;
};

extern "C" void __stdcall create_dxmedia_reader(i_dxmedia_reader * *pReader);
