#include "pch.h"
#include "dxmedia_factory.h"
#include "dxmedia_reader.h"
#include "dxmedia_writer.h"

void __stdcall create_dxmedia_factory(i_dxmedia_factory** pFactory)
{
	if (pFactory)
	{
		*pFactory = new dxmedia_factory;
	}
}

dxmedia_factory::dxmedia_factory()
{
	MFStartup(MF_VERSION);
}

dxmedia_factory::~dxmedia_factory()
{
	MFShutdown();
}

void dxmedia_factory::create_dxmedia_object(ushort object, i_dxmedia_object** ppObject)
{
	if (ppObject)
	{
		switch (object)
		{

		case DXMEDIA_READER:
		{
			*ppObject = new dxmedia_reader;
			break;
		}

		case DXMEDIA_WRITER:
		{
			*ppObject = new dxmedia_writer;
			break;
		}

		default:
			break;

		}
	}
}
