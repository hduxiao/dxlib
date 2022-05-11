#include "pch.h"
#include "dxmedia_factory.h"
#include "dxmedia_reader.h"
dxmedia_factory::dxmedia_factory()
{
	MFStartup(MF_VERSION);
}
dxmedia_factory::~dxmedia_factory()
{
	MFShutdown();
}
void dxmedia_factory::create_dxmedia_reader(i_dxmedia_reader** pReader)
{
	if (pReader)
	{
		*pReader = new dxmedia_reader;
	}
}
