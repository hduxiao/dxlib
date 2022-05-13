#pragma once
#include "i_dxmedia_factory.h"
#include "i_dxmedia_struct.h"
#include "i_dxmedia_reader.h"

#define DXMEDIA_READER 10000
#define DXMEDIA_WRITER 10001

extern "C" void __stdcall create_dxmedia_factory(i_dxmedia_factory * *pFactory);
