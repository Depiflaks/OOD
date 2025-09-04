#include <iostream>
#include "PizzaStore.h"

int main()
{
	CNYPizzaStore store;
	CChicagoPizzaStore chicagoStore;
	//auto pizza = store.OrderPizza("cheese");
	//std::cout << pizza->ToString();

	auto clamPizza = chicagoStore.OrderPizza("clam");
	std::cout << clamPizza->ToString();

	CAshotsPizzaStore uAshota;
	auto lavash = uAshota.OrderPizza("It doesn't matter. Only lavash is produced");
	std::cout << lavash->ToString();
	return 0;
}