#include "pch.h"
#include "dxmedia_reader.h"

void __stdcall create_dxmedia_reader(i_dxmedia_reader** pReader)
{
	if (pReader)
	{
		*pReader = new dxmedia_reader;
	}
}

void __stdcall startup_mediafoundation()
{
	MFStartup(MF_VERSION);
}

void __stdcall shutdown_mediafoundation()
{
	MFShutdown();
}
