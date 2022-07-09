#pragma once
#include "interface.h"
class dxnetwork_factory :
	public i_dxnetwork_factory
{
public:
	dxnetwork_factory();
	~dxnetwork_factory();

	// i_dxnetwork_factory
	void create_dxnetwork_object(ushort object, i_dxnetwork_object** ppObject) override;
};
