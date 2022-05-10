#include "pch.h"
#include "dxmedia_reader.h"

void __stdcall create_dxmedia_reader(i_dxmedia_reader** pReader)
{
	if (pReader)
	{
		*pReader = new dxmedia_reader;
	}
}

void dxmedia_reader::read_media()
{
}
