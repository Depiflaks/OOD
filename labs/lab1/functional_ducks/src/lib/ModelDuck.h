#ifndef MODEL_DUCK_H
#define MODEL_DUCK_H

#include "Duck.h"

#include <cassert>
#include <iostream>

class ModelDuck : public Duck
{
public:
	ModelDuck()
		: Duck(
			  FlyBehaviorFactory::CreateFlyNoWay(),
			  QuackBehaviorFactory::CreateQuackBehavior(),
			  DanceBehaviorFactory::CreateNoDance())
	{
	}

	void Display() const override
	{
		std::cout << "I'm model duck" << std::endl;
	}
};

#endif
