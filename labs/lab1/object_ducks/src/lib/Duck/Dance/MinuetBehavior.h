#ifndef MINUETBEHAVIOR_H
#define MINUETBEHAVIOR_H

#include "IDanceBehavior.h"
#include <iostream>

class MinuetBehavior : public IDanceBehavior
{
public:
	void Dance() override
	{
		std::cout << "I'm dancing minuet!" << std::endl;
	}
};

#endif
