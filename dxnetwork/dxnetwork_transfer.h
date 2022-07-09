#pragma once
#include "interface.h"
class dxnetwork_transfer :
	public i_dxnetwork_transfer
{
public:

	void listen(const char* ip, ushort port, int& result) override;
	void connect(const char* ip, ushort port, int& result) override;

private:

};
