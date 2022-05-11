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

	long long   stream_id = 0;
	type        stream_type = type::none;
	bool        isSelected = false;
	// video attribute
	long long   duration = 0;
	long long   frame_width = 0;
	long long   frame_height = 0;
	long long   framerate_nume = 0;
	long long   framerate_deno = 0;
	long long	display_aspect_ratio_nume = 0;
	long long	display_aspect_ratio_deno = 0;
	// audio attribute
	long long   channel_num = 0;
	long long   sample_rate = 0;
	long long   bits_per_sample = 0;
	long long   block_alignment = 0;
	long long   channel_mask = 0;
};
