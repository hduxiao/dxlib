#pragma once
#include "interface.h"
class dxmedia_factory :
	public i_dxmedia_factory
{
public:
	dxmedia_factory();
	~dxmedia_factory();

	// i_dxmedia_factory
	void create_dxmedia_object(ushort object, i_dxmedia_object** ppObject) override;
};
