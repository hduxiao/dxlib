#pragma once
#include "i_dxmedia_factory.h"
class dxmedia_factory :
	public i_dxmedia_factory
{
public:
	dxmedia_factory();
	~dxmedia_factory();

	// i_dxmedia_factory
	void create_dxmedia_reader(i_dxmedia_reader** pReader) override;
};
