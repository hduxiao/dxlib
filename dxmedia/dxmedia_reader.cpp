#include "pch.h"
#include "dxmedia_reader.h"

void dxmedia_reader::open_media(const wchar_t* media, unsigned int& stream_num)
{
	if (media)
	{
		m_pReader = NULL;

		CComPtr<IMFAttributes> pReaderAttr;

		MFCreateAttributes(&pReaderAttr, 1);
		pReaderAttr->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);

		HRESULT hr = MFCreateSourceReaderFromURL(media, pReaderAttr, &m_pReader);
		if (SUCCEEDED(hr))
		{
			get_mediainfo();
			set_mediatype();

			stream_num = (unsigned int)m_stream.size();
		}
	}
}

void dxmedia_reader::get_stream_info(unsigned int stream_index, dxstream& stream_info)
{
	if (stream_index < m_stream.size())
	{
		stream_info = m_stream[stream_index];
	}
}

void dxmedia_reader::get_sample(unsigned int stream_index, long long frame_time, dxframe& frame)
{
	if (stream_index < m_stream.size())
	{
		if (frame_time < 0)
		{
			DWORD    dwActualStreamIndex = 0;
			DWORD    dwStreamFlags = 0;
			LONGLONG llTimestamp = 0;

			CComPtr<IMFSample> pMFSample;
			m_pReader->ReadSample(stream_index, 0,
				&dwActualStreamIndex,
				&dwStreamFlags,
				&llTimestamp,
				&pMFSample);

			if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
				return;
			}

			LONGLONG llDuration = 0;
			pMFSample->GetSampleDuration(&llDuration);
			frame.frame_time = llTimestamp;
			frame.duration = llDuration;

			CComPtr<IMFMediaBuffer> media_buffer;
			pMFSample->ConvertToContiguousBuffer(&media_buffer);

			if (dxstream::type::video == m_stream[stream_index].type)
			{
				CComPtr<IMF2DBuffer2> media_buffer2;
				media_buffer->QueryInterface(IID_PPV_ARGS(&media_buffer2));
				DWORD cbBufferLength = 0;
				BYTE* pbScanline0 = NULL;
				LONG lPitch = 0;
				BYTE* pbBufferStart = NULL;
				media_buffer2->Lock2DSize(MF2DBuffer_LockFlags_Read,
					&pbScanline0, &lPitch, &pbBufferStart, &cbBufferLength);
				if (cbBufferLength > 0)
				{
					frame.data_ptr = new byte[cbBufferLength]();
					memcpy(frame.data_ptr, pbBufferStart, cbBufferLength);
					frame.frame_size = cbBufferLength;
					frame.width = m_stream[stream_index].frame_width;
					frame.height = m_stream[stream_index].frame_height;
					frame.stride = frame.width * 4/*RGB32*/;
					
				}
				media_buffer2->Unlock2D();
			}
			else
			{
				DWORD cbBufferLength = 0;
				media_buffer->GetCurrentLength(&cbBufferLength);
				frame.data_ptr = new byte[cbBufferLength]();

				BYTE* pData = NULL;
				media_buffer->Lock(&pData, NULL, NULL);
				memcpy(frame.data_ptr, pData, cbBufferLength);
				frame.frame_size = cbBufferLength;
				media_buffer->Unlock();
			}
		}
	}
}

void dxmedia_reader::get_mediainfo()
{
	m_stream.clear();

	DWORD dwStreamIndex = 0;
	while (true)
	{
		CComPtr<IMFMediaType> pCurrentType;
		HRESULT hr = m_pReader->GetCurrentMediaType(dwStreamIndex, &pCurrentType);
		if (FAILED(hr))
			break;

		// set stream index
		dxstream stream;
		stream.index = dwStreamIndex++;

		GUID major_type{};
		pCurrentType->GetMajorType(&major_type);
		if (MFMediaType_Video == major_type)
		{ // video stream
			stream.type = dxstream::type::video;

			// get duration
			PROPVARIANT varAttribute;
			m_pReader->GetPresentationAttribute(
				MF_SOURCE_READER_MEDIASOURCE,
				MF_PD_DURATION,
				&varAttribute);
			stream.duration = varAttribute.uhVal.QuadPart;

			// get frame size
			UINT32 width = 0, height = 0;
			MFGetAttributeSize(pCurrentType, MF_MT_FRAME_SIZE, &width, &height);
			stream.frame_width = width;
			stream.frame_height = height;

			// get framerate
			UINT32 nume = 0, deno = 0;
			MFGetAttributeRatio(pCurrentType, MF_MT_FRAME_RATE, &nume, &deno);
			stream.framerate_nume = nume;
			stream.framerate_deno = deno;
		}
		else if (major_type == MFMediaType_Audio)
		{ // audio stream
			stream.type = dxstream::type::audio;

			// get audio info
			stream.channels = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_NUM_CHANNELS, 0);
			stream.sample_rate = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
			stream.bits_per_sample = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			stream.channel_mask = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_CHANNEL_MASK, 0);
			stream.block_alignment =
				MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_BLOCK_ALIGNMENT, UINT32(stream.channels * (stream.bits_per_sample / 8)));
		}
		else
		{ // other stream, not process
			continue;
		}

		m_stream.push_back(stream);
	}

}

void dxmedia_reader::set_mediatype()
{
	for (const auto& stream : m_stream)
	{
		if (dxstream::type::video == stream.type)
		{
			CComPtr<IMFMediaType> pNativeType;
			m_pReader->GetNativeMediaType((DWORD)stream.index, 0, &pNativeType);
			pNativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
			m_pReader->SetCurrentMediaType((DWORD)stream.index, 0, pNativeType);
		}
		else if (dxstream::type::audio == stream.type)
		{
			CComPtr<IMFMediaType> pNativeType;
			m_pReader->GetNativeMediaType((DWORD)stream.index, 0, &pNativeType);
			pNativeType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			m_pReader->SetCurrentMediaType((DWORD)stream.index, 0, pNativeType);
		}
	}
}
