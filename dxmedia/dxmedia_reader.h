#pragma once
#include "interface.h"
class dxmedia_reader :
    public i_dxmedia_reader
{
public:

    void open_media(const wchar_t* media, int& stream_num) override;
    void get_stream(unsigned int stream_index, dxstream& stream_info) override;
    void get_sample(unsigned int stream_index, long long frame_time, dxframe& frame) override;

private:

    void get_mediainfo();
    void set_mediatype();

    dxmedia m_media;
    std::vector<dxstream> m_stream;
    CComPtr<IMFSourceReader> m_pReader;
};
