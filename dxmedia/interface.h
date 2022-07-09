#pragma once

#define DXMEDIA_READER 10000
#define DXMEDIA_WRITER 10001

using uint = unsigned int;
using uchar = unsigned char;
using ushort = unsigned short;
using llong = long long;

static const uint DEFAULT_AUDIO_NUM_CHANNELS = 2;  // Stereo
static const uint DEFAULT_AUDIO_SAMPLE_RATE = 48000;  // 48000 Hz
static const uint DEFAULT_BITS_PER_AUDIOSAMPLE = 16;  // 16 bit

struct dxmedia
{
	llong stream_num = 0;
	llong first_video_stream = 0;
	llong first_audio_stream = 0;
	llong duration = 0;
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

	llong   index = 0;
	type        type = type::none;
	llong   avg_bitrate = 0;
	// video attribute
	uint   frame_width = 0;
	uint   frame_height = 0;
	llong   framerate_nume = 0;
	llong   framerate_deno = 0;
	// audio attribute
	llong   channels = 0;
	llong   sample_rate = 0;
	llong   bits_per_sample = 0;
};

struct dxframe
{
	llong    frame_size = 0;
	llong    frame_time = 0;
	llong    duration = 0;
	llong    stride = 0;
	llong    height = 0;
	llong    width = 0;
	uchar* data_ptr = nullptr;

	void release()
	{
		if (data_ptr)
		{
			delete[] data_ptr;
			data_ptr = nullptr;
		}
	}
};

class i_dxmedia_object {};

class i_dxmedia_factory
{
public:
	virtual ~i_dxmedia_factory() {}
	virtual void create_dxmedia_object(ushort object, i_dxmedia_object** ppObject) = 0;
};

class i_dxmedia_reader :
	public i_dxmedia_object
{
public:
	virtual ~i_dxmedia_reader() {}
	virtual void open_media(const wchar_t* media, dxmedia& media_info, bool useDXVA = false) = 0;
	virtual void get_stream(int stream_index, dxstream& stream_info) = 0;
	virtual void SetStreamOutput(int stream_index, const dxstream& streamType) = 0;
	virtual void read_sample(const int stream_index, int& actual_index, dxframe& frame) = 0;
	virtual void set_position(llong position) = 0;
};

class i_dxmedia_writer :
	public i_dxmedia_object
{
public:
	virtual ~i_dxmedia_writer() {}
	virtual void create_media(const wchar_t* media) = 0;
	virtual void add_stream(const dxstream& in, const dxstream& out, int& index) = 0;
	virtual void write_sample(int stream_index, dxframe& frame) = 0;
	virtual void begin_writing() = 0;
	virtual void finalize() = 0;
};

extern "C"
void __stdcall create_dxmedia_factory(i_dxmedia_factory * *pFactory);