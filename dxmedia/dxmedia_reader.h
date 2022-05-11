#pragma once
#include "interface.h"
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

