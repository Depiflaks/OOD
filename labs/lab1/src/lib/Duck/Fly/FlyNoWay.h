#ifndef FLYNOWAY_H
#define FLYNOWAY_H

#include "IFlyBehavior.h"

class FlyNoWay : public IFlyBehavior
{
public:
	void Fly() override {}

	bool CanFly() const override
	{
		return false;
	}

	int GetFlyCount() const override
	{
		return m_flyCount;
	}

private:
	int m_flyCount{ 0 };
};

#endif
