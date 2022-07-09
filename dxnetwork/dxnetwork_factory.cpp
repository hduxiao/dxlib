#include "pch.h"
#include "dxnetwork_factory.h"
#include "dxnetwork_transfer.h"

void create_dxnetwork_factory(i_dxnetwork_factory** pFactory)
{
	if (pFactory)
	{
		*pFactory = new dxnetwork_factory;
	}
}

dxnetwork_factory::dxnetwork_factory()
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		// WSAStartup failed
	}
#endif
}

dxnetwork_factory::~dxnetwork_factory()
{
#ifdef _WIN32
	WSACleanup();
#endif
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
