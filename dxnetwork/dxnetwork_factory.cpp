#include "pch.h"
#include "dxnetwork_factory.h"
#include "dxnetwork_transfer.h"

void __stdcall create_dxnetwork_factory(i_dxnetwork_factory** pFactory)
{
	if (pFactory)
	{
		*pFactory = new dxnetwork_factory;
	}
}

dxnetwork_factory::dxnetwork_factory()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		// WSAStartup failed
	}
}

dxnetwork_factory::~dxnetwork_factory()
{
	WSACleanup();
}

void dxnetwork_factory::create_dxnetwork_object(ushort object, i_dxnetwork_object** ppObject)
{
	if (ppObject)
	{
		switch (object)
		{

		case DXNETWORK_TRANSFER:
		{
			*ppObject = new dxnetwork_transfer;
			break;
		}

		default:
			break;

		}
	}
}
