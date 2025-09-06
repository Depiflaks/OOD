#ifndef FLYWITHWINGS_H
#define FLYWITHWINGS_H

#include "IFlyBehavior.h"
#include <iostream>

class FlyWithWings : public IFlyBehavior
{
public:
	void Fly() override
	{
		m_flyCount += 1;
		std::cout << "I'm flying with wings!!" << std::endl;
	}

	bool CanFly() const override
	{
		return true;
	}

	int GetFlyCount() const override
	{
		return m_flyCount;
	}

private:
	int m_flyCount{ 0 };
};

#endif
