#pragma once
#include "interface.h"
class dxmedia_reader :
    public i_dxmedia_reader
{
public:
    void read_media() override;
};

