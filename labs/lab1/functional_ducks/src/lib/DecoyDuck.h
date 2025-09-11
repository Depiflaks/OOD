#ifndef DECOY_DUCK_H
#define DECOY_DUCK_H

#include "Duck.h"

#include <cassert>
#include <iostream>

class DecoyDuck : public Duck
{
public:
	DecoyDuck()
		: Duck(
			  FlyBehaviorFactory::CreateFlyNoWay(),
			  QuackBehaviorFactory::CreateMuteQuackBehavior(),
			  DanceBehaviorFactory::CreateNoDance())
	{
	}

	void Display() const override
	{
		std::cout << "I'm decoy duck" << std::endl;
	}
};

#endif
