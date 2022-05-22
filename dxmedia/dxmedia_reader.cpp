#include "pch.h"
#include "dxmedia_reader.h"

HRESULT EnumerateTypesForStream(IMFSourceReader* pReader, DWORD dwStreamIndex)
{
	HRESULT hr = S_OK;
	DWORD dwMediaTypeIndex = 0;

	while (SUCCEEDED(hr))
	{
		IMFMediaType* pType = NULL;
		hr = pReader->GetNativeMediaType(dwStreamIndex, dwMediaTypeIndex, &pType);
		if (hr == MF_E_NO_MORE_TYPES)
		{
			hr = S_OK;
			break;
		}
		else if (SUCCEEDED(hr))
		{
			LogMediaType(pType);

			pType->Release();
		}
		++dwMediaTypeIndex;
	}
	return hr;
}

void dxmedia_reader::open_media(const wchar_t* media, dxmedia& media_info)
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

			media_info = m_media;
		}
		else
		{
			media_info = dxmedia();
		}
	}
}

void dxmedia_reader::get_stream(int stream_index, dxstream& stream_info)
{
	if (stream_index < m_stream.size())
	{
		stream_info = m_stream[stream_index];
	}
}

void dxmedia_reader::read_sample(int& stream_index, dxframe& frame)
{
	if (m_pReader)
	{
		DWORD    dwActualStreamIndex = 0;
		DWORD    dwStreamFlags = 0;
		LONGLONG llTimestamp = 0;

		CComPtr<IMFSample> pMFSample;
		m_pReader->ReadSample(
			MF_SOURCE_READER_ANY_STREAM,
			0,
			&dwActualStreamIndex,
			&dwStreamFlags,
			&llTimestamp,
			&pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			stream_index = -1;
			return;
		}

		stream_index = dwActualStreamIndex;

		LONGLONG llDuration = 0;
		pMFSample->GetSampleDuration(&llDuration);
		frame.frame_time = llTimestamp;
		frame.duration = llDuration;

		CComPtr<IMFMediaBuffer> media_buffer;
		pMFSample->ConvertToContiguousBuffer(&media_buffer);

		CComPtr<IMF2DBuffer2> media_buffer2;
		media_buffer->QueryInterface(IID_PPV_ARGS(&media_buffer2));
		if (media_buffer2)
		{
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

		frame.width = m_stream[stream_index].frame_width;
		frame.height = m_stream[stream_index].frame_height;
		frame.stride = frame.width * 3/*RGB24*/;
	}
}

void dxmedia_reader::read_sample(const int& stream_index, dxframe& frame)
{
	if (stream_index < m_stream.size())
	{
		DWORD    dwActualStreamIndex = 0;
		DWORD    dwStreamFlags = 0;
		LONGLONG llTimestamp = 0;

		CComPtr<IMFSample> pMFSample;
		m_pReader->ReadSample(
			stream_index,
			0,
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

		CComPtr<IMF2DBuffer2> media_buffer2;
		media_buffer->QueryInterface(IID_PPV_ARGS(&media_buffer2));
		if (media_buffer2)
		{
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

		frame.width = m_stream[stream_index].frame_width;
		frame.height = m_stream[stream_index].frame_height;
		frame.stride = frame.width * 3/*RGB24*/;
	}
}

void dxmedia_reader::get_mediainfo()
{
	m_stream.clear();

	// get duration
	PROPVARIANT varAttribute;
	m_pReader->GetPresentationAttribute(
		MF_SOURCE_READER_MEDIASOURCE,
		MF_PD_DURATION,
		&varAttribute);
	m_media.duration = varAttribute.uhVal.QuadPart;

	DWORD dwStreamIndex = 0;
	while (true)
	{
		CComPtr<IMFMediaType> pCurrentType;
		HRESULT hr = m_pReader->GetCurrentMediaType(dwStreamIndex, &pCurrentType);
		if (FAILED(hr))
			break;

		//EnumerateTypesForStream(m_pReader, dwStreamIndex);

		// set stream index
		dxstream stream;
		stream.index = dwStreamIndex++;

		GUID major_type{};
		pCurrentType->GetMajorType(&major_type);
		if (MFMediaType_Video == major_type)
		{ // video stream
			stream.type = dxstream::type::video;

			VideoType vt(pCurrentType);
			UINT32 avg_bitrate = 0;
			vt.GetAverageBitRate(&avg_bitrate);
			stream.avg_bitrate = avg_bitrate;

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

			if (false == m_media.have_video_stream)
			{
				m_media.first_video_stream = stream.index;
				m_media.have_video_stream = true;
			}
		}
		else if (major_type == MFMediaType_Audio)
		{ // audio stream
			stream.type = dxstream::type::audio;

			// get audio info
			stream.channels = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_NUM_CHANNELS, 0);
			stream.sample_rate = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
			stream.bits_per_sample = MFGetAttributeUINT32(pCurrentType, MF_MT_AUDIO_BITS_PER_SAMPLE, 16);

			if (false == m_media.have_audio_stream)
			{
				m_media.first_audio_stream = stream.index;
				m_media.have_audio_stream = true;
			}
		}
		else
		{ // other stream, not process
			continue;
		}

		m_stream.push_back(stream);
	}

	m_media.stream_num = m_stream.size();
}

void dxmedia_reader::set_mediatype()
{
	for (const auto& stream : m_stream)
	{
		if (dxstream::type::video == stream.type)
		{
			CComPtr<IMFMediaType> pNativeType;
			m_pReader->GetNativeMediaType((DWORD)stream.index, 0, &pNativeType);
			pNativeType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
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
