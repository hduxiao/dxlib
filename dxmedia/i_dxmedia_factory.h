#pragma once
class i_dxmedia_factory
{
public:
	virtual ~i_dxmedia_factory() {}
	virtual void create_dxmedia_object(unsigned short object, void** ppObject) = 0;
};
