#pragma once
#include "dxmedia_struct.h"
#include "i_dxmedia_reader.h"

extern "C" void __stdcall create_dxmedia_reader(i_dxmedia_reader * *pReader);

extern "C" void __stdcall startup_mediafoundation();

extern "C" void __stdcall shutdown_mediafoundation();
