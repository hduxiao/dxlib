#pragma once

#include <iostream>

#define DXMEDIA_READER 10000
#define DXMEDIA_WRITER 10001

using uchar = unsigned char;

struct dxmedia
{
	long long stream_num = 0;
	long long first_video_stream = 0;
	long long first_audio_stream = 0;
	long long duration = 0;
	bool have_video_stream = false;
	bool have_audio_stream = false;
};

struct dxstream
{
	enum class type
	{
		audio,
		video,
		none
	};

	long long   index = 0;
	type        type = type::none;
	long long   avg_bitrate = 0;
	// video attribute
	long long   frame_width = 0;
	long long   frame_height = 0;
	long long   framerate_nume = 0;
	long long   framerate_deno = 0;
	// audio attribute
	long long   channels = 0;
	long long   sample_rate = 0;
	long long   bits_per_sample = 0;
};

struct dxframe
{
	long long    datasize = 0;
	long long    timestamp = 0;
	long long    duration = 0;
	long long    stride = 0;
	long long    height = 0;
	long long    width = 0;
	uchar* data_ptr = nullptr;

	dxframe& operator=(const dxframe& dxf)
	{
		// first release
		release();

		// then allocate
		*this = dxf;
		this->data_ptr = new uchar[dxf.datasize]();
		memcpy(this->data_ptr, dxf.data_ptr, dxf.datasize);
	}

	dxframe& operator=(dxframe&& dxf) noexcept
	{
		// first release
		release();

		// then move
		*this = dxf;
		dxf.reset();
	}

	~dxframe()
	{
		release();
	}

	void release()
	{
		if (data_ptr)
		{
			delete[] data_ptr;
			data_ptr = nullptr;
		}
	}

	void reset()
	{
		datasize = {};
		timestamp = {};
		duration = {};
		stride = {};
		height = {};
		width = {};
		data_ptr = {};
	}
};

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
	virtual void open_media(const wchar_t* media, dxmedia& media_info) = 0;
	virtual void get_stream(int stream_index, dxstream& stream_info) = 0;
	virtual void read_sample(const int stream_index, int& actual_index, dxframe& frame) = 0;
	virtual void set_position(long long position) = 0;
};

class i_dxmedia_writer :
	public i_dxmedia_object
{
public:
	virtual ~i_dxmedia_writer() {}
	virtual void create_media(const wchar_t* media) = 0;
	virtual void add_stream(const dxstream& out_stream, int& stream_index) = 0;
	virtual void write_sample(int stream_index, dxframe& frame) = 0;
	virtual void begin_writing() = 0;
	virtual void finalize() = 0;
};

extern "C"
void __stdcall create_dxmedia_factory(i_dxmedia_factory * *pFactory);