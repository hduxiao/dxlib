#include "pch.h"
#include "dxmedia_factory.h"
void __stdcall create_dxmedia_factory(i_dxmedia_factory** pFactory)
{
	if (pFactory)
	{
		*pFactory = new dxmedia_factory;
	}
}
