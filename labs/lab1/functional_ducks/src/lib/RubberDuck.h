#ifndef RUBBER_DUCK_H
#define RUBBER_DUCK_H

#include "Duck.h"

#include <cassert>
#include <iostream>

class RubberDuck : public Duck
{
public:
	RubberDuck()
		: Duck(
			  FlyBehaviorFactory::CreateFlyNoWay(),
			  QuackBehaviorFactory::CreateSqueakBehavior(),
			  DanceBehaviorFactory::CreateNoDance())
	{
	}

	void Display() const override
	{
		std::cout << "I'm rubber duck" << std::endl;
	}
};

#endif
