#pragma once

struct dxmedia
{
	long long stream_num = 0;
	long long first_video_stream = 0;
	long long first_audio_stream = 0;
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
	// video attribute
	long long   duration = 0;
	long long   frame_width = 0;
	long long   frame_height = 0;
	long long   framerate_nume = 0;
	long long   framerate_deno = 0;
	// audio attribute
	long long   channels = 0;
	long long   sample_rate = 0;
	long long   bits_per_sample = 0;
	long long   block_alignment = 0;
	long long   channel_mask = 0;
};

struct dxframe
{
	long long    frame_size = 0;
	long long    frame_time = 0;
	long long    duration = 0;
	long long    stride = 0;
	long long    height = 0;
	long long    width = 0;
	void* data_ptr = nullptr;

	void release()
	{
		if (data_ptr)
		{
			delete[] data_ptr;
			data_ptr = nullptr;
		}
	}

	~dxframe() { release(); }
};
