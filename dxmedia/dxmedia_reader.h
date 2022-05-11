#pragma once
#include "i_dxmedia_reader.h"
#include "dxmedia_struct.h"
#include <vector>
class dxmedia_reader :
    public i_dxmedia_reader
{
public:

    void open_media(const wchar_t* media) override;

private:

    void get_mediainfo();

    dxmedia m_media;
    std::vector<dxstream> m_stream;
    CComPtr<IMFSourceReader> m_pReader;
};
