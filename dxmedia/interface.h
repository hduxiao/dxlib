#pragma once

#include "dxmedia_struct.h"

#define DXMEDIA_READER 10000
#define DXMEDIA_WRITER 10001

class i_dxmedia_object {};

class i_dxmedia_factory
{
public:
	virtual ~i_dxmedia_factory() {}
	virtual void create_dxmedia_object(unsigned short object, i_dxmedia_object** ppObject) = 0;
};

class i_dxmedia_reader :
	public i_dxmedia_object
{
public:
	virtual ~i_dxmedia_reader() {}
	virtual void open_media(const wchar_t* media, int& stream_num) = 0;
	virtual void get_stream(unsigned int stream_index, dxstream& stream_info) = 0;
	virtual void get_sample(unsigned int stream_index, long long frame_time, dxframe& frame) = 0;
};

class i_dxmedia_writer :
	public i_dxmedia_object
{
public:
	virtual ~i_dxmedia_writer() {}
	virtual void create_media(const wchar_t* media) = 0;
	virtual void set_stream(const dxstream& out_stream, int& stream_index) = 0;
};

extern "C"
void __stdcall create_dxmedia_factory(i_dxmedia_factory * *pFactory);