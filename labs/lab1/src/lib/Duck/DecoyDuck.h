#ifndef DECOYDUCK_H
#define DECOYDUCK_H

#include "Dance/NoDanceBehavior.h"
#include "Duck.h"
#include "Fly/FlyNoWay.h"
#include "Quack/MuteQuackBehavior.h"
#include <iostream>
#include <memory>

class DecoyDuck : public Duck
{
public:
	DecoyDuck()
		: Duck(
			  std::make_unique<FlyNoWay>(),
			  std::make_unique<MuteQuackBehavior>(),
			  std::make_unique<NoDanceBehavior>())
	{
	}

	void Display() const override
	{
		std::cout << "I'm decoy duck" << std::endl;
	}
};

#endif
