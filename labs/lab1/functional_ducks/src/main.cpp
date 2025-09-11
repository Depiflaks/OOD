#include <iostream>

#include "lib/Duck.h"

int main()
{
	Duck duck{ FlyBehaviorFactory::CreateFlyByWings(), QuackBehaviorFactory::CreateSqueakBehavior(), DanceBehaviorFactory::CreateDanceWaltz() } std::cout << "123\n";
	
}
