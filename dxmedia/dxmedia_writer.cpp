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

void dxmedia_writer::add_stream(const dxstream& in, const dxstream& out, int& index)
{
	if (m_pWriter)
	{
		HRESULT hr = S_OK;
		DWORD dwStreamIndex{};

		if (dxstream::type::video == out.type)
		{
			VideoType out_stream_type;
			out_stream_type.CreateEmptyType();
			out_stream_type.SetSubType(MFVideoFormat_H264);
			out_stream_type.SetFrameDimensions((UINT32)out.frame_width, (UINT32)out.frame_height);
			out_stream_type.SetFrameRate((UINT32)out.framerate_nume, (UINT32)out.framerate_deno);
			out_stream_type.SetInterlaceMode(MFVideoInterlace_Progressive);

			if (out.avg_bitrate > 0)
			{
				out_stream_type.SetAvgerageBitRate((UINT32)out.avg_bitrate);
			}
			else
			{
				UINT32 avg_1080 = 3500'000;
				FLOAT ratio = FLOAT(out.frame_width * out.frame_height) / (1920 * 1080);
				UINT32 avg_bitrate = UINT32(avg_1080 * ratio);
				out_stream_type.SetAvgerageBitRate(avg_bitrate);
			}

			hr = m_pWriter->AddStream(out_stream_type, &dwStreamIndex);
			if (SUCCEEDED(hr))
			{
				VideoType in_stream_type;
				in_stream_type.CreateEmptyType();
				in_stream_type.SetSubType(MFVideoFormat_RGB24);
				in_stream_type.SetFrameDimensions((UINT32)in.frame_width, (UINT32)in.frame_height);
				in_stream_type.SetFrameRate((UINT32)in.framerate_nume, (UINT32)in.framerate_deno);

				LONG lStride = 0;
				in_stream_type.GetDefaultStride(&lStride);
				in_stream_type.SetDefaultStride(-lStride);

				//UINT32 width = 1080;
				//MFGetStrideForBitmapInfoHeader(MFVideoFormat_RGB32.Data1, width, &lStride);

				hr = m_pWriter->SetInputMediaType(dwStreamIndex, in_stream_type, NULL);
			}
		}
		else if (dxstream::type::audio == out.type)
		{
			AudioType out_stream_type;
			out_stream_type.CreateEmptyType();
			out_stream_type.SetSubType(MFAudioFormat_AAC);
			out_stream_type.SetSamplesPerSecond(DEFAULT_AUDIO_SAMPLE_RATE);
			out_stream_type.SetBitsPerSample(DEFAULT_BITS_PER_AUDIOSAMPLE);
			out_stream_type.SetNumChannels(DEFAULT_AUDIO_NUM_CHANNELS);

			hr = m_pWriter->AddStream(out_stream_type, &dwStreamIndex);
			if (SUCCEEDED(hr))
			{
				AudioType in_stream_type;
				in_stream_type.CreateEmptyType();
				in_stream_type.SetSubType(MFAudioFormat_PCM);
				in_stream_type.SetSamplesPerSecond(DEFAULT_AUDIO_SAMPLE_RATE);
				in_stream_type.SetBitsPerSample(DEFAULT_BITS_PER_AUDIOSAMPLE);
				in_stream_type.SetNumChannels(DEFAULT_AUDIO_NUM_CHANNELS);

				hr = m_pWriter->SetInputMediaType(dwStreamIndex, in_stream_type, NULL);
			}
		}
		else
		{
			hr = E_UNEXPECTED;
		}

		if (SUCCEEDED(hr))
		{
			index = dwStreamIndex;
			m_stream.push_back(out);
		}
		else
		{
			index = -1;
		}
	}
}

void dxmedia_writer::write_sample(int stream_index, dxframe& frame)
{
	if (stream_index < m_stream.size())
	{
		IMFSample* pSample = NULL;
		IMFMediaBuffer* pBuffer = NULL;

		const DWORD cbBuffer = (DWORD)frame.frame_size;

		BYTE* pData = NULL;

		// Create a new memory buffer.
		HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);

		// Lock the buffer and copy the video frame to the buffer.
		if (SUCCEEDED(hr))
		{
			hr = pBuffer->Lock(&pData, NULL, NULL);
		}
		if (SUCCEEDED(hr))
		{
			memcpy(pData, frame.data_ptr, cbBuffer);
		}
		if (pBuffer)
		{
			pBuffer->Unlock();
		}

		// Set the data length of the buffer.
		if (SUCCEEDED(hr))
		{
			hr = pBuffer->SetCurrentLength(cbBuffer);
		}

		// Create a media sample and add the buffer to the sample.
		if (SUCCEEDED(hr))
		{
			hr = MFCreateSample(&pSample);
		}
		if (SUCCEEDED(hr))
		{
			hr = pSample->AddBuffer(pBuffer);
		}

		// Set the time stamp and the duration.
		if (SUCCEEDED(hr))
		{
			hr = pSample->SetSampleTime(frame.frame_time);
		}
		if (SUCCEEDED(hr))
		{
			hr = pSample->SetSampleDuration(frame.duration);
		}

		// Send the sample to the Sink Writer.
		if (SUCCEEDED(hr))
		{
			hr = m_pWriter->WriteSample(stream_index, pSample);
		}

		SAFE_RELEASE(pSample);
		SAFE_RELEASE(pBuffer);
	}
}

void dxmedia_writer::begin_writing()
{
	if (m_pWriter)
	{
		m_pWriter->BeginWriting();
	}
}

void dxmedia_writer::finalize()
{
	if (m_pWriter)
	{
		m_pWriter->Finalize();
	}
}
