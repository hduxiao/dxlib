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

void dxmedia_reader::open_media(const wchar_t* media, dxmedia& media_info, bool useDXVA)
{
	if (media)
	{
		m_pReader = NULL;

		CComPtr<IMFAttributes> pReaderAttr;

		MFCreateAttributes(&pReaderAttr, 1);
		pReaderAttr->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);
		if (useDXVA)
		{
			if (!m_pDXGIManager)
				CreateDXGIManagerAndDevice();

			CComPtr<IUnknown> unknown;
			m_pDXGIManager->QueryInterface(IID_PPV_ARGS(&unknown));
			pReaderAttr->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, unknown);

			pReaderAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
			pReaderAttr->SetUINT32(MF_SOURCE_READER_DISABLE_DXVA, FALSE);
		}

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

void dxmedia_reader::read_sample(const int stream_index, int& actual_index, dxframe& frame)
{
	DWORD    dwStreamIndex = 0;
	DWORD    dwActualStreamIndex = 0;
	DWORD    dwStreamFlags = 0;
	LONGLONG llTimestamp = 0;
	LONGLONG llDuration = 0;
	CComPtr<IMFSample> pMFSample;
	CComPtr<IMFMediaBuffer> media_buffer;
	CComPtr<IMF2DBuffer2> media_buffer2;

	if (m_pReader == nullptr)
		goto done;

	if (-1 == stream_index)
	{
		dwStreamIndex = MF_SOURCE_READER_ANY_STREAM;
	}
	else if (-2 == stream_index)
	{
		dwStreamIndex = MF_SOURCE_READER_FIRST_VIDEO_STREAM;
	}
	else if (-3 == stream_index)
	{
		dwStreamIndex = MF_SOURCE_READER_FIRST_AUDIO_STREAM;
	}
	else if (stream_index >= 0 && stream_index < m_media.stream_num)
	{
		dwStreamIndex = stream_index;
	}
	else
	{
		goto done;
	}

	m_pReader->ReadSample(
		dwStreamIndex,
		0,
		&dwActualStreamIndex,
		&dwStreamFlags,
		&llTimestamp,
		&pMFSample);

	if (dwStreamFlags & MF_SOURCE_READERF_ERROR) {
		throw std::exception();
	}

	if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
		goto done;
	}

	actual_index = dwActualStreamIndex;

	pMFSample->GetSampleDuration(&llDuration);
	frame.frame_time = llTimestamp;
	frame.duration = llDuration;

	pMFSample->ConvertToContiguousBuffer(&media_buffer);

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

	frame.width = m_stream[actual_index].frame_width;
	frame.height = m_stream[actual_index].frame_height;
	frame.stride = frame.width * 3/*RGB24*/;

	return;

done:
	actual_index = -1;
}

void dxmedia_reader::set_position(long long position)
{
	if (m_pReader)
	{
		PROPVARIANT varPosition{};
		varPosition.vt = VT_I8;
		varPosition.hVal.QuadPart = position;

		m_pReader->SetCurrentPosition(GUID_NULL, varPosition);
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

			VideoType out_type(pNativeType);
			out_type.SetSubType(MFVideoFormat_RGB24);
			//out_type.SetSubType(MFVideoFormat_RGB32);
			m_pReader->SetCurrentMediaType((DWORD)stream.index, 0, out_type);
		}
		else if (dxstream::type::audio == stream.type)
		{
			CComPtr<IMFMediaType> pNativeType;
			m_pReader->GetNativeMediaType((DWORD)stream.index, 0, &pNativeType);

			AudioType out_type(pNativeType);
			out_type.SetSubType(MFAudioFormat_PCM);
			out_type.SetSamplesPerSecond(DEFAULT_AUDIO_SAMPLE_RATE);
			out_type.SetBitsPerSample(DEFAULT_BITS_PER_AUDIOSAMPLE);
			out_type.SetNumChannels(DEFAULT_AUDIO_NUM_CHANNELS);
			m_pReader->SetCurrentMediaType((DWORD)stream.index, 0, out_type);
		}
	}
}

HRESULT dxmedia_reader::CreateDXGIManagerAndDevice()
{
	HRESULT hr = S_OK;

	CComPtr<IDXGIAdapter> pTempAdapter;
	CComPtr<ID3D10Multithread> pMultiThread;
	CComPtr<IDXGIDevice1> pDXGIDev;
	CComPtr<IDXGIAdapter1> pAdapter;
	CComPtr<IDXGIOutput> pDXGIOutput;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL featureLevel;
	UINT resetToken;

	do
	{
		for (DWORD dwCount = 0; dwCount < ARRAYSIZE(featureLevels); dwCount++)
		{
			m_pD3D11Device = NULL;
			hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_VIDEO_SUPPORT, &featureLevels[dwCount], 1, D3D11_SDK_VERSION, &m_pD3D11Device, &featureLevel, NULL);
			if (SUCCEEDED(hr))
			{
				CComPtr<ID3D11VideoDevice> pDX11VideoDevice = NULL;
				hr = m_pD3D11Device->QueryInterface(__uuidof(ID3D11VideoDevice), (void**)&pDX11VideoDevice);

				if (SUCCEEDED(hr))
				{
					break;
				}
			}
		}

		if (FAILED(hr))
		{
			break;
		}

		if (NULL == m_pDXGIManager)
		{
			hr = MFCreateDXGIDeviceManager(&resetToken, &m_pDXGIManager);
			if (FAILED(hr))
			{
				break;
			}
			m_DeviceResetToken = resetToken;
		}

		hr = m_pDXGIManager->ResetDevice(m_pD3D11Device, m_DeviceResetToken);
		if (FAILED(hr))
		{
			break;
		}

		// Need to explitly set the multithreaded mode for this device
		hr = m_pD3D11Device->QueryInterface(__uuidof(ID3D10Multithread), (void**)&pMultiThread);
		if (FAILED(hr))
		{
			break;
		}

		pMultiThread->SetMultithreadProtected(TRUE);

	} while (FALSE);

	return hr;
}
