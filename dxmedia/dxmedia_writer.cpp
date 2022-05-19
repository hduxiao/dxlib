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

void dxmedia_writer::set_stream(const dxstream& in_stream,
	const dxstream& out_stream, int& stream_index)
{
	if (m_pWriter)
	{

	}
}
