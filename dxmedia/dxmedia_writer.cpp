#include "pch.h"
#include "dxmedia_writer.h"

void dxmedia_writer::create_media(const wchar_t* media)
{
	if (media)
	{
		m_pWriter = NULL;

		CComPtr<IMFAttributes> pWriterAttrs;

		MFCreateAttributes(&pWriterAttrs, 1);
		pWriterAttrs->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);

		HRESULT hr = MFCreateSinkWriterFromURL(media, NULL, NULL, &m_pWriter);
		if (SUCCEEDED(hr))
		{
		}
	}
}

void dxmedia_writer::set_stream(const dxstream& out_stream, int& stream_index)
{
	if (m_pWriter)
	{
		HRESULT hr = S_OK;
		DWORD dwStreamIndex{};

		if (dxstream::type::video == out_stream.type)
		{
			VideoType out_stream_type;
			out_stream_type.CreateEmptyType();
			out_stream_type.SetSubType(MFVideoFormat_H264);
			out_stream_type.SetFrameDimensions((UINT32)out_stream.frame_width, (UINT32)out_stream.frame_height);
			out_stream_type.SetFrameRate((UINT32)out_stream.framerate_nume, (UINT32)out_stream.framerate_deno);
			out_stream_type.SetAvgerageBitRate((UINT32)out_stream.avg_bitrate);
			out_stream_type.SetInterlaceMode(MFVideoInterlace_Progressive);

			hr = m_pWriter->AddStream(out_stream_type, &dwStreamIndex);
			if (SUCCEEDED(hr))
			{
				VideoType in_stream_type;
				in_stream_type.CreateEmptyType();
				in_stream_type.SetSubType(MFVideoFormat_RGB32);
				in_stream_type.SetFrameDimensions((UINT32)out_stream.frame_width, (UINT32)out_stream.frame_height);
				in_stream_type.SetFrameRate((UINT32)out_stream.framerate_nume, (UINT32)out_stream.framerate_deno);

				hr = m_pWriter->SetInputMediaType(dwStreamIndex, in_stream_type, NULL);
			}

		}
		else if (dxstream::type::audio == out_stream.type)
		{
			AudioType out_stream_type;
			out_stream_type.CreateEmptyType();
			out_stream_type.SetSubType(MFAudioFormat_AAC);
			out_stream_type.SetSamplesPerSecond((UINT32)out_stream.sample_rate);
			out_stream_type.SetBitsPerSample((UINT32)out_stream.bits_per_sample);
			out_stream_type.SetNumChannels((UINT32)out_stream.channels);

			hr = m_pWriter->AddStream(out_stream_type, &dwStreamIndex);
			if (SUCCEEDED(hr))
			{
				AudioType in_stream_type;
				in_stream_type.CreateEmptyType();
				in_stream_type.SetSubType(MFAudioFormat_PCM);
				in_stream_type.SetSamplesPerSecond((UINT32)out_stream.sample_rate);
				in_stream_type.SetBitsPerSample((UINT32)out_stream.bits_per_sample);
				in_stream_type.SetNumChannels((UINT32)out_stream.channels);

				hr = m_pWriter->SetInputMediaType(dwStreamIndex, in_stream_type, NULL);
			}
		}
		else
		{
			hr = E_UNEXPECTED;
		}

		if (SUCCEEDED(hr))
		{
			stream_index = dwStreamIndex;
		}
	}
}
