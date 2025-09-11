#ifndef MALLARD_DUCK_H
#define MALLARD_DUCK_H

#include "Duck.h"

#include <cassert>
#include <iostream>

class MallardDuck : public Duck
{
public:
	MallardDuck()
		: Duck(
			  FlyBehaviorFactory::CreateFlyByWings(),
			  QuackBehaviorFactory::CreateQuackBehavior(),
			  DanceBehaviorFactory::CreateDanceWaltz())
	{
	}

	void Display() const override
	{
		std::cout << "I'm mallard duck" << std::endl;
	}
};

#endif
