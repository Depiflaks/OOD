#include <iostream>

#include "lib/Duck.h"
#include "lib/MallardDuck.h"

int main()
{
	MallardDuck duck{};
	duck.Display();
	duck.Dance();
	duck.Fly();
	duck.Quack();
	std::cout << "\n";
	return 0;
}
