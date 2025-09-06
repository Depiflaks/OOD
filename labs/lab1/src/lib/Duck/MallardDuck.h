#ifndef MALLARDDUCK_H
#define MALLARDDUCK_H

#include "Dance/WaltzBehavior.h"
#include "Duck.h"
#include "Fly/FlyWithWings.h"
#include "Quack/QuackBehavior.h"

class MallardDuck : public Duck
{
public:
	MallardDuck()
		: Duck(
			  std::make_unique<FlyWithWings>(),
			  std::make_unique<QuackBehavior>(),
			  std::make_unique<WaltzBehavior>())
	{
	}

	void Display() const override
	{
		std::cout << "I'm mallard duck" << std::endl;
	}
};

#endif
