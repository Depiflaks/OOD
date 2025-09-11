#ifndef REDHEAD_DUCK_H
#define REDHEAD_DUCK_H

#include "Duck.h"

#include <cassert>
#include <iostream>

class RedHeadDuck : public Duck
{
public:
	RedHeadDuck()
		: Duck(
			  FlyBehaviorFactory::CreateFlyByWings(),
			  QuackBehaviorFactory::CreateQuackBehavior(),
			  DanceBehaviorFactory::CreateDanceMinuet())
	{
	}

	void Display() const override
	{
		std::cout << "I'm redhead duck" << std::endl;
	}
};

#endif
