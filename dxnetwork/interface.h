#pragma once

#define DXNETWORK_TRANSFER 10000

using uint = unsigned int;
using uchar = unsigned char;
using ushort = unsigned short;
using llong = long long;

class i_dxnetwork_object {};

class i_dxnetwork_factory
{
public:
	virtual ~i_dxnetwork_factory() {}
	virtual void create_dxnetwork_object(ushort object, i_dxnetwork_object** ppObject) = 0;
};

class i_dxnetwork_transfer :
	public i_dxnetwork_object
{
public:
	virtual ~i_dxnetwork_transfer() {}
	virtual void listen(const char* ip, ushort port, int& result) = 0;
	virtual void connect(const char* ip, ushort port, int& result) = 0;
};

extern "C"
void __stdcall create_dxnetwork_factory(i_dxnetwork_factory * *pFactory);
