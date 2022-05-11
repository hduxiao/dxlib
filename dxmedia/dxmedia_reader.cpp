#include "pch.h"
#include "dxmedia_reader.h"

void dxmedia_reader::open_media(const wchar_t* media)
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
		}
	}
}

void dxmedia_reader::get_mediainfo()
{
}
