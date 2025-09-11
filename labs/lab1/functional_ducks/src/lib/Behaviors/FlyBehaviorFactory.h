#ifndef FLY_BEHAVIOR_FACTORY_H
#define FLY_BEHAVIOR_FACTORY_H

#include <cassert>
#include <functional>
#include <iostream>

using FlyBehavior = std::function<int()>;

class FlyBehaviorFactory final
{
public:
	static FlyBehavior CreateFlyByWings()
	{
		return [flyCount = 0]() mutable -> int {
			std::cout << "Fly with wings!!";
			return ++flyCount;
		};
	}

	static FlyBehavior CreateFlyNoWay()
	{
		return []() mutable -> int {
			return 0;
		};
	}

private:
	FlyBehaviorFactory()
	{
	}
};

#endif
