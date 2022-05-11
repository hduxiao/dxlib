#pragma once
#include "i_dxmedia_reader.h"
class i_dxmedia_factory
{
public:
	virtual ~i_dxmedia_factory() {}
	virtual void create_dxmedia_reader(i_dxmedia_reader** pReader) = 0;
};
